#pragma once

// Implements IDirect3DVertexBuffer8 and IDirect3DIndexBuffer8

#include "d3d8_include.h"
#include "d3d8_resource.h"

namespace dxvk {

  class D3D9VertexBuffer;
  class D3D9IndexBuffer;

  class D3D8VertexBuffer final : public d3d8::IDirect3DVertexBuffer8 {

  public:

    D3D8VertexBuffer(D3D9VertexBuffer* pParent)
      : m_d3d9(pParent) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID  riid,
            void** ppvObject) final;

    d3d8::D3DRESOURCETYPE STDMETHODCALLTYPE GetType() final;

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

    HRESULT STDMETHODCALLTYPE GetDesc(d3d8::D3DVERTEXBUFFER_DESC* pDesc) final;

    HRESULT STDMETHODCALLTYPE Lock(
            UINT   OffsetToLock,
            UINT   SizeToLock,
            BYTE** ppbData,
            DWORD  Flags);

    HRESULT STDMETHODCALLTYPE Unlock();

    DWORD STDMETHODCALLTYPE GetPriority();

    void STDMETHODCALLTYPE PreLoad();

    DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew);

    D3D9VertexBuffer* GetD3D9Iface() {
      return m_d3d9;
    }

  private:

    D3D9VertexBuffer* m_d3d9;

  };

  class D3D8IndexBuffer final : public d3d8::IDirect3DIndexBuffer8 {

  public:

    D3D8IndexBuffer(D3D9IndexBuffer* pParent)
      : m_d3d9(pParent) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID  riid,
            void** ppvObject) final;

    d3d8::D3DRESOURCETYPE STDMETHODCALLTYPE GetType() final;

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

    HRESULT STDMETHODCALLTYPE GetDesc(d3d8::D3DINDEXBUFFER_DESC* pDesc) final;

    HRESULT STDMETHODCALLTYPE Lock(
            UINT   OffsetToLock,
            UINT   SizeToLock,
            BYTE** ppbData,
            DWORD  Flags);

    HRESULT STDMETHODCALLTYPE Unlock();

    DWORD STDMETHODCALLTYPE GetPriority();

    void STDMETHODCALLTYPE PreLoad();

    DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew);

    D3D9IndexBuffer* GetD3D9Iface() {
      return m_d3d9;
    }

  private:

    D3D9IndexBuffer* m_d3d9;

  };
}