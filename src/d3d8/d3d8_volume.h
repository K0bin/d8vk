#pragma once

#include "d3d8_include.h"
#include "d3d8_d3d9_util.h"

namespace dxvk {

class D3D9Volume;

class D3D8Volume final : public d3d8::IDirect3DVolume8 {

  public:

    D3D8Volume(D3D9Volume* pParent)
      : m_d3d9(pParent) { }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

    d3d8::D3DRESOURCETYPE STDMETHODCALLTYPE GetType();

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

    HRESULT STDMETHODCALLTYPE GetDesc(d3d8::D3DVOLUME_DESC* pDesc);

    HRESULT STDMETHODCALLTYPE LockBox(
            d3d8::D3DLOCKED_BOX* pLockedVolume,
            const d3d8::D3DBOX* pBox,
            DWORD Flags);

    HRESULT STDMETHODCALLTYPE UnlockBox();

    HRESULT STDMETHODCALLTYPE GetContainer(REFIID riid, void** ppContainer);

    D3D9Volume* GetD3D9Iface() {
      return m_d3d9;
    }

  private:

    D3D9Volume* m_d3d9;

  };

}