#include "d3d8_device.h"

#include "d3d8_interface.h"

#include "../d3d9/d3d9_device.h"
#include "../d3d9/d3d9_texture.h"
#include "../d3d9/d3d9_surface.h"
#include "../d3d9/d3d9_buffer.h"
#include "../d3d9/d3d9_stateblock.h"

#ifdef MSC_VER
#pragma fenv_access (on)
#endif

#define DXVK_D3D8_SHADER_BIT 0x80000000

namespace dxvk {

  // v0-v16, used by fixed function vs
  static constexpr int D3D8_NUM_VERTEX_INPUT_REGISTERS = 17;

  // standard mapping of vertx input v0-v16 to d3d9 usages and usage indices
  // (See D3DVSDE_ values in d3d8types.h or DirectX 8 docs for vertex shader input registers vn)
  static const BYTE D3D8_VERTEX_INPUT_REGISTERS[D3D8_NUM_VERTEX_INPUT_REGISTERS][2] = {
    {D3DDECLUSAGE_POSITION, 0},      // dcl_position     v0
    {D3DDECLUSAGE_BLENDWEIGHT, 0},   // dcl_blendweight  v1
    {D3DDECLUSAGE_BLENDINDICES, 0},  // dcl_blendindices v2
    {D3DDECLUSAGE_NORMAL, 0},        // dcl_normal       v3
    {D3DDECLUSAGE_PSIZE, 0},         // dcl_psize        v4
    {D3DDECLUSAGE_COLOR, 0},         // dcl_color        v5 ; diffuse
    {D3DDECLUSAGE_COLOR, 1},         // dcl_color1       v6 ; specular
    {D3DDECLUSAGE_TEXCOORD, 0},      // dcl_texcoord0    v7
    {D3DDECLUSAGE_TEXCOORD, 1},      // dcl_texcoord1    v8
    {D3DDECLUSAGE_TEXCOORD, 2},      // dcl_texcoord2    v9
    {D3DDECLUSAGE_TEXCOORD, 3},      // dcl_texcoord3    v10
    {D3DDECLUSAGE_TEXCOORD, 4},      // dcl_texcoord4    v11
    {D3DDECLUSAGE_TEXCOORD, 5},      // dcl_texcoord5    v12
    {D3DDECLUSAGE_TEXCOORD, 6},      // dcl_texcoord6    v13
    {D3DDECLUSAGE_TEXCOORD, 7},      // dcl_texcoord7    v14
    {D3DDECLUSAGE_POSITION, 1},      // dcl_position1    v15 ; position 2
    {D3DDECLUSAGE_NORMAL, 1},        // dcl_normal1      v16 ; normal 2
  };
  
  // width in bytes of each D3DDECLTYPE (dx9) or D3DVSDT (dx8)
  static const BYTE D3D9_DECL_TYPE_SIZES[MAXD3DDECLTYPE + 1] = {
    4,  // FLOAT1
    8,  // FLOAT2
    12, // FLOAT3
    16, // FLOAT4
    4,  // D3DCOLOR

    4,  // UBYTE4
    4,  // SHORT2
    8,  // SHORT4

    // The following are for vs2.0+ //
    4,  // UBYTE4N
    4,  // SHORT2N
    8,  // SHORT4N
    4,  // USHORT2N
    8,  // USHORT4N
    6,  // UDEC3
    6,  // DEC3N
    8,  // FLOAT16_2
    16, // FLOAT16_4

    0   // UNUSED
  };

  struct D3D8VertexShaderInfo {
    // Vertex Shader
    IDirect3DVertexDeclaration9*  pVertexDecl;
    IDirect3DVertexShader9*       pVertexShader;
  };

  D3D8Device::D3D8Device(D3D9DeviceEx* pParent)
    : m_d3d9(pParent) {

      // TODO Set d3d8 in device

    m_textures.fill(nullptr);

  }

  D3D8Device::~D3D8Device() {
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::QueryInterface(REFIID riid, void** ppvObject) {

    return S_OK;
    if (ppvObject == nullptr)
      return E_POINTER;

    *ppvObject = nullptr;

    if (riid == __uuidof(IUnknown)) {
    //  || riid == IID_d3d8::IDirect3DDevice8) {
      *ppvObject = ref(this);
      return S_OK;
    }

    Logger::warn("D3D8Device::QueryInterface: Unknown interface query");
    Logger::warn(str::format(riid));
    return E_NOINTERFACE;
  }

  ULONG STDMETHODCALLTYPE D3D8Device::AddRef() {
    return m_d3d9->AddRef();
  }

  ULONG STDMETHODCALLTYPE D3D8Device::Release() {
    return m_d3d9->Release();
  }


  HRESULT STDMETHODCALLTYPE D3D8Device::TestCooperativeLevel() {
    // Equivelant of D3D11/DXGI present tests. We can always present.
    return D3D_OK;
  }

  UINT STDMETHODCALLTYPE D3D8Device::GetAvailableTextureMem() {
    return m_d3d9->GetAvailableTextureMem();
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::ResourceManagerDiscardBytes(DWORD bytes) {
    return m_d3d9->EvictManagedResources();
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetDeviceCaps(d3d8::D3DCAPS8* pCaps) {
    D3DCAPS9 caps9;
    HRESULT res = m_d3d9->GetDeviceCaps(&caps9);
    dxvk::ConvertCaps8(caps9, pCaps);
    return res;
  }


  HRESULT STDMETHODCALLTYPE D3D8Device::GetDisplayMode(d3d8::D3DDISPLAYMODE* pMode) {
    // swap chain 0
    return m_d3d9->GetDisplayMode(0, (D3DDISPLAYMODE*)pMode);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetCreationParameters(d3d8::D3DDEVICE_CREATION_PARAMETERS* pParameters) {
    return m_d3d9->GetCreationParameters((D3DDEVICE_CREATION_PARAMETERS*)pParameters);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::SetCursorProperties(
    UINT               XHotSpot,
    UINT               YHotSpot,
    d3d8::IDirect3DSurface8* pCursorBitmap) {

    D3D8Surface* surf = static_cast<D3D8Surface*>(pCursorBitmap);
    return m_d3d9->SetCursorProperties(XHotSpot, YHotSpot, surf ? surf->GetD3D9Iface() : nullptr);
  }



  void STDMETHODCALLTYPE D3D8Device::SetCursorPosition(UINT XScreenSpace, UINT YScreenSpace, DWORD Flags) {
    // TODO: do we need to convert from screenspace?
    //GetD3D9Iface()->SetCursorPosition(XScreenSpace, YScreenSpace, Flags);
  }

  BOOL STDMETHODCALLTYPE D3D8Device::ShowCursor(BOOL bShow) {
    return m_d3d9->ShowCursor(bShow);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::Reset(d3d8::D3DPRESENT_PARAMETERS* pPresentationParameters) {
    D3DPRESENT_PARAMETERS params = ConvertPresentParameters9(pPresentationParameters);
    return m_d3d9->Reset(&params);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::Present(
    const RECT* pSourceRect,
    const RECT* pDestRect,
          HWND hDestWindowOverride,
    const RGNDATA* pDirtyRegion) {
    return m_d3d9->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetBackBuffer(
          UINT iBackBuffer,
          d3d8::D3DBACKBUFFER_TYPE Type,
          d3d8::IDirect3DSurface8** ppBackBuffer) {
    InitReturnPtr(ppBackBuffer);
    IDirect3DSurface9* pSurface9;
    HRESULT res = m_d3d9->GetBackBuffer(0, iBackBuffer, (D3DBACKBUFFER_TYPE)Type, &pSurface9);
    if (res != D3D_OK)
      return res;

    *ppBackBuffer = static_cast<D3D9Surface*>(pSurface9)->GetD3D8Iface();
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetRasterStatus(d3d8::D3DRASTER_STATUS* pRasterStatus) {
    return m_d3d9->GetRasterStatus(0, (D3DRASTER_STATUS*)pRasterStatus);
  }

  void STDMETHODCALLTYPE D3D8Device::SetGammaRamp(DWORD Flags, const d3d8::D3DGAMMARAMP* pRamp) {
    // For swap chain 0
    m_d3d9->SetGammaRamp(0, Flags, reinterpret_cast<const D3DGAMMARAMP*>(pRamp));
  }

  void STDMETHODCALLTYPE D3D8Device::GetGammaRamp(d3d8::D3DGAMMARAMP* pRamp) {
    // For swap chain 0
    m_d3d9->GetGammaRamp(0, reinterpret_cast<D3DGAMMARAMP*>(pRamp));
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::CreateTexture (
          UINT                Width,
          UINT                Height,
          UINT                Levels,
          DWORD               Usage,
          d3d8::D3DFORMAT           Format,
          d3d8::D3DPOOL             Pool,
          d3d8::IDirect3DTexture8** ppTexture) {
    InitReturnPtr(ppTexture);

    IDirect3DTexture9* pTex9;
    HRESULT res = m_d3d9->CreateTexture(
      Width,
      Height,
      Levels,
      Usage,
      D3DFORMAT(Format),
      D3DPOOL(Pool),
      &pTex9,
      NULL);

    if (res != D3D_OK)
      return res;

    *ppTexture = static_cast<D3D9Texture2D*>(pTex9)->GetD3D8Iface();
    return D3D_OK;
  }

    HRESULT STDMETHODCALLTYPE D3D8Device::CreateCubeTexture (
          UINT                    EdgeLength,
          UINT                    Levels,
          DWORD                   Usage,
          d3d8::D3DFORMAT               Format,
          d3d8::D3DPOOL                 Pool,
          d3d8::IDirect3DCubeTexture8** ppCubeTexture) {
    InitReturnPtr(ppCubeTexture);

    IDirect3DCubeTexture9* pTex9;
    HRESULT res = m_d3d9->CreateCubeTexture(
      EdgeLength,
      Levels,
      Usage,
      D3DFORMAT(Format),
      D3DPOOL(Pool),
      &pTex9,
      NULL);

    if (res != D3D_OK)
      return res;

    *ppCubeTexture = static_cast<D3D9TextureCube*>(pTex9)->GetD3D8Iface();
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::CreateVertexBuffer(
          UINT                     Length,
          DWORD                    Usage,
          DWORD                    FVF,
          d3d8::D3DPOOL                  Pool,
          d3d8::IDirect3DVertexBuffer8** ppVertexBuffer) {

    IDirect3DVertexBuffer9* pVertexBuffer9;
    HRESULT res = m_d3d9->CreateVertexBuffer(Length, Usage, FVF, D3DPOOL(Pool), &pVertexBuffer9, NULL);
    if (res != D3D_OK)
      return res;

    *ppVertexBuffer = static_cast<D3D9VertexBuffer*>(pVertexBuffer9)->GetD3D8Iface();
    return res;
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::CreateIndexBuffer(
          UINT                    Length,
          DWORD                   Usage,
          d3d8::D3DFORMAT               Format,
          d3d8::D3DPOOL                 Pool,
          d3d8::IDirect3DIndexBuffer8** ppIndexBuffer) {
    IDirect3DIndexBuffer9* pIndexBuffer9;
    HRESULT res = m_d3d9->CreateIndexBuffer(Length, Usage, D3DFORMAT(Format), D3DPOOL(Pool), &pIndexBuffer9, NULL);
    if (res != D3D_OK)
      return res;

    *ppIndexBuffer = static_cast<D3D9IndexBuffer*>(pIndexBuffer9)->GetD3D8Iface();
    return res;
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::CreateRenderTarget(
          UINT                Width,
          UINT                Height,
          d3d8::D3DFORMAT           Format,
          d3d8::D3DMULTISAMPLE_TYPE MultiSample,
          BOOL                Lockable,
          d3d8::IDirect3DSurface8** ppSurface) {
    InitReturnPtr(ppSurface);

    IDirect3DSurface9* pD3D9Surf;
    HRESULT res = m_d3d9->CreateRenderTarget(
      Width,
      Height,
      D3DFORMAT(Format),
      D3DMULTISAMPLE_TYPE(MultiSample),
      0,    // TODO: CreateRenderTarget MultisampleQuality
      Lockable,
      &pD3D9Surf,
      NULL);

    if (res != D3D_OK)
      return res;

    *ppSurface = static_cast<D3D9Surface*>(pD3D9Surf)->GetD3D8Iface();
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::CreateDepthStencilSurface(
          UINT                Width,
          UINT                Height,
          d3d8::D3DFORMAT           Format,
          d3d8::D3DMULTISAMPLE_TYPE MultiSample,
          d3d8::IDirect3DSurface8** ppSurface) {
    InitReturnPtr(ppSurface);

    IDirect3DSurface9* pD3D9Surf;
    HRESULT res = m_d3d9->CreateDepthStencilSurface(
      Width,
      Height,
      D3DFORMAT(Format),
      D3DMULTISAMPLE_TYPE(MultiSample),
      0,    // TODO: CreateDepthStencilSurface MultisampleQuality
      true, // TODO: CreateDepthStencilSurface Discard
      &pD3D9Surf,
      NULL);

    if (res != D3D_OK)
      return res;

    *ppSurface = static_cast<D3D9Surface*>(pD3D9Surf)->GetD3D8Iface();
    return D3D_OK;
  }

    // CreateImageSurface -> CreateOffscreenPlainSurface
  HRESULT STDMETHODCALLTYPE D3D8Device::CreateImageSurface(
          UINT Width,
          UINT Height,
          d3d8::D3DFORMAT Format,
          d3d8::IDirect3DSurface8** ppSurface) {
    InitReturnPtr(ppSurface);

    IDirect3DSurface9* pD3D9Surf;
    HRESULT res = m_d3d9->CreateOffscreenPlainSurface(
      Width,
      Height,
      D3DFORMAT(Format),
      // FIXME: D3DPOOL_SCRATCH is said to be dx8 compatible, but currently won't work with CopyRects
      D3DPOOL_SYSTEMMEM,
      &pD3D9Surf,
      NULL);

    if (res != D3D_OK)
      return res;

    *ppSurface = static_cast<D3D9Surface*>(pD3D9Surf)->GetD3D8Iface();
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::SetRenderTarget(
          d3d8::IDirect3DSurface8* pRenderTarget,
          d3d8::IDirect3DSurface8* pNewZStencil) {
    HRESULT res;

    if (pRenderTarget != NULL) {
      D3D8Surface* surf = static_cast<D3D8Surface*>(pRenderTarget);
      res = m_d3d9->SetRenderTarget(0, surf->GetD3D9Iface()); // use RT index 0

      if (res != D3D_OK)
        return res;
    }

    // SetDepthStencilSurface is a separate call
    D3D8Surface* zStencil = static_cast<D3D8Surface*>(pNewZStencil);
    res = m_d3d9->SetDepthStencilSurface(zStencil ? zStencil->GetD3D9Iface() : nullptr);

    return res;
  }


  HRESULT STDMETHODCALLTYPE D3D8Device::GetRenderTarget(d3d8::IDirect3DSurface8** ppRenderTarget) {
    InitReturnPtr(ppRenderTarget);

    IDirect3DSurface9* pD3D9Surf;
    HRESULT res = m_d3d9->GetRenderTarget(0, &pD3D9Surf);
    if (res != D3D_OK)
      return res;

    *ppRenderTarget = static_cast<D3D9Surface*>(pD3D9Surf)->GetD3D8Iface();
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetDepthStencilSurface(d3d8::IDirect3DSurface8** ppZStencilSurface) {
    InitReturnPtr(ppZStencilSurface);

    IDirect3DSurface9* pD3D9Surf;
    HRESULT res = m_d3d9->GetDepthStencilSurface(&pD3D9Surf);
    if (res != D3D_OK)
      return res;

    *ppZStencilSurface = static_cast<D3D9Surface*>(pD3D9Surf)->GetD3D8Iface();
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::BeginScene() { return m_d3d9->BeginScene(); }

  HRESULT STDMETHODCALLTYPE D3D8Device::EndScene() { return m_d3d9->EndScene(); }

  HRESULT STDMETHODCALLTYPE D3D8Device::Clear(
          DWORD    Count,
    const d3d8::D3DRECT* pRects,
          DWORD    Flags,
          d3d8::D3DCOLOR Color,
          float    Z,
          DWORD    Stencil) {
    return m_d3d9->Clear(Count, reinterpret_cast<const D3DRECT*>(pRects), Flags, D3DCOLOR(Color), Z, Stencil);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::SetTransform(d3d8::D3DTRANSFORMSTATETYPE State, const d3d8::D3DMATRIX* pMatrix) {
    return m_d3d9->SetTransform(D3DTRANSFORMSTATETYPE(State), reinterpret_cast<const D3DMATRIX*>(pMatrix));
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetTransform(d3d8::D3DTRANSFORMSTATETYPE State, d3d8::D3DMATRIX* pMatrix) {
    return m_d3d9->GetTransform(D3DTRANSFORMSTATETYPE(State), reinterpret_cast<D3DMATRIX*>(pMatrix));
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::MultiplyTransform(d3d8::D3DTRANSFORMSTATETYPE TransformState, const d3d8::D3DMATRIX* pMatrix) {
    return m_d3d9->MultiplyTransform(D3DTRANSFORMSTATETYPE(TransformState), reinterpret_cast<const D3DMATRIX*>(pMatrix));
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::SetViewport(const d3d8::D3DVIEWPORT8* pViewport) {
    return m_d3d9->SetViewport(reinterpret_cast<const D3DVIEWPORT9*>(pViewport));
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetViewport(d3d8::D3DVIEWPORT8* pViewport) {
    return m_d3d9->GetViewport(reinterpret_cast<D3DVIEWPORT9*>(pViewport));
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::SetMaterial(const d3d8::D3DMATERIAL8* pMaterial) {
    return m_d3d9->SetMaterial(reinterpret_cast<const D3DMATERIAL9*>(pMaterial));
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetMaterial(d3d8::D3DMATERIAL8* pMaterial) {
    return m_d3d9->GetMaterial(reinterpret_cast<D3DMATERIAL9*>(pMaterial));
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::SetLight(DWORD Index, const d3d8::D3DLIGHT8* pLight) {
    return m_d3d9->SetLight(Index, reinterpret_cast<const D3DLIGHT9*>(pLight));
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetLight(DWORD Index, d3d8::D3DLIGHT8* pLight) {
    return m_d3d9->GetLight(Index, reinterpret_cast<D3DLIGHT9*>(pLight));
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::LightEnable(DWORD Index, BOOL Enable) {
    return m_d3d9->LightEnable(Index, Enable);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetLightEnable(DWORD Index, BOOL* pEnable) {
    return m_d3d9->GetLightEnable(Index, pEnable);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::SetClipPlane(DWORD Index, const float* pPlane) {
    return m_d3d9->SetClipPlane(Index, pPlane);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetClipPlane(DWORD Index, float* pPlane) {
    return m_d3d9->GetClipPlane(Index, pPlane);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::SetRenderState(d3d8::D3DRENDERSTATETYPE State, DWORD Value) {
    return m_d3d9->SetRenderState((D3DRENDERSTATETYPE)State, Value);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetRenderState(d3d8::D3DRENDERSTATETYPE State, DWORD* pValue) {
    return m_d3d9->GetRenderState((D3DRENDERSTATETYPE)State, pValue);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::CreateStateBlock(
          d3d8::D3DSTATEBLOCKTYPE     Type,
          DWORD*                pToken) {

    /*Com<IDirect3DStateBlock9> pStateBlock9;
    HRESULT res = m_d3d9->CreateStateBlock(D3DSTATEBLOCKTYPE(Type), &pStateBlock9);

    D3D8StateBlock* pStateBlock = new D3D8StateBlock(this, pStateBlock9.ref());

    *pToken = DWORD(reinterpret_cast<uintptr_t>(pStateBlock));*/

    return S_FALSE;
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::CaptureStateBlock(DWORD Token) {
    //return reinterpret_cast<D3D8StateBlock*>(Token)->Capture();
    return S_FALSE;
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::ApplyStateBlock(DWORD Token) {
    //return reinterpret_cast<D3D8StateBlock*>(Token)->Apply();
    return S_FALSE;
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::DeleteStateBlock(DWORD Token) {
    //delete reinterpret_cast<D3D8StateBlock*>(Token);
    //return D3D_OK;
    return S_FALSE;
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::BeginStateBlock() {

    /*if (unlikely(m_recorder != nullptr))
      return D3DERR_INVALIDCALL;

    m_recorder = new D3D8StateBlock(this);

    return m_d3d9->BeginStateBlock();*/
    return S_FALSE;
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::EndStateBlock(DWORD* pToken) {

    /*if (unlikely(pToken == nullptr || m_recorder == nullptr))
      return D3DERR_INVALIDCALL;

    Com<IDirect3DStateBlock9> pStateBlock;
    HRESULT res = m_d3d9->EndStateBlock(&pStateBlock);

    m_recorder->SetD3D9(std::move(pStateBlock));

    *pToken = DWORD(reinterpret_cast<uintptr_t>(m_recorder));

    m_recorder = nullptr;

    return res;*/
    return S_FALSE;
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::SetClipStatus(const d3d8::D3DCLIPSTATUS8* pClipStatus) {
    return m_d3d9->SetClipStatus(reinterpret_cast<const D3DCLIPSTATUS9*>(pClipStatus));
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetClipStatus(d3d8::D3DCLIPSTATUS8* pClipStatus) {
    return m_d3d9->GetClipStatus(reinterpret_cast<D3DCLIPSTATUS9*>(pClipStatus));
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetTexture(DWORD Stage, d3d8::IDirect3DBaseTexture8** ppTexture) {
    InitReturnPtr(ppTexture);

    *ppTexture = m_textures[Stage];

    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::SetTexture(DWORD Stage, d3d8::IDirect3DBaseTexture8* pTexture) {
    return m_d3d9->SetTexture(Stage, pTexture ? static_cast<D3D8Texture2D*>(pTexture)->GetD3D9Iface() : nullptr);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetTextureStageState(
          DWORD                    Stage,
          d3d8::D3DTEXTURESTAGESTATETYPE Type,
          DWORD*                   pValue) {
    D3DSAMPLERSTATETYPE stateType = GetSamplerStateType9(Type);

    if (stateType != -1) {
      // if the type has been remapped to a sampler state type:
      return m_d3d9->GetSamplerState(Stage, stateType, pValue);
    }
    else {
      return m_d3d9->GetTextureStageState(Stage, D3DTEXTURESTAGESTATETYPE(Type), pValue);
    }
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::SetTextureStageState(
          DWORD                    Stage,
          d3d8::D3DTEXTURESTAGESTATETYPE Type,
          DWORD                    Value) {

    D3DSAMPLERSTATETYPE stateType = GetSamplerStateType9(Type);

    if (stateType != -1) {
      // if the type has been remapped to a sampler state type:
      return m_d3d9->SetSamplerState(Stage, stateType, Value);
    } else {
      return m_d3d9->SetTextureStageState(Stage, D3DTEXTURESTAGESTATETYPE(Type), Value);
    }
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::ValidateDevice(DWORD* pNumPasses) {
    return m_d3d9->ValidateDevice(pNumPasses);
  }

  // Palettes not supported by d9vk, but we pass the values through anyway.

  HRESULT STDMETHODCALLTYPE D3D8Device::SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY* pEntries) {
    return m_d3d9->SetPaletteEntries(PaletteNumber, pEntries);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) {
    return m_d3d9->GetPaletteEntries(PaletteNumber, pEntries);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::SetCurrentTexturePalette(UINT PaletteNumber) {
    return m_d3d9->SetCurrentTexturePalette(PaletteNumber);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetCurrentTexturePalette(UINT* PaletteNumber) {
    return m_d3d9->GetCurrentTexturePalette(PaletteNumber);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::DrawPrimitive(
          d3d8::D3DPRIMITIVETYPE PrimitiveType,
          UINT             StartVertex,
          UINT             PrimitiveCount) {
    return m_d3d9->DrawPrimitive(D3DPRIMITIVETYPE(PrimitiveType), StartVertex, PrimitiveCount);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::DrawIndexedPrimitive(
          d3d8::D3DPRIMITIVETYPE PrimitiveType,
          UINT             MinVertexIndex,
          UINT             NumVertices,
          UINT             StartIndex,
          UINT             PrimitiveCount) {
    return m_d3d9->DrawIndexedPrimitive(
      D3DPRIMITIVETYPE(PrimitiveType),
      m_baseVertexIndex, // set by SetIndices
      MinVertexIndex,
      NumVertices,
      StartIndex,
      PrimitiveCount);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::DrawPrimitiveUP(
          d3d8::D3DPRIMITIVETYPE PrimitiveType,
          UINT             PrimitiveCount,
    const void*            pVertexStreamZeroData,
          UINT             VertexStreamZeroStride) {
    return m_d3d9->DrawPrimitiveUP(D3DPRIMITIVETYPE(PrimitiveType), PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::DrawIndexedPrimitiveUP(
          d3d8::D3DPRIMITIVETYPE PrimitiveType,
          UINT             MinVertexIndex,
          UINT             NumVertices,
          UINT             PrimitiveCount,
    const void*            pIndexData,
          d3d8::D3DFORMAT        IndexDataFormat,
    const void*            pVertexStreamZeroData,
          UINT             VertexStreamZeroStride) {
    return m_d3d9->DrawIndexedPrimitiveUP(
      D3DPRIMITIVETYPE(PrimitiveType),
      MinVertexIndex,
      NumVertices,
      PrimitiveCount,
      pIndexData,
      D3DFORMAT(IndexDataFormat),
      pVertexStreamZeroData,
      VertexStreamZeroStride);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::SetVertexShaderConstant(
          DWORD StartRegister,
    const void* pConstantData,
          DWORD ConstantCount) {
    // ConstantCount is actually the same as Vector4fCount
    return m_d3d9->SetVertexShaderConstantF(StartRegister, reinterpret_cast<const float*>(pConstantData), ConstantCount);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::SetStreamSource(
          UINT                    StreamNumber,
          d3d8::IDirect3DVertexBuffer8* pStreamData,
          UINT                    Stride) {
    D3D8VertexBuffer* buffer = static_cast<D3D8VertexBuffer*>(pStreamData);

    return m_d3d9->SetStreamSource(StreamNumber, buffer->GetD3D9Iface(), 0, Stride);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::SetIndices(d3d8::IDirect3DIndexBuffer8* pIndexData, UINT BaseVertexIndex) {
    /*if (unlikely(m_d3d9->ShouldRecord()))
      return m_recorder->SetIndices(pIndexData, BaseVertexIndex);*/

    // used by DrawIndexedPrimitive
    m_baseVertexIndex = static_cast<INT>(BaseVertexIndex);

    D3D8IndexBuffer* buffer = static_cast<D3D8IndexBuffer*>(pIndexData);

    m_indices = buffer;

    return m_d3d9->SetIndices(buffer->GetD3D9Iface());
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetIndices(
          d3d8::IDirect3DIndexBuffer8** ppIndexData,
          UINT* pBaseVertexIndex) {
    *ppIndexData      = m_indices.ptr();
    *pBaseVertexIndex = m_baseVertexIndex;
    return D3D_OK;
  }

    HRESULT STDMETHODCALLTYPE D3D8Device::SetPixelShaderConstant(
          DWORD StartRegister,
    const void* pConstantData,
          DWORD ConstantCount) {
    // ConstantCount is actually the same as Vector4fCount
    return m_d3d9->SetPixelShaderConstantF(StartRegister, reinterpret_cast<const float*>(pConstantData), ConstantCount);
  }

  // Patches not supported by d9vk but pass the values through anyway.

  HRESULT STDMETHODCALLTYPE D3D8Device::DrawRectPatch(
          UINT               Handle,
    const float*             pNumSegs,
    const d3d8::D3DRECTPATCH_INFO* pRectPatchInfo) {
    return m_d3d9->DrawRectPatch(Handle, pNumSegs, reinterpret_cast<const D3DRECTPATCH_INFO*>(pRectPatchInfo));
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::DrawTriPatch(
          UINT              Handle,
    const float*            pNumSegs,
    const d3d8::D3DTRIPATCH_INFO* pTriPatchInfo) {
    return m_d3d9->DrawTriPatch(Handle, pNumSegs, reinterpret_cast<const D3DTRIPATCH_INFO*>(pTriPatchInfo));
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::DeletePatch(UINT Handle) {
    return m_d3d9->DeletePatch(Handle);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::CopyRects(
          d3d8::IDirect3DSurface8* pSourceSurface,
          CONST RECT* pSourceRectsArray,
          UINT cRects,
          d3d8::IDirect3DSurface8* pDestinationSurface,
          CONST POINT* pDestPointsArray) {

      if (pSourceSurface == NULL || pDestinationSurface == NULL) {
        return D3DERR_INVALIDCALL;
      }

      Com<D3D8Surface> src = static_cast<D3D8Surface*>(pSourceSurface);
      Com<D3D8Surface> dst = static_cast<D3D8Surface*>(pDestinationSurface);

      D3DSURFACE_DESC srcDesc, dstDesc;
      src->GetD3D9Iface()->GetDesc(&srcDesc);
      dst->GetD3D9Iface()->GetDesc(&dstDesc);

      // If pSourceRectsArray is NULL, then the entire surface is copied
      if (pSourceRectsArray == NULL) {
        cRects = 1;
        RECT rect;
        rect.top    = rect.left = 0;
        rect.right  = srcDesc.Width;
        rect.bottom = srcDesc.Height;
        pSourceRectsArray = &rect;

        POINT point = { 0, 0 };
        pDestPointsArray = &point;
      }

      HRESULT res = D3DERR_INVALIDCALL;

      for (UINT i = 0; i < cRects; i++) {

        RECT srcRect, dstRect;

        srcRect = pSourceRectsArray[i];

        if (pDestPointsArray != NULL) {
          dstRect.left    = pDestPointsArray[i].x;
          dstRect.right   = dstRect.left + (srcRect.right - srcRect.left);
          dstRect.top     = pDestPointsArray[i].y;
          dstRect.bottom  = dstRect.top + (srcRect.bottom - srcRect.top);
        } else {
          dstRect = srcRect;
        }

        if (srcDesc.Pool == D3DPOOL_MANAGED || dstDesc.Pool != D3DPOOL_DEFAULT) {
          // copying from managed or to non-default dest

          if ( false /*m_renderTarget == src && dstDesc.Pool == D3DPOOL_SYSTEMMEM */) {

            // rt -> system mem: use GetRenderTargetData
            res = m_d3d9->GetRenderTargetData(src->GetD3D9Iface(), dst->GetD3D9Iface());

          } else {

            // TODO: CopyRect all other cases

          }

        } else if (srcDesc.Pool == D3DPOOL_DEFAULT) {

          // default -> default: use StretchRect
          res = m_d3d9->StretchRect(
            src->GetD3D9Iface(),
            &srcRect,
            dst->GetD3D9Iface(),
            &dstRect,
            D3DTEXF_NONE
          );

        } else if (srcDesc.Pool == D3DPOOL_SYSTEMMEM) {

          // system mem -> default: use UpdateSurface
          POINT dstPt = { dstRect.left, dstRect.top };

          res = m_d3d9->UpdateSurface(
            src->GetD3D9Iface(),
            &srcRect,
            dst->GetD3D9Iface(),
            &dstPt
          );

        }

        if (FAILED(res)) break;

      }

      return res;
    }


  HRESULT STDMETHODCALLTYPE D3D8Device::GetDirect3D(d3d8::IDirect3D8** ppD3D8) {
    IDirect3D9* d3d9Iface;
    HRESULT res = m_d3d9->GetDirect3D(&d3d9Iface);
    if (res != D3D_OK)
      return res;

    //*ppD3D8 = static_cast<D3D9InterfaceEx*>(static_cast<IDirect3D9Ex*>(d3d9Iface))->GetD3D8Iface();

    return D3D_OK;
  }

  // Vertex Shaders //

#define VSD_SHIFT_MASK(token, field) ( (token & field ## MASK) >> field ## SHIFT )

  HRESULT STDMETHODCALLTYPE D3D8Device::CreateVertexShader(
        const DWORD* pDeclaration,
        const DWORD* pFunction,
              DWORD* pHandle,
              DWORD Usage ) {

    D3D8VertexShaderInfo& info = m_vertexShaders.emplace_back();

    std::vector<DWORD> tokens;

    // shaderInputRegisters:
    // set bit N to enable input register vN
    DWORD shaderInputRegisters = 0;
    
    D3DVERTEXELEMENT9 vertexElements[MAXD3DDECLLENGTH + 1];
    unsigned int elementIdx = 0;

    // these are used for pDeclaration and pFunction
    int i = 0;
    DWORD token;

    std::stringstream dbg;
    dbg << "Vertex Declaration Tokens:\n\t";

    WORD currentStream = 0;
    WORD currentOffset = 0;

    // remap d3d8 tokens to d3d9 vertex elements
    // and enable specific shaderInputRegisters for each
    do {
      token = pDeclaration[i++];

      d3d8::D3DVSD_TOKENTYPE tokenType = d3d8::D3DVSD_TOKENTYPE(VSD_SHIFT_MASK(token, D3DVSD_TOKENTYPE));

      switch ( tokenType ) {
        case d3d8::D3DVSD_TOKEN_NOP:
          dbg << "NOP";
          break;
        case d3d8::D3DVSD_TOKEN_STREAM: {
          dbg << "STREAM ";

          // TODO: D3DVSD_STREAMTESSS

          DWORD streamNum = VSD_SHIFT_MASK(token, D3DVSD_STREAMNUMBER);

          currentStream = streamNum;
          currentOffset = 0; // reset offset

          dbg << streamNum;
          break;
        }
        case d3d8::D3DVSD_TOKEN_STREAMDATA: {

          dbg << "STREAMDATA ";

          DWORD dataLoadType = VSD_SHIFT_MASK(token, D3DVSD_DATALOADTYPE);

          if ( dataLoadType == 0 ) { // vertex

            vertexElements[elementIdx].Stream = currentStream;
            vertexElements[elementIdx].Offset = currentOffset;

            // Read and set data type
            D3DDECLTYPE dataType  = D3DDECLTYPE(VSD_SHIFT_MASK(token, D3DVSD_DATATYPE));
            vertexElements[elementIdx].Type = dataType; // (D3DVSDT values map directly to D3DDECLTYPE)

            // Increase stream offset
            currentOffset += D3D9_DECL_TYPE_SIZES[dataType];

            vertexElements[elementIdx].Method = D3DDECLMETHOD_DEFAULT;

            DWORD dataReg = VSD_SHIFT_MASK(token, D3DVSD_VERTEXREG);

            // Map D3DVSDE register num to Usage and UsageIndex
            vertexElements[elementIdx].Usage = D3D8_VERTEX_INPUT_REGISTERS[dataReg][0];
            vertexElements[elementIdx].UsageIndex = D3D8_VERTEX_INPUT_REGISTERS[dataReg][1];

            // Enable register vn
            shaderInputRegisters |= 1 << dataReg;

            // Finished with this element
            elementIdx++;
            
            dbg << "type=" << dataType << ", register=" << dataReg;
          }
          break;
        }
        case d3d8::D3DVSD_TOKEN_TESSELLATOR:
          dbg << "TESSELLATOR";
          // TODO: D3DVSD_TOKEN_TESSELLATOR
          break;
        case d3d8::D3DVSD_TOKEN_CONSTMEM:
          dbg << "CONSTMEM";
          // TODO: D3DVSD_TOKEN_CONSTMEM
          break;
        case d3d8::D3DVSD_TOKEN_EXT:
          dbg << "EXT";
          // TODO: D3DVSD_TOKEN_EXT
          break;
        case d3d8::D3DVSD_TOKEN_END: {
          vertexElements[elementIdx] = D3DDECL_END();

          // Finished with this element
          elementIdx++;

          dbg << "END";
          break;
        }
        default:
          dbg << "UNKNOWN TYPE";
          break;
      }
      dbg << "\n\t";

      //dbg << std::hex << token << " ";

    } while (token != D3DVSD_END());
    Logger::debug(dbg.str());

    // copy first token
    // TODO: ensure first token is always only one dword
    tokens.push_back(pFunction[0]);

    // insert dcl instructions 

    for ( int vn = 0; vn < D3D8_NUM_VERTEX_INPUT_REGISTERS; vn++ ) {

      // if bit N is set then we need to dcl register vN
      if ( ( shaderInputRegisters & ( 1 << vn ) ) != 0 ) {

        Logger::debug(str::format("\tShader Input Regsiter: v", vn));

        DWORD usage = D3D8_VERTEX_INPUT_REGISTERS[vn][0];
        DWORD index = D3D8_VERTEX_INPUT_REGISTERS[vn][1];

        DWORD dclUsage  = (usage << D3DSP_DCL_USAGE_SHIFT) & D3DSP_DCL_USAGE_MASK;           // usage
        dclUsage       |= (index << D3DSP_DCL_USAGEINDEX_SHIFT) & D3DSP_DCL_USAGEINDEX_MASK; // usage index

        tokens.push_back(D3DSIO_DCL);   // dcl opcode
        tokens.push_back(0x80000000 | dclUsage); // usage token
        tokens.push_back(0x900F0000 | vn);    // register num
      }
    }

    // copy shader tokens from input

    i = 1; // skip first token (we already copied it)
    do {
      token = pFunction[i++];
      tokens.push_back(token);
      //Logger::debug(str::format(std::hex, token));
    } while ( token != D3DVS_END() );

    m_d3d9->CreateVertexDeclaration(vertexElements, &(info.pVertexDecl));
    HRESULT res = m_d3d9->CreateVertexShader(tokens.data(), &(info.pVertexShader));

    // Set bit to indicate this is not a fixed function FVF
    *pHandle = DWORD(m_vertexShaders.size() - 1) | DXVK_D3D8_SHADER_BIT;

    return res;
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::SetVertexShader( DWORD Handle ) {

    if (unlikely(m_d3d9->ShouldRecord())) {
      //return m_recorder->SetVertexShader(Handle);
    }

    // Check for extra bit that indicates this is not an FVF
    if ( (Handle & DXVK_D3D8_SHADER_BIT ) != 0 ) {
      // Remove that bit
      Handle &= ~DXVK_D3D8_SHADER_BIT;

      if ( unlikely( Handle >= m_vertexShaders.size() ) ) {
        Logger::err(str::format("SetVertexShader: Invalid vertex shader index ", Handle));
        return D3DERR_INVALIDCALL;
      }

      D3D8VertexShaderInfo& info = m_vertexShaders[Handle];

      if ( info.pVertexDecl == nullptr && info.pVertexShader == nullptr ) {
        Logger::err(str::format("SetVertexShader: Application provided deleted vertex shader ", Handle));
        return D3DERR_INVALIDCALL;
      }

      // Cache current shader
      m_currentVertexShader = Handle | DXVK_D3D8_SHADER_BIT;
      
      m_d3d9->SetVertexDeclaration(info.pVertexDecl);
      return m_d3d9->SetVertexShader(info.pVertexShader);

    } else {

      // Cache current FVF
      m_currentVertexShader = Handle;

      //m_d3d9->SetVertexDeclaration(nullptr);
      m_d3d9->SetVertexShader(nullptr);
      return m_d3d9->SetFVF( Handle );
    }
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetVertexShader(DWORD* pHandle) {

    // Return cached shader
    *pHandle = m_currentVertexShader;

    return D3D_OK;

    /*
    // Slow path. Use to debug cached shader validation. //
    
    IDirect3DVertexShader9* pVertexShader;
    HRESULT res = m_d3d9->GetVertexShader(&pVertexShader);

    if (FAILED(res) || pVertexShader == nullptr) {
      return m_d3d9->GetFVF(pHandle);
    }

    for (unsigned int i = 0; i < m_vertexShaders.size(); i++) {
      D3D8VertexShaderInfo& info = m_vertexShaders[i];

      if (info.pVertexShader == pVertexShader) {
        *pHandle = DWORD(i) | DXVK_D3D8_SHADER_BIT;
        return res;
      }
    }

    return res;
    */
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::DeleteVertexShader(DWORD Handle) {

    if ((Handle & DXVK_D3D8_SHADER_BIT) != 0) {

      Handle &= ~DXVK_D3D8_SHADER_BIT;

      if ( Handle >= m_vertexShaders.size() ) {
        Logger::err(str::format("DeleteVertexShader: Invalid vertex shader index ", Handle));
        return D3DERR_INVALIDCALL;
      }

      D3D8VertexShaderInfo& info = m_vertexShaders[Handle];

      if (info.pVertexDecl == nullptr && info.pVertexShader == nullptr) {
        Logger::err(str::format("DeleteVertexShader: Application provided already deleted vertex shader ", Handle));
        return D3DERR_INVALIDCALL;
      }

      if (info.pVertexDecl) {
        info.pVertexDecl->Release();
        info.pVertexDecl = nullptr;
      }

      if (info.pVertexShader) {
        info.pVertexShader->Release();
        info.pVertexShader = nullptr;
      }
    }

    return D3D_OK;
  }

  // Pixel Shaders //

  HRESULT STDMETHODCALLTYPE D3D8Device::CreatePixelShader(
        const DWORD* pFunction,
              DWORD* pHandle) {

    IDirect3DPixelShader9* pPixelShader;
    
    HRESULT res = m_d3d9->CreatePixelShader(pFunction, &pPixelShader);

    m_pixelShaders.push_back(pPixelShader);

    // Still set the shader bit so that SetVertexShader can recognize and reject a pixel shader,
    // and to prevent conflicts with NULL.
    *pHandle = DWORD(m_pixelShaders.size() - 1) | DXVK_D3D8_SHADER_BIT;

    return res;
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::SetPixelShader(DWORD Handle) {

    if (unlikely(m_d3d9->ShouldRecord())) {
      // TODO
      // return m_recorder->SetPixelShader(Handle);
    }

    if (Handle == 0) {
      return m_d3d9->SetPixelShader(nullptr);
    }

    if ( (Handle & DXVK_D3D8_SHADER_BIT) != 0 ) {
      Handle &= ~DXVK_D3D8_SHADER_BIT; // We don't care
    }

    if (unlikely(Handle >= m_pixelShaders.size())) {
      Logger::err(str::format("SetPixelShader: Invalid pixel shader index ", Handle));
      return D3DERR_INVALIDCALL;
    }

    IDirect3DPixelShader9* pPixelShader = m_pixelShaders[Handle];

    if (unlikely(pPixelShader == nullptr)) {
      Logger::err(str::format("SetPixelShader: Application provided deleted pixel shader ", Handle));
      return D3DERR_INVALIDCALL;
    }

    // Cache current pixel shader
    m_currentPixelShader = Handle | DXVK_D3D8_SHADER_BIT;

    return m_d3d9->SetPixelShader(pPixelShader);
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::GetPixelShader(DWORD* pHandle) {
    // Return cached shader
    *pHandle = m_currentPixelShader;

    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE D3D8Device::DeletePixelShader(DWORD Handle) {

    if ( (Handle & DXVK_D3D8_SHADER_BIT) != 0 ) {
      Handle &= ~DXVK_D3D8_SHADER_BIT; // We don't care
    }

    if (Handle >= m_pixelShaders.size()) {
      Logger::err(str::format("DeletePixelShader: Invalid pixel shader index ", Handle));
      return D3DERR_INVALIDCALL;
    }

    IDirect3DPixelShader9* pPixelShader = m_pixelShaders[Handle];

    if (unlikely(pPixelShader == nullptr)) {
      Logger::err(str::format("SetPixelShader: Application provided already deleted pixel shader ", Handle));
      return D3DERR_INVALIDCALL;
    }

    pPixelShader->Release();
    m_pixelShaders[Handle] = nullptr;

    return D3D_OK;
  }

} // namespace dxvk