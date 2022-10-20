#pragma once

// Implements IDirect3DVertexBuffer8 and IDirect3DIndexBuffer8

#include "d3d8_include.h"
#include "d3d8_resource.h"

namespace dxvk {

  template <typename D3D9, typename D3D8>
  class D3D8Buffer : public D3D8Resource<D3D9, D3D8> {

  public:

    D3D8Buffer(
            D3D8Device*   pDevice,
            Com<D3D9>&&     pBuffer)
      : D3D8Resource<D3D9, D3D8> ( pDevice, std::move(pBuffer) ) {
    }

    HRESULT STDMETHODCALLTYPE Lock(
            UINT   OffsetToLock,
            UINT   SizeToLock,
            BYTE** ppbData,
            DWORD  Flags) {
      return this->GetD3D9()->Lock(
        OffsetToLock,
        SizeToLock,
        reinterpret_cast<void**>(ppbData),
        Flags);
    }

    HRESULT STDMETHODCALLTYPE Unlock() {
      return this->GetD3D9()->Unlock();
    }

    void STDMETHODCALLTYPE PreLoad() {
      this->GetD3D9()->PreLoad();
    }

  };


  using D3D8VertexBufferBase = D3D8Buffer<IDirect3DVertexBuffer9, d3d8::IDirect3DVertexBuffer8>;
  class D3D8VertexBuffer final : public D3D8VertexBufferBase {

  public:

    D3D8VertexBuffer(
        D3D8Device*                       pDevice,
        Com<IDirect3DVertexBuffer9>&& pBuffer)
      : D3D8VertexBufferBase(pDevice, std::move(pBuffer)) {
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID  riid,
            void** ppvObject) final { return D3DERR_INVALIDCALL; }

    d3d8::D3DRESOURCETYPE STDMETHODCALLTYPE GetType() final { return d3d8::D3DRTYPE_VERTEXBUFFER; }

    HRESULT STDMETHODCALLTYPE GetDesc(d3d8::D3DVERTEXBUFFER_DESC* pDesc) final {
      // TODO: Remove reinterpret cast.
      return GetD3D9()->GetDesc(reinterpret_cast<D3DVERTEXBUFFER_DESC*>(pDesc));
    }

  };

  using D3D8IndexBufferBase = D3D8Buffer<IDirect3DIndexBuffer9, d3d8::IDirect3DIndexBuffer8>;
  class D3D8IndexBuffer final : public D3D8IndexBufferBase {

  public:

    D3D8IndexBuffer(
        D3D8Device*                      pDevice,
        Com<IDirect3DIndexBuffer9>&& pBuffer)
      : D3D8IndexBufferBase(pDevice, std::move(pBuffer)) {
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID  riid,
            void** ppvObject) final { return D3DERR_INVALIDCALL; }

    d3d8::D3DRESOURCETYPE STDMETHODCALLTYPE GetType() final { return d3d8::D3DRTYPE_INDEXBUFFER; }

    HRESULT STDMETHODCALLTYPE GetDesc(d3d8::D3DINDEXBUFFER_DESC* pDesc) final {
      // TODO: Remove reinterpret_cast.
      return GetD3D9()->GetDesc(reinterpret_cast<D3DINDEXBUFFER_DESC*>(pDesc));
    }

  };
}