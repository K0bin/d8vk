#pragma once

// Implements d3d8::IDirect3DDevice8

#include "d3d8_include.h"
#include "d3d8_texture.h"
#include "d3d8_buffer.h"
#include "d3d8_state_block.h"
#include "d3d8_d3d9_util.h"
#include "d3d8_caps.h"

#include "../d3d9/d3d9_bridge.h"

#include <array>
#include <vector>
#include <type_traits>
#include <unordered_map>

// so we dont have to write 100 stubs in development
// TODO: eliminate use of this by implementing stubs
#define D3D8_DEVICE_STUB(...) \
(__VA_ARGS__) { \
  Logger::warn("D3D8Device: STUB (" #__VA_ARGS__ ") -> HRESULT"); \
  return D3DERR_INVALIDCALL;\
}

#define D3D8_DEVICE_STUB_(Name, ...) \
(__VA_ARGS__) { \
  Logger::warn("D3D8Device::" #Name " STUB (" #__VA_ARGS__ ") -> HRESULT"); \
  return D3DERR_INVALIDCALL;\
}


#define D3D8_DEVICE_STUB_VOID(...) \
(__VA_ARGS__) { \
  Logger::warn("D3D8Device: STUB (" #__VA_ARGS__ ") -> void"); \
  return;\
}

namespace dxvk {

  class D3D8Interface;
  class D3D8SwapChainEx;
  class D3D9DeviceEx;

  struct D3D8VertexShaderInfo;

  class D3D8Device final : public d3d8::IDirect3DDevice8 {

    friend class D3D8SwapChainEx;
    friend class D3D8StateBlock;
  public:

    D3D8Device(
      D3D8Interface*              pParent,
      Com<IDirect3DDevice9>&& pDevice,
      //D3D8Adapter*                    pAdapter,
      D3DDEVTYPE                    DeviceType,
      HWND                          hFocusWindow,
      DWORD                         BehaviorFlags);

    ~D3D8Device();

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

    ULONG STDMETHODCALLTYPE AddRef();

    ULONG STDMETHODCALLTYPE Release();

    /* Direct3D 8 Exclusive Methods */

    HRESULT STDMETHODCALLTYPE CopyRects(
            d3d8::IDirect3DSurface8* pSourceSurface,
            CONST RECT* pSourceRectsArray,
            UINT cRects,
            d3d8::IDirect3DSurface8* pDestinationSurface,
            CONST POINT* pDestPointsArray);
    
    STDMETHOD(GetPixelShaderConstant) D3D8_DEVICE_STUB(THIS_ DWORD Register, void* pConstantData, DWORD ConstantCount);
    STDMETHOD(GetPixelShaderFunction) D3D8_DEVICE_STUB(THIS_ DWORD Handle, void* pData, DWORD* pSizeOfData);
    STDMETHOD(GetVertexShaderConstant) D3D8_DEVICE_STUB(THIS_ DWORD Register, void* pConstantData, DWORD ConstantCount);
    STDMETHOD(GetVertexShaderDeclaration) D3D8_DEVICE_STUB(THIS_ DWORD Handle, void* pData, DWORD* pSizeOfData);
    STDMETHOD(GetVertexShaderFunction) D3D8_DEVICE_STUB(THIS_ DWORD Handle, void* pData, DWORD* pSizeOfData);

    STDMETHOD(GetInfo) D3D8_DEVICE_STUB(THIS_ DWORD DevInfoID, void* pDevInfoStruct, DWORD DevInfoStructSize);


    HRESULT STDMETHODCALLTYPE TestCooperativeLevel();

    UINT    STDMETHODCALLTYPE GetAvailableTextureMem();

    HRESULT STDMETHODCALLTYPE ResourceManagerDiscardBytes(DWORD bytes);

    HRESULT STDMETHODCALLTYPE GetDirect3D(d3d8::IDirect3D8** ppD3D8);

    HRESULT STDMETHODCALLTYPE GetDeviceCaps(d3d8::D3DCAPS8* pCaps);

    HRESULT STDMETHODCALLTYPE GetDisplayMode(d3d8::D3DDISPLAYMODE* pMode);

    HRESULT STDMETHODCALLTYPE GetCreationParameters(d3d8::D3DDEVICE_CREATION_PARAMETERS* pParameters);

    HRESULT STDMETHODCALLTYPE SetCursorProperties(
      UINT               XHotSpot,
      UINT               YHotSpot,
      d3d8::IDirect3DSurface8* pCursorBitmap);

    void    STDMETHODCALLTYPE SetCursorPosition(UINT XScreenSpace, UINT YScreenSpace, DWORD Flags);

    BOOL    STDMETHODCALLTYPE ShowCursor(BOOL bShow);

    // TODO: CreateAdditionalSwapChain
    HRESULT STDMETHODCALLTYPE CreateAdditionalSwapChain D3D8_DEVICE_STUB(
      d3d8::D3DPRESENT_PARAMETERS* pPresentationParameters,
      d3d8::IDirect3DSwapChain8** ppSwapChain);


    HRESULT STDMETHODCALLTYPE Reset(d3d8::D3DPRESENT_PARAMETERS* pPresentationParameters);

    HRESULT STDMETHODCALLTYPE Present(
      const RECT* pSourceRect,
      const RECT* pDestRect,
            HWND hDestWindowOverride,
      const RGNDATA* pDirtyRegion);

    HRESULT STDMETHODCALLTYPE GetBackBuffer(
            UINT iBackBuffer,
            d3d8::D3DBACKBUFFER_TYPE Type,
            d3d8::IDirect3DSurface8** ppBackBuffer);

    HRESULT STDMETHODCALLTYPE GetRasterStatus(d3d8::D3DRASTER_STATUS* pRasterStatus);

    void STDMETHODCALLTYPE SetGammaRamp(DWORD Flags, const d3d8::D3DGAMMARAMP* pRamp);

    void STDMETHODCALLTYPE GetGammaRamp(d3d8::D3DGAMMARAMP* pRamp);

    HRESULT STDMETHODCALLTYPE CreateTexture (
            UINT                Width,
            UINT                Height,
            UINT                Levels,
            DWORD               Usage,
            d3d8::D3DFORMAT           Format,
            d3d8::D3DPOOL             Pool,
            d3d8::IDirect3DTexture8** ppTexture);

    HRESULT STDMETHODCALLTYPE CreateVolumeTexture D3D8_DEVICE_STUB(
            UINT                      Width,
            UINT                      Height,
            UINT                      Depth,
            UINT                      Levels,
            DWORD                     Usage,
            d3d8::D3DFORMAT                 Format,
            d3d8::D3DPOOL                   Pool,
            d3d8::IDirect3DVolumeTexture8** ppVolumeTexture);

    HRESULT STDMETHODCALLTYPE CreateCubeTexture(
          UINT                      EdgeLength,
            UINT                    Levels,
            DWORD                   Usage,
            d3d8::D3DFORMAT               Format,
            d3d8::D3DPOOL                 Pool,
            d3d8::IDirect3DCubeTexture8** ppCubeTexture);

    HRESULT STDMETHODCALLTYPE CreateVertexBuffer(
            UINT                     Length,
            DWORD                    Usage,
            DWORD                    FVF,
            d3d8::D3DPOOL                  Pool,
            d3d8::IDirect3DVertexBuffer8** ppVertexBuffer);

    HRESULT STDMETHODCALLTYPE CreateIndexBuffer(
            UINT                    Length,
            DWORD                   Usage,
            d3d8::D3DFORMAT               Format,
            d3d8::D3DPOOL                 Pool,
            d3d8::IDirect3DIndexBuffer8** ppIndexBuffer);

    HRESULT STDMETHODCALLTYPE CreateRenderTarget(
            UINT                Width,
            UINT                Height,
            d3d8::D3DFORMAT           Format,
            d3d8::D3DMULTISAMPLE_TYPE MultiSample,
            BOOL                Lockable,
            d3d8::IDirect3DSurface8** ppSurface);

    HRESULT STDMETHODCALLTYPE CreateDepthStencilSurface(
            UINT                Width,
            UINT                Height,
            d3d8::D3DFORMAT           Format,
            d3d8::D3DMULTISAMPLE_TYPE MultiSample,
            d3d8::IDirect3DSurface8** ppSurface);

    HRESULT STDMETHODCALLTYPE UpdateTexture D3D8_DEVICE_STUB_(UpdateTexture,
            d3d8::IDirect3DBaseTexture8* pSourceTexture,
            d3d8::IDirect3DBaseTexture8* pDestinationTexture);

    HRESULT STDMETHODCALLTYPE GetFrontBuffer D3D8_DEVICE_STUB(d3d8::IDirect3DSurface8* pDestSurface);

    // CreateImageSurface -> CreateOffscreenPlainSurface
    HRESULT STDMETHODCALLTYPE CreateImageSurface(UINT Width, UINT Height, d3d8::D3DFORMAT Format, d3d8::IDirect3DSurface8** ppSurface);

    HRESULT STDMETHODCALLTYPE SetRenderTarget(d3d8::IDirect3DSurface8* pRenderTarget, d3d8::IDirect3DSurface8* pNewZStencil);

    HRESULT STDMETHODCALLTYPE GetRenderTarget(d3d8::IDirect3DSurface8** ppRenderTarget);

    HRESULT STDMETHODCALLTYPE GetDepthStencilSurface(d3d8::IDirect3DSurface8** ppZStencilSurface);

    HRESULT STDMETHODCALLTYPE BeginScene();

    HRESULT STDMETHODCALLTYPE EndScene();

    HRESULT STDMETHODCALLTYPE Clear(
            DWORD    Count,
      const d3d8::D3DRECT* pRects,
            DWORD    Flags,
            d3d8::D3DCOLOR Color,
            float    Z,
            DWORD    Stencil);

    HRESULT STDMETHODCALLTYPE SetTransform(d3d8::D3DTRANSFORMSTATETYPE State, const d3d8::D3DMATRIX* pMatrix);

    HRESULT STDMETHODCALLTYPE GetTransform(d3d8::D3DTRANSFORMSTATETYPE State, d3d8::D3DMATRIX* pMatrix);

    HRESULT STDMETHODCALLTYPE MultiplyTransform(d3d8::D3DTRANSFORMSTATETYPE TransformState, const d3d8::D3DMATRIX* pMatrix);

    HRESULT STDMETHODCALLTYPE SetViewport(const d3d8::D3DVIEWPORT8* pViewport);

    HRESULT STDMETHODCALLTYPE GetViewport(d3d8::D3DVIEWPORT8* pViewport);

    HRESULT STDMETHODCALLTYPE SetMaterial(const d3d8::D3DMATERIAL8* pMaterial);

    HRESULT STDMETHODCALLTYPE GetMaterial(d3d8::D3DMATERIAL8* pMaterial);

    HRESULT STDMETHODCALLTYPE SetLight(DWORD Index, const d3d8::D3DLIGHT8* pLight);

    HRESULT STDMETHODCALLTYPE GetLight(DWORD Index, d3d8::D3DLIGHT8* pLight);

    HRESULT STDMETHODCALLTYPE LightEnable(DWORD Index, BOOL Enable);

    HRESULT STDMETHODCALLTYPE GetLightEnable(DWORD Index, BOOL* pEnable);

    HRESULT STDMETHODCALLTYPE SetClipPlane(DWORD Index, const float* pPlane);

    HRESULT STDMETHODCALLTYPE GetClipPlane(DWORD Index, float* pPlane);

    HRESULT STDMETHODCALLTYPE SetRenderState(d3d8::D3DRENDERSTATETYPE State, DWORD Value);

    HRESULT STDMETHODCALLTYPE GetRenderState(d3d8::D3DRENDERSTATETYPE State, DWORD* pValue);

    HRESULT STDMETHODCALLTYPE CreateStateBlock(
            d3d8::D3DSTATEBLOCKTYPE     Type,
            DWORD*                pToken);

    HRESULT STDMETHODCALLTYPE CaptureStateBlock(DWORD Token);

    HRESULT STDMETHODCALLTYPE ApplyStateBlock(DWORD Token);

    HRESULT STDMETHODCALLTYPE DeleteStateBlock(DWORD Token);

    HRESULT STDMETHODCALLTYPE BeginStateBlock();

    HRESULT STDMETHODCALLTYPE EndStateBlock(DWORD* pToken);

    HRESULT STDMETHODCALLTYPE SetClipStatus(const d3d8::D3DCLIPSTATUS8* pClipStatus);

    HRESULT STDMETHODCALLTYPE GetClipStatus(d3d8::D3DCLIPSTATUS8* pClipStatus);

    HRESULT STDMETHODCALLTYPE GetTexture(DWORD Stage, d3d8::IDirect3DBaseTexture8** ppTexture);

    HRESULT STDMETHODCALLTYPE SetTexture(DWORD Stage, d3d8::IDirect3DBaseTexture8* pTexture);

    HRESULT STDMETHODCALLTYPE GetTextureStageState(
            DWORD                    Stage,
            d3d8::D3DTEXTURESTAGESTATETYPE Type,
            DWORD*                   pValue);

    HRESULT STDMETHODCALLTYPE SetTextureStageState(
            DWORD                    Stage,
            d3d8::D3DTEXTURESTAGESTATETYPE Type,
            DWORD                    Value);

    HRESULT STDMETHODCALLTYPE ValidateDevice(DWORD* pNumPasses);

    HRESULT STDMETHODCALLTYPE SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY* pEntries);

    HRESULT STDMETHODCALLTYPE GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries);

    HRESULT STDMETHODCALLTYPE SetCurrentTexturePalette(UINT PaletteNumber);

    HRESULT STDMETHODCALLTYPE GetCurrentTexturePalette(UINT* PaletteNumber);

    HRESULT STDMETHODCALLTYPE DrawPrimitive(
            d3d8::D3DPRIMITIVETYPE PrimitiveType,
            UINT             StartVertex,
            UINT             PrimitiveCount);

    HRESULT STDMETHODCALLTYPE DrawIndexedPrimitive(
            d3d8::D3DPRIMITIVETYPE PrimitiveType,
            UINT             MinVertexIndex,
            UINT             NumVertices,
            UINT             StartIndex,
            UINT             PrimitiveCount);

    HRESULT STDMETHODCALLTYPE DrawPrimitiveUP(
            d3d8::D3DPRIMITIVETYPE PrimitiveType,
            UINT             PrimitiveCount,
      const void*            pVertexStreamZeroData,
            UINT             VertexStreamZeroStride);

    HRESULT STDMETHODCALLTYPE DrawIndexedPrimitiveUP(
            d3d8::D3DPRIMITIVETYPE PrimitiveType,
            UINT             MinVertexIndex,
            UINT             NumVertices,
            UINT             PrimitiveCount,
      const void*            pIndexData,
            d3d8::D3DFORMAT        IndexDataFormat,
      const void*            pVertexStreamZeroData,
            UINT             VertexStreamZeroStride);

    HRESULT STDMETHODCALLTYPE ProcessVertices D3D8_DEVICE_STUB(
            UINT                         SrcStartIndex,
            UINT                         DestIndex,
            UINT                         VertexCount,
            d3d8::IDirect3DVertexBuffer8*      pDestBuffer,
            DWORD                        Flags);


    HRESULT STDMETHODCALLTYPE CreateVertexShader(
      const DWORD*  pDeclaration,
      const DWORD*  pFunction,
            DWORD*  pHandle,
            DWORD   Usage);

    HRESULT STDMETHODCALLTYPE SetVertexShader(DWORD Handle);

    HRESULT STDMETHODCALLTYPE GetVertexShader(DWORD* pHandle);

    HRESULT STDMETHODCALLTYPE DeleteVertexShader(DWORD Handle);

    HRESULT STDMETHODCALLTYPE SetVertexShaderConstant(
            DWORD StartRegister,
      const void* pConstantData,
            DWORD ConstantCount);

    HRESULT STDMETHODCALLTYPE SetStreamSource(
            UINT                    StreamNumber,
            d3d8::IDirect3DVertexBuffer8* pStreamData,
            UINT                    Stride);

    HRESULT STDMETHODCALLTYPE GetStreamSource D3D8_DEVICE_STUB(
            UINT                     StreamNumber,
            d3d8::IDirect3DVertexBuffer8** ppStreamData,
            UINT*                    pStride);

    HRESULT STDMETHODCALLTYPE SetIndices(d3d8::IDirect3DIndexBuffer8* pIndexData, UINT BaseVertexIndex);

    HRESULT STDMETHODCALLTYPE GetIndices(
            d3d8::IDirect3DIndexBuffer8** ppIndexData,
            UINT* pBaseVertexIndex);

    HRESULT STDMETHODCALLTYPE CreatePixelShader(
      const DWORD* pFunction, 
            DWORD* pHandle);

    HRESULT STDMETHODCALLTYPE SetPixelShader(DWORD Handle);

    HRESULT STDMETHODCALLTYPE GetPixelShader(DWORD* pHandle);

    HRESULT STDMETHODCALLTYPE DeletePixelShader(DWORD Handle);

    HRESULT STDMETHODCALLTYPE SetPixelShaderConstant(
            DWORD StartRegister,
      const void* pConstantData,
            DWORD ConstantCount);
    // Patches not supported by d9vk but pass the values through anyway.

    HRESULT STDMETHODCALLTYPE DrawRectPatch(
            UINT               Handle,
      const float*             pNumSegs,
      const d3d8::D3DRECTPATCH_INFO* pRectPatchInfo);

    HRESULT STDMETHODCALLTYPE DrawTriPatch(
            UINT              Handle,
      const float*            pNumSegs,
      const d3d8::D3DTRIPATCH_INFO* pTriPatchInfo);

    HRESULT STDMETHODCALLTYPE DeletePatch(UINT Handle);

  public: // Internal Methods //

    inline bool ShouldRecord() { return m_d3d9->ShouldRecord() }

    D3D9DeviceEx* GetD3D9Iface() { return m_d3d9; }

  private:

    D3D9Bridge*           m_bridge;

    D3D9DeviceEx*         m_d3d9;

    Com<D3D8Interface>  m_parent;

    std::array<d3d8::IDirect3DBaseTexture8*, d8caps::MAX_TEXTURE_STAGES>  m_textures;

    Com<D3D8IndexBuffer>        m_indices;
    INT                         m_baseVertexIndex = 0;

    std::vector<D3D8VertexShaderInfo>           m_vertexShaders;
    std::vector<IDirect3DPixelShader9*>   m_pixelShaders;
    DWORD                       m_currentVertexShader  = 0;  // can be FVF or vs index, can have DXVK_D3D8_SHADER_BIT
    DWORD                       m_currentPixelShader   = 0;  // will have DXVK_D3D8_SHADER_BIT

    std::unordered_map<DWORD, Com<D3D9StateBlock>> m_stateBlocks;

  };

}
