#pragma once

#include "d3d8_include.h"

namespace dxvk {

  class D3D9Texture2D;
  class D3D9TextureCube;
  class D3D9Surface;
  class D3D9Texture3D;
  class D3D9Volume;

  class D3D8Texture2D final : public d3d8::IDirect3DTexture8 {

  public:

    D3D8Texture2D(D3D9Texture2D* pParent)
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

    DWORD STDMETHODCALLTYPE GetPriority();

    void STDMETHODCALLTYPE PreLoad();

    DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew);

    HRESULT STDMETHODCALLTYPE GetLevelDesc(UINT Level, d3d8::D3DSURFACE_DESC* pDesc);

    DWORD STDMETHODCALLTYPE SetLOD(DWORD LodNew);

    DWORD STDMETHODCALLTYPE GetLOD();

    DWORD STDMETHODCALLTYPE GetLevelCount();

    HRESULT STDMETHODCALLTYPE GetSurfaceLevel(UINT Level, d3d8::IDirect3DSurface8** ppSurfaceLevel);

    HRESULT STDMETHODCALLTYPE LockRect(UINT Level, d3d8::D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags);

    HRESULT STDMETHODCALLTYPE UnlockRect(UINT Level);

    HRESULT STDMETHODCALLTYPE AddDirtyRect(CONST RECT* pDirtyRect);

    D3D9Texture2D* GetD3D9Iface() {
      return m_d3d9;
    }

  private:

    D3D9Texture2D* m_d3d9;

  };

  class D3D8TextureCube final : public d3d8::IDirect3DCubeTexture8 {

  public:

    D3D8TextureCube(D3D9TextureCube* pParent)
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

    DWORD STDMETHODCALLTYPE GetPriority();

    void STDMETHODCALLTYPE PreLoad();

    DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew);

    HRESULT STDMETHODCALLTYPE GetLevelDesc(UINT Level, d3d8::D3DSURFACE_DESC* pDesc);

    DWORD STDMETHODCALLTYPE SetLOD(DWORD LodNew);

    DWORD STDMETHODCALLTYPE GetLOD();

    DWORD STDMETHODCALLTYPE GetLevelCount();

    HRESULT STDMETHODCALLTYPE GetCubeMapSurface(d3d8::D3DCUBEMAP_FACES Face, UINT Level, d3d8::IDirect3DSurface8** ppSurfaceLevel);

    HRESULT STDMETHODCALLTYPE LockRect(
            d3d8::D3DCUBEMAP_FACES Face,
            UINT Level,
            d3d8::D3DLOCKED_RECT* pLockedRect,
            const RECT* pRect,
            DWORD Flags);

    HRESULT STDMETHODCALLTYPE UnlockRect(d3d8::D3DCUBEMAP_FACES Face, UINT Level);

    HRESULT STDMETHODCALLTYPE AddDirtyRect(d3d8::D3DCUBEMAP_FACES Face, const RECT* pDirtyRect);

    D3D9TextureCube* GetD3D9Iface() {
      return m_d3d9;
    }

  private:

    D3D9TextureCube* m_d3d9;

  };

  class D3D8Texture3D final : public d3d8::IDirect3DVolumeTexture8 {

  public:

    D3D8Texture3D(D3D9Texture3D* pParent)
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

    DWORD STDMETHODCALLTYPE GetPriority();

    void STDMETHODCALLTYPE PreLoad();

    DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew);

    HRESULT STDMETHODCALLTYPE GetLevelDesc(UINT Level, d3d8::D3DVOLUME_DESC* pDesc);

    DWORD STDMETHODCALLTYPE SetLOD(DWORD LodNew);

    DWORD STDMETHODCALLTYPE GetLOD();

    DWORD STDMETHODCALLTYPE GetLevelCount();

    HRESULT STDMETHODCALLTYPE GetVolumeLevel(UINT Level, d3d8::IDirect3DVolume8** ppVolumeLevel);

    HRESULT STDMETHODCALLTYPE LockBox(
            UINT Level,
            d3d8::D3DLOCKED_BOX* pLockedVolume,
            const d3d8::D3DBOX* pBox,
            DWORD Flags);

    HRESULT STDMETHODCALLTYPE UnlockBox(UINT Level);

    HRESULT STDMETHODCALLTYPE AddDirtyBox(const d3d8::D3DBOX* pDirtyBox);

    D3D9Texture3D* GetD3D9Iface() {
      return m_d3d9;
    }

  private:

    D3D9Texture3D* m_d3d9;

  };

}