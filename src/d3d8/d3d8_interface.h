#pragma once

// Implements IDirect3D8

#include "d3d8_include.h"
#include "d3d8_d3d9_util.h"
#include "d3d8_options.h"
#include "../d3d9/d3d9_bridge.h"

//#include "../dxvk/dxvk_instance.h"

namespace dxvk {

  class D3D9InterfaceEx;

  /**
  * \brief D3D8 interface implementation
  *
  * Implements the d3d8::IDirect3DDevice8 interfaces
  * which provides the way to get adapters and create other objects such as \ref d3d8::IDirect3DDevice8.
  * similar to \ref DxgiFactory but for D3D8.
  */
  class D3D8Interface final : public d3d8::IDirect3D8 {

  public:
    D3D8Interface(D3D9InterfaceEx* pParent);

    // IUnknown methods //
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

    ULONG STDMETHODCALLTYPE AddRef();

    ULONG STDMETHODCALLTYPE Release();

    // IDirect3D8 methods //

    HRESULT STDMETHODCALLTYPE RegisterSoftwareDevice(void* pInitializeFunction);

    UINT STDMETHODCALLTYPE GetAdapterCount();

    HRESULT STDMETHODCALLTYPE GetAdapterIdentifier(
      UINT                    Adapter,
      DWORD                   Flags,
      d3d8::D3DADAPTER_IDENTIFIER8* pIdentifier);

    UINT STDMETHODCALLTYPE GetAdapterModeCount(UINT Adapter);

    HRESULT STDMETHODCALLTYPE EnumAdapterModes(
      UINT            Adapter,
      UINT            Mode,
      d3d8::D3DDISPLAYMODE* pMode);

    HRESULT STDMETHODCALLTYPE GetAdapterDisplayMode(UINT Adapter, d3d8::D3DDISPLAYMODE* pMode);

    HRESULT STDMETHODCALLTYPE CheckDeviceType(
        UINT        Adapter,
        d3d8::D3DDEVTYPE  DevType,
        d3d8::D3DFORMAT   AdapterFormat,
        d3d8::D3DFORMAT   BackBufferFormat,
        BOOL        bWindowed);

    HRESULT STDMETHODCALLTYPE CheckDeviceFormat(
        UINT            Adapter,
        d3d8::D3DDEVTYPE      DeviceType,
        d3d8::D3DFORMAT       AdapterFormat,
        DWORD           Usage,
        d3d8::D3DRESOURCETYPE RType,
        d3d8::D3DFORMAT       CheckFormat);

    HRESULT STDMETHODCALLTYPE CheckDeviceMultiSampleType(
        UINT                Adapter,
        d3d8::D3DDEVTYPE          DeviceType,
        d3d8::D3DFORMAT           SurfaceFormat,
        BOOL                Windowed,
        d3d8::D3DMULTISAMPLE_TYPE MultiSampleType);

    HRESULT STDMETHODCALLTYPE CheckDepthStencilMatch(
        UINT Adapter,
        d3d8::D3DDEVTYPE DeviceType,
        d3d8::D3DFORMAT AdapterFormat,
        d3d8::D3DFORMAT RenderTargetFormat,
        d3d8::D3DFORMAT DepthStencilFormat);

    HRESULT STDMETHODCALLTYPE GetDeviceCaps(
        UINT Adapter,
        d3d8::D3DDEVTYPE DeviceType,
        d3d8::D3DCAPS8* pCaps);

    HMONITOR STDMETHODCALLTYPE GetAdapterMonitor(UINT Adapter);

    HRESULT STDMETHODCALLTYPE CreateDevice(
        UINT Adapter,
        d3d8::D3DDEVTYPE DeviceType,
        HWND hFocusWindow,
        DWORD BehaviorFlags,
        d3d8::D3DPRESENT_PARAMETERS* pPresentationParameters,
        d3d8::IDirect3DDevice8** ppReturnedDeviceInterface);

  private:
    static constexpr D3DFORMAT ADAPTER_FORMATS[] = {
      D3DFMT_A1R5G5B5,
      //D3DFMT_A2R10G10B10, (not in D3D8)
      D3DFMT_A8R8G8B8,
      D3DFMT_R5G6B5,
      D3DFMT_X1R5G5B5,
      D3DFMT_X8R8G8B8
    };

    std::vector<std::vector<d3d8::D3DDISPLAYMODE>> m_adapterModes;

    D3D9InterfaceEx* m_d3d9;
  };

} // namespace dxvk