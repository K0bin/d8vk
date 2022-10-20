#pragma once

#include "d3d8_include.h"

namespace dxvk {
  class D3D9Surface;

  class D3D8Surface final : public d3d8::IDirect3DSurface8 {

  public:

    D3D8Surface(D3D9Surface* pParent)
      : m_d3d9(pParent) { }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();

    ULONG STDMETHODCALLTYPE Release();

    HRESULT STDMETHODCALLTYPE GetPrivateData(
            REFGUID     refguid,
            void*       pData,
            DWORD*      pSizeOfData);

    HRESULT STDMETHODCALLTYPE SetPrivateData(
            REFGUID     refguid,
      const void*       pData,
            DWORD       pSizeOfData,
            DWORD       Flags);

    HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid);

    HRESULT STDMETHODCALLTYPE GetDevice(d3d8::IDirect3DDevice8** ppDevice);

    HRESULT STDMETHODCALLTYPE GetDesc(d3d8::D3DSURFACE_DESC* pDesc);

    HRESULT STDMETHODCALLTYPE GetContainer(REFIID riid, void** ppContainer);

    HRESULT STDMETHODCALLTYPE LockRect(
            d3d8::D3DLOCKED_RECT* pLockedRect,
            const RECT* pRect,
            DWORD Flags);

    HRESULT STDMETHODCALLTYPE UnlockRect();

    D3D9Surface* GetD3D9Iface() {
      return m_d3d9;
    }

  private:

    D3D9Surface* m_d3d9;

  };
}
