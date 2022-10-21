#include "d3d8_interface.h"

#include "d3d8_texture.h"

#include "../d3d9/d3d9_interface.h"
#include "../d3d9/d3d9_device.h"

#include <cstring>

namespace dxvk
{

  D3D8Interface::D3D8Interface(D3D9InterfaceEx* pParent)
    : m_d3d9 (pParent) {

    UINT adapterCount = m_d3d9->GetAdapterCount();

    for (UINT adapter = 0; adapter < adapterCount; adapter++) {

      m_adapterModes.emplace_back();

      // cache adapter modes and mode counts for each d3d9 format
      for (D3DFORMAT fmt : ADAPTER_FORMATS) {

        const UINT modeCount = m_d3d9->GetAdapterModeCount(adapter, fmt);

        for (UINT mode = 0; mode < modeCount; mode++) {
          D3DDISPLAYMODE displayMode;
          m_d3d9->EnumAdapterModes(adapter, fmt, mode, &displayMode);

          d3d8::D3DDISPLAYMODE d3d8DisplayMode;
          d3d8DisplayMode.Width = displayMode.Width;
          d3d8DisplayMode.Height = displayMode.Height;
          d3d8DisplayMode.RefreshRate = displayMode.RefreshRate;
          d3d8DisplayMode.Format = d3d8::D3DFORMAT(displayMode.Format);

          m_adapterModes[adapter].push_back(d3d8DisplayMode);
        }
      }
    }
  }

  HRESULT STDMETHODCALLTYPE D3D8Interface::QueryInterface(REFIID riid, void** ppvObject) {
    return m_d3d9->QueryInterface(riid, ppvObject);
  }

  ULONG STDMETHODCALLTYPE D3D8Interface::AddRef() {
    return m_d3d9->AddRef();
  }

  ULONG STDMETHODCALLTYPE D3D8Interface::Release() {
    return m_d3d9->AddRef();
  }

  HRESULT STDMETHODCALLTYPE D3D8Interface::RegisterSoftwareDevice(void* pInitializeFunction) {
    return m_d3d9->RegisterSoftwareDevice(pInitializeFunction);
  }

  UINT STDMETHODCALLTYPE D3D8Interface::GetAdapterCount() {
    return m_d3d9->GetAdapterCount();
  }

  HRESULT STDMETHODCALLTYPE D3D8Interface::GetAdapterIdentifier(
          UINT Adapter,
          DWORD Flags,
          d3d8::D3DADAPTER_IDENTIFIER8* pIdentifier) {

    // This flag now has the opposite effect.
    // Either way, WHQLevel will be 1 with Direct3D9Ex
    if (Flags & D3DENUM_NO_WHQL_LEVEL)
      Flags &= ~D3DENUM_WHQL_LEVEL;
    else
      Flags |= D3DENUM_WHQL_LEVEL;

    D3DADAPTER_IDENTIFIER9 identifier9;
    HRESULT res = m_d3d9->GetAdapterIdentifier(Adapter, Flags, &identifier9);

    strncpy(pIdentifier->Driver, identifier9.Driver, MAX_DEVICE_IDENTIFIER_STRING);
    strncpy(pIdentifier->Description, identifier9.Description, MAX_DEVICE_IDENTIFIER_STRING);

    pIdentifier->DriverVersion = identifier9.DriverVersion;
    pIdentifier->VendorId = identifier9.VendorId;
    pIdentifier->DeviceId = identifier9.DeviceId;
    pIdentifier->SubSysId = identifier9.SubSysId;
    pIdentifier->Revision = identifier9.Revision;

    // copy
    pIdentifier->DeviceIdentifier = identifier9.DeviceIdentifier;

    pIdentifier->WHQLLevel = identifier9.WHQLLevel;

    return res;
  }

  UINT STDMETHODCALLTYPE D3D8Interface::GetAdapterModeCount(UINT Adapter) {
    return Adapter < m_adapterModes.size() ? m_adapterModes[Adapter].size() : 0;
  }

  HRESULT STDMETHODCALLTYPE D3D8Interface::EnumAdapterModes(
          UINT Adapter,
          UINT Mode,
          d3d8::D3DDISPLAYMODE* pMode) {
    if (Adapter >= m_adapterModes.size() || Mode >= m_adapterModes[Adapter].size() || pMode == nullptr) {
      return D3DERR_INVALIDCALL;
    }

    *pMode = m_adapterModes[Adapter][Mode];
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE D3D8Interface::GetAdapterDisplayMode(UINT Adapter, d3d8::D3DDISPLAYMODE* pMode) {
    return m_d3d9->GetAdapterDisplayMode(Adapter, reinterpret_cast<D3DDISPLAYMODE*>(pMode));
  }

  HRESULT STDMETHODCALLTYPE D3D8Interface::CheckDeviceType(
      UINT        Adapter,
      d3d8::D3DDEVTYPE  DevType,
      d3d8::D3DFORMAT   AdapterFormat,
      d3d8::D3DFORMAT   BackBufferFormat,
      BOOL        bWindowed) {
    return m_d3d9->CheckDeviceType(
      Adapter,
      D3DDEVTYPE(DevType),
      D3DFORMAT(AdapterFormat),
      D3DFORMAT(BackBufferFormat),
      bWindowed
    );
  }

  HRESULT STDMETHODCALLTYPE D3D8Interface::CheckDeviceFormat(
      UINT            Adapter,
      d3d8::D3DDEVTYPE      DeviceType,
      d3d8::D3DFORMAT       AdapterFormat,
      DWORD           Usage,
      d3d8::D3DRESOURCETYPE RType,
      d3d8::D3DFORMAT       CheckFormat) {
    // HACK: forceD16.
    /*if (m_d3d8Options.forceD16 && (Usage & D3DUSAGE_DEPTHSTENCIL)) {
      // Some other less common depth formats are not checked here.
      if (CheckFormat == d3d8::D3DFMT_D32 || CheckFormat == d3d8::D3DFMT_D24S8)
        return D3DERR_NOTAVAILABLE;
    }*/

    return m_d3d9->CheckDeviceFormat(
      Adapter,
      D3DDEVTYPE(DeviceType),
      D3DFORMAT(AdapterFormat),
      Usage,
      D3DRESOURCETYPE(RType),
      D3DFORMAT(CheckFormat)
    );
  }

  HRESULT STDMETHODCALLTYPE D3D8Interface::CheckDeviceMultiSampleType(
      UINT                Adapter,
      d3d8::D3DDEVTYPE          DeviceType,
      d3d8::D3DFORMAT           SurfaceFormat,
      BOOL                Windowed,
      d3d8::D3DMULTISAMPLE_TYPE MultiSampleType) {

    DWORD* pQualityLevels = nullptr;
    return m_d3d9->CheckDeviceMultiSampleType(
      Adapter,
      D3DDEVTYPE(DeviceType),
      D3DFORMAT(SurfaceFormat),
      Windowed,
      D3DMULTISAMPLE_TYPE(MultiSampleType),
      pQualityLevels
    );
  }

  HRESULT STDMETHODCALLTYPE D3D8Interface::CheckDepthStencilMatch(
      UINT Adapter,
      d3d8::D3DDEVTYPE DeviceType,
      d3d8::D3DFORMAT AdapterFormat,
      d3d8::D3DFORMAT RenderTargetFormat,
      d3d8::D3DFORMAT DepthStencilFormat) {
    return m_d3d9->CheckDepthStencilMatch(
      Adapter,
      D3DDEVTYPE(DeviceType),
      D3DFORMAT(AdapterFormat),
      D3DFORMAT(RenderTargetFormat),
      D3DFORMAT(DepthStencilFormat)
    );
  }

  HRESULT STDMETHODCALLTYPE D3D8Interface::GetDeviceCaps(
      UINT Adapter,
      d3d8::D3DDEVTYPE DeviceType,
      d3d8::D3DCAPS8* pCaps) {
    D3DCAPS9 caps9;
    HRESULT res = m_d3d9->GetDeviceCaps(Adapter, D3DDEVTYPE(DeviceType), &caps9);
    dxvk::ConvertCaps8(caps9, pCaps);
    return res;
  }

  HMONITOR STDMETHODCALLTYPE D3D8Interface::GetAdapterMonitor(UINT Adapter) {
    return m_d3d9->GetAdapterMonitor(Adapter);
  }

  HRESULT STDMETHODCALLTYPE D3D8Interface::CreateDevice(
        UINT Adapter,
        d3d8::D3DDEVTYPE DeviceType,
        HWND hFocusWindow,
        DWORD BehaviorFlags,
        d3d8::D3DPRESENT_PARAMETERS* pPresentationParameters,
        d3d8::IDirect3DDevice8** ppReturnedDeviceInterface) {

    IDirect3DDevice9* pDevice9 = nullptr;
    D3DPRESENT_PARAMETERS params = ConvertPresentParameters9(pPresentationParameters);

    // HACK: forceD16
    /*if (m_d3d8Options.forceD16 && params.EnableAutoDepthStencil) {
      params.AutoDepthStencilFormat = D3DFMT_D16;
    }*/

    HRESULT res = m_d3d9->CreateDevice(
      Adapter,
      (D3DDEVTYPE)DeviceType,
      hFocusWindow,
      BehaviorFlags,
      &params,
      &pDevice9
    );

    if (FAILED(res)) {
      return res;
    }

    *ppReturnedDeviceInterface = static_cast<D3D9DeviceEx*>(pDevice9)->GetD3D8Iface();

    return res;
  }



} // namespace dxvk