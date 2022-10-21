#include "d3d8_volume.h"

#include "../d3d9/d3d9_volume.h"
#include "../d3d9/d3d9_device.h"

namespace dxvk {

  HRESULT STDMETHODCALLTYPE D3D8Volume::QueryInterface(REFIID riid, void** ppvObject) {
    // TODO
    return S_FALSE;
  }

  d3d8::D3DRESOURCETYPE STDMETHODCALLTYPE D3D8Volume::GetType() {
    return d3d8::D3DRTYPE_VOLUME;
  }

  ULONG STDMETHODCALLTYPE D3D8Volume::AddRef() {
    return m_d3d9->AddRef();
  }

  ULONG STDMETHODCALLTYPE D3D8Volume::Release() {
    return m_d3d9->Release();
  }

  HRESULT STDMETHODCALLTYPE D3D8Volume::GetPrivateData(
          REFGUID     refguid,
          void*       pData,
          DWORD*      pSizeOfData) {
   return m_d3d9->GetPrivateData(refguid, pData, pSizeOfData);
  }

  HRESULT STDMETHODCALLTYPE D3D8Volume::SetPrivateData(
          REFGUID     refguid,
    const void*       pData,
          DWORD       pSizeOfData,
          DWORD       Flags) {
    return m_d3d9->SetPrivateData(refguid, pData, pSizeOfData, Flags);
  }

  HRESULT STDMETHODCALLTYPE D3D8Volume::FreePrivateData(REFGUID refguid) {
    return m_d3d9->FreePrivateData(refguid);
  }

  HRESULT STDMETHODCALLTYPE D3D8Volume::GetDevice(d3d8::IDirect3DDevice8** ppDevice) {
    IDirect3DDevice9* d3d9Device;
    HRESULT res = m_d3d9->GetDevice(&d3d9Device);
    if (res != D3D_OK) {
      return res;
    }
    *ppDevice = static_cast<D3D9DeviceEx*>(d3d9Device)->GetD3D8Iface();
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE D3D8Volume::GetDesc(d3d8::D3DVOLUME_DESC* pDesc) {
    // TODO
    return S_FALSE;
  }

  HRESULT STDMETHODCALLTYPE D3D8Volume::LockBox(
          d3d8::D3DLOCKED_BOX* pLockedVolume,
          const d3d8::D3DBOX* pBox,
          DWORD Flags) {
   return m_d3d9->LockBox(reinterpret_cast<D3DLOCKED_BOX*>(pLockedVolume), reinterpret_cast<const D3DBOX*>(pBox), Flags);
  }

  HRESULT STDMETHODCALLTYPE D3D8Volume::UnlockBox() {
    return m_d3d9->UnlockBox();
  }

  HRESULT STDMETHODCALLTYPE D3D8Volume::GetContainer(REFIID riid, void** ppContainer) {
    return m_d3d9->GetContainer(riid, ppContainer);
  }

}
