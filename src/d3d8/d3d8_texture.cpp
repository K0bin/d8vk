#include "d3d8_texture.h"

#include "../d3d9/d3d9_texture.h"
#include "../d3d9/d3d9_surface.h"
#include "../d3d9/d3d9_volume.h"
#include "../d3d9/d3d9_device.h"
#include "d3d8_d3d9_util.h"

namespace dxvk {

  // D3D8Texture2D

  HRESULT STDMETHODCALLTYPE D3D8Texture2D::QueryInterface(REFIID riid, void** ppvObject) {
    return m_d3d9->QueryInterface(riid, ppvObject);
  }

  ULONG STDMETHODCALLTYPE D3D8Texture2D::AddRef() {
    return m_d3d9->AddRef();
  }

  ULONG STDMETHODCALLTYPE D3D8Texture2D::Release() {
    return m_d3d9->Release();
  }

  HRESULT STDMETHODCALLTYPE D3D8Texture2D::GetPrivateData(
          REFGUID     refguid,
          void*       pData,
          DWORD*      pSizeOfData) {
    return m_d3d9->GetPrivateData(refguid, pData, pSizeOfData);
  }

  HRESULT STDMETHODCALLTYPE D3D8Texture2D::SetPrivateData(
          REFGUID     refguid,
    const void*       pData,
          DWORD       pSizeOfData,
          DWORD       Flags) {
    return m_d3d9->SetPrivateData(refguid, pData, pSizeOfData, Flags);
  }

  HRESULT STDMETHODCALLTYPE D3D8Texture2D::FreePrivateData(REFGUID refguid) {
    return m_d3d9->FreePrivateData(refguid);
  }

  HRESULT STDMETHODCALLTYPE D3D8Texture2D::GetDevice(d3d8::IDirect3DDevice8** ppDevice) {
    IDirect3DDevice9* d3d9Device;
    HRESULT res = m_d3d9->GetDevice(&d3d9Device);
    if (res != D3D_OK) {
      return res;
    }
    *ppDevice = static_cast<D3D9DeviceEx*>(d3d9Device)->GetD3D8Iface();
    return D3D_OK;
  }

  DWORD STDMETHODCALLTYPE D3D8Texture2D::GetPriority() {
    return m_d3d9->GetPriority();
  }

  void STDMETHODCALLTYPE D3D8Texture2D::PreLoad() {
    m_d3d9->PreLoad();
  }

  DWORD STDMETHODCALLTYPE D3D8Texture2D::SetPriority(DWORD PriorityNew) {
    return m_d3d9->SetPriority(PriorityNew);
  }

  d3d8::D3DRESOURCETYPE STDMETHODCALLTYPE D3D8Texture2D::GetType() {
    return d3d8::D3DRESOURCETYPE(m_d3d9->GetType());
  }

  HRESULT STDMETHODCALLTYPE D3D8Texture2D::GetLevelDesc(UINT Level, d3d8::D3DSURFACE_DESC* pDesc) {
    D3DSURFACE_DESC desc;
    HRESULT res = m_d3d9->GetLevelDesc(Level, &desc);
    ConvertSurfaceDesc8(&desc, pDesc);
    return res;
  }

  DWORD STDMETHODCALLTYPE D3D8Texture2D::SetLOD(DWORD LODNew) {
    return m_d3d9->SetLOD(LODNew);
  }

  DWORD STDMETHODCALLTYPE D3D8Texture2D::GetLOD() {
    return m_d3d9->GetLOD();
  }

  DWORD STDMETHODCALLTYPE D3D8Texture2D::GetLevelCount() {
    return m_d3d9->GetLevelCount();
  }

  HRESULT STDMETHODCALLTYPE D3D8Texture2D::GetSurfaceLevel(UINT Level, d3d8::IDirect3DSurface8** ppSurfaceLevel) {
    IDirect3DSurface9* d3d9Surface;
    HRESULT res = m_d3d9->GetSurfaceLevel(Level, &d3d9Surface);
    if (res != D3D_OK) {
      return res;
    }
    *ppSurfaceLevel = static_cast<D3D9Surface*>(d3d9Surface)->GetD3D8Iface();
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE D3D8Texture2D::LockRect(UINT Level, d3d8::D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) {
    return m_d3d9->LockRect(Level, reinterpret_cast<D3DLOCKED_RECT*>(pLockedRect), pRect, Flags);
  }

  HRESULT STDMETHODCALLTYPE D3D8Texture2D::UnlockRect(UINT Level) {
    return m_d3d9->UnlockRect(Level);
  }

  HRESULT STDMETHODCALLTYPE D3D8Texture2D::AddDirtyRect(const RECT* pDirtyRect) {
    return m_d3d9->AddDirtyRect(pDirtyRect);
  }


  // D3D8TextureCube

  HRESULT STDMETHODCALLTYPE D3D8TextureCube::QueryInterface(REFIID riid, void** ppvObject) {
    return m_d3d9->QueryInterface(riid, ppvObject);
  }

  ULONG STDMETHODCALLTYPE D3D8TextureCube::AddRef() {
    return m_d3d9->AddRef();
  }

  ULONG STDMETHODCALLTYPE D3D8TextureCube::Release() {
    return m_d3d9->Release();
  }

  HRESULT STDMETHODCALLTYPE D3D8TextureCube::GetPrivateData(
          REFGUID     refguid,
          void*       pData,
          DWORD*      pSizeOfData) {
    return m_d3d9->GetPrivateData(refguid, pData, pSizeOfData);
  }

  HRESULT STDMETHODCALLTYPE D3D8TextureCube::SetPrivateData(
          REFGUID     refguid,
    const void*       pData,
          DWORD       pSizeOfData,
          DWORD       Flags) {
    return m_d3d9->SetPrivateData(refguid, pData, pSizeOfData, Flags);
  }

  HRESULT STDMETHODCALLTYPE D3D8TextureCube::FreePrivateData(REFGUID refguid) {
    return m_d3d9->FreePrivateData(refguid);
  }

  HRESULT STDMETHODCALLTYPE D3D8TextureCube::GetDevice(d3d8::IDirect3DDevice8** ppDevice) {
    IDirect3DDevice9* d3d9Device;
    HRESULT res = m_d3d9->GetDevice(&d3d9Device);
    if (res != D3D_OK) {
      return res;
    }
    *ppDevice = static_cast<D3D9DeviceEx*>(d3d9Device)->GetD3D8Iface();
    return D3D_OK;
  }

  DWORD STDMETHODCALLTYPE D3D8TextureCube::GetPriority() {
    return m_d3d9->GetPriority();
  }

  void STDMETHODCALLTYPE D3D8TextureCube::PreLoad() {
    m_d3d9->PreLoad();
  }

  DWORD STDMETHODCALLTYPE D3D8TextureCube::SetPriority(DWORD PriorityNew) {
    return m_d3d9->SetPriority(PriorityNew);
  }

  d3d8::D3DRESOURCETYPE STDMETHODCALLTYPE D3D8TextureCube::GetType() {
    return d3d8::D3DRESOURCETYPE(m_d3d9->GetType());
  }

  HRESULT STDMETHODCALLTYPE D3D8TextureCube::GetLevelDesc(UINT Level, d3d8::D3DSURFACE_DESC* pDesc) {
    D3DSURFACE_DESC desc;
    HRESULT res = m_d3d9->GetLevelDesc(Level, &desc);
    ConvertSurfaceDesc8(&desc, pDesc);
    return res;
  }

  DWORD STDMETHODCALLTYPE D3D8TextureCube::SetLOD(DWORD LODNew) {
    return m_d3d9->SetLOD(LODNew);
  }

  DWORD STDMETHODCALLTYPE D3D8TextureCube::GetLOD() {
    return m_d3d9->GetLOD();
  }

  DWORD STDMETHODCALLTYPE D3D8TextureCube::GetLevelCount() {
    return m_d3d9->GetLevelCount();
  }

  HRESULT STDMETHODCALLTYPE D3D8TextureCube::GetCubeMapSurface(d3d8::D3DCUBEMAP_FACES Face, UINT Level, d3d8::IDirect3DSurface8** ppSurfaceLevel) {
    IDirect3DSurface9* d3d9Surface;
    HRESULT res = m_d3d9->GetCubeMapSurface(D3DCUBEMAP_FACES(Face), Level, &d3d9Surface);
    if (res != D3D_OK) {
      return res;
    }
    *ppSurfaceLevel = static_cast<D3D9Surface*>(d3d9Surface)->GetD3D8Iface();
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE D3D8TextureCube::LockRect(d3d8::D3DCUBEMAP_FACES FaceType, UINT Level, d3d8::D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) {
    return m_d3d9->LockRect(D3DCUBEMAP_FACES(FaceType), Level, reinterpret_cast<D3DLOCKED_RECT*>(pLockedRect), pRect, Flags);
  }

  HRESULT STDMETHODCALLTYPE D3D8TextureCube::UnlockRect(d3d8::D3DCUBEMAP_FACES FaceType, UINT Level) {
    return m_d3d9->UnlockRect(D3DCUBEMAP_FACES(FaceType), Level);
  }

  HRESULT STDMETHODCALLTYPE D3D8TextureCube::AddDirtyRect(d3d8::D3DCUBEMAP_FACES Face, const RECT* pDirtyRect) {
    return m_d3d9->AddDirtyRect(D3DCUBEMAP_FACES(Face), pDirtyRect);
  }


  // D3D8Texture3D

  HRESULT STDMETHODCALLTYPE D3D8Texture3D::QueryInterface(REFIID riid, void** ppvObject) {
    return m_d3d9->QueryInterface(riid, ppvObject);
  }

  ULONG STDMETHODCALLTYPE D3D8Texture3D::AddRef() {
    return m_d3d9->AddRef();
  }

  ULONG STDMETHODCALLTYPE D3D8Texture3D::Release() {
    return m_d3d9->Release();
  }

  HRESULT STDMETHODCALLTYPE D3D8Texture3D::GetPrivateData(
          REFGUID     refguid,
          void*       pData,
          DWORD*      pSizeOfData) {
    return m_d3d9->GetPrivateData(refguid, pData, pSizeOfData);
  }

  HRESULT STDMETHODCALLTYPE D3D8Texture3D::SetPrivateData(
          REFGUID     refguid,
    const void*       pData,
          DWORD       pSizeOfData,
          DWORD       Flags) {
    return m_d3d9->SetPrivateData(refguid, pData, pSizeOfData, Flags);
  }

  HRESULT STDMETHODCALLTYPE D3D8Texture3D::FreePrivateData(REFGUID refguid) {
    return m_d3d9->FreePrivateData(refguid);
  }

  HRESULT STDMETHODCALLTYPE D3D8Texture3D::GetDevice(d3d8::IDirect3DDevice8** ppDevice) {
    IDirect3DDevice9* d3d9Device;
    HRESULT res = m_d3d9->GetDevice(&d3d9Device);
    if (res != D3D_OK) {
      return res;
    }
    *ppDevice = static_cast<D3D9DeviceEx*>(d3d9Device)->GetD3D8Iface();
    return D3D_OK;
  }

  DWORD STDMETHODCALLTYPE D3D8Texture3D::GetPriority() {
    return m_d3d9->GetPriority();
  }

  void STDMETHODCALLTYPE D3D8Texture3D::PreLoad() {
    m_d3d9->PreLoad();
  }

  DWORD STDMETHODCALLTYPE D3D8Texture3D::SetPriority(DWORD PriorityNew) {
    return m_d3d9->SetPriority(PriorityNew);
  }

  d3d8::D3DRESOURCETYPE STDMETHODCALLTYPE D3D8Texture3D::GetType() {
    return d3d8::D3DRESOURCETYPE(m_d3d9->GetType());
  }

  HRESULT STDMETHODCALLTYPE D3D8Texture3D::GetLevelDesc(UINT Level, d3d8::D3DVOLUME_DESC* pDesc) {
    /*D3DSURFACE_DESC desc;
    HRESULT res = m_d3d9->GetLevelDesc(Level, &desc);
    ConvertSurfaceDesc8(&desc, pDesc);
    return res;*/
    return S_FALSE;
  }

  DWORD STDMETHODCALLTYPE D3D8Texture3D::SetLOD(DWORD LODNew) {
    return m_d3d9->SetLOD(LODNew);
  }

  DWORD STDMETHODCALLTYPE D3D8Texture3D::GetLOD() {
    return m_d3d9->GetLOD();
  }

  DWORD STDMETHODCALLTYPE D3D8Texture3D::GetLevelCount() {
    return m_d3d9->GetLevelCount();
  }

  HRESULT STDMETHODCALLTYPE D3D8Texture3D::GetVolumeLevel(UINT Level, d3d8::IDirect3DVolume8** ppVolumeLevel) {
    IDirect3DVolume9* d3d9Volume;
    HRESULT res = m_d3d9->GetVolumeLevel(Level, &d3d9Volume);
    if (res != D3D_OK) {
      return res;
    }
    *ppVolumeLevel = static_cast<D3D9Volume*>(d3d9Volume)->GetD3D8Iface();
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE D3D8Texture3D::LockBox(
            UINT Level,
            d3d8::D3DLOCKED_BOX* pLockedVolume,
            const d3d8::D3DBOX* pBox,
            DWORD Flags) {
    return m_d3d9->LockBox(Level, reinterpret_cast<D3DLOCKED_BOX*>(pLockedVolume), reinterpret_cast<const D3DBOX*>(pBox), Flags);
  }

  HRESULT STDMETHODCALLTYPE D3D8Texture3D::UnlockBox(UINT Level) {
    return m_d3d9->UnlockBox(Level);
  }

  HRESULT STDMETHODCALLTYPE D3D8Texture3D::AddDirtyBox(const d3d8::D3DBOX* pDirtyBox) {
    return m_d3d9->AddDirtyBox(reinterpret_cast<const D3DBOX*>(pDirtyBox));
  }

}
