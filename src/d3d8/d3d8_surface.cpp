#include "d3d8_surface.h"

#include "../d3d9/d3d9_surface.h"
#include "d3d8_d3d9_util.h"

namespace dxvk {

  HRESULT STDMETHODCALLTYPE D3D8Surface::QueryInterface(REFIID riid, void** ppvObject) {
    return m_d3d9->QueryInterface(riid, ppvObject);
  }

  ULONG STDMETHODCALLTYPE D3D8Surface::AddRef() {
    return m_d3d9->AddRef();
  }

  ULONG STDMETHODCALLTYPE D3D8Surface::Release() {
    return m_d3d9->Release();
  }

  HRESULT STDMETHODCALLTYPE D3D8Surface::GetPrivateData(
          REFGUID     refguid,
          void*       pData,
          DWORD*      pSizeOfData) {
    return m_d3d9->GetPrivateData(refguid, pData, pSizeOfData);
  }

  HRESULT STDMETHODCALLTYPE D3D8Surface::SetPrivateData(
          REFGUID     refguid,
    const void*       pData,
          DWORD       pSizeOfData,
          DWORD       Flags) {
    return m_d3d9->SetPrivateData(refguid, pData, pSizeOfData, Flags);
  }

  HRESULT STDMETHODCALLTYPE D3D8Surface::FreePrivateData(REFGUID refguid) {
    return m_d3d9->FreePrivateData(refguid);
  }

  HRESULT STDMETHODCALLTYPE D3D8Surface::GetDevice(d3d8::IDirect3DDevice8** ppDevice) {
    IDirect3DDevice9* d3d9Device;
    HRESULT res = m_d3d9->GetDevice(&d3d9Device);
    if (res != D3D_OK) {
      return res;
    }
    //*ppSurfaceLevel = static_cast<D3D9Device*>(d3d9Device)->GetD3D8Iface();
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE D3D8Surface::GetContainer(REFIID riid, void** ppContainer) {
    return m_d3d9->GetContainer(riid, ppContainer);
  }

  HRESULT STDMETHODCALLTYPE D3D8Surface::GetDesc(d3d8::D3DSURFACE_DESC* pDesc) {
    D3DSURFACE_DESC desc;
    HRESULT res = m_d3d9->GetDesc(&desc);
    ConvertSurfaceDesc8(&desc, pDesc);
    return res;
  }

  HRESULT STDMETHODCALLTYPE D3D8Surface::LockRect(d3d8::D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) {
    return m_d3d9->LockRect(reinterpret_cast<D3DLOCKED_RECT*>(pLockedRect), pRect, Flags);
  }

  HRESULT STDMETHODCALLTYPE D3D8Surface::UnlockRect() {
    return m_d3d9->UnlockRect();
  }

}
