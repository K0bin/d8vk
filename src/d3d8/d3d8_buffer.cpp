#include "d3d8_buffer.h"

#include "../d3d9/d3d9_buffer.h"

namespace dxvk {

  // D3D8VertexBuffer

  HRESULT STDMETHODCALLTYPE D3D8VertexBuffer::QueryInterface(
            REFIID  riid,
            void** ppvObject) {
    return S_FALSE;
  }

  ULONG STDMETHODCALLTYPE D3D8VertexBuffer::AddRef() {
    return m_d3d9->AddRef();
  }

  ULONG STDMETHODCALLTYPE D3D8VertexBuffer::Release() {
    return m_d3d9->Release();
  }

  HRESULT STDMETHODCALLTYPE D3D8VertexBuffer::GetPrivateData(
          REFGUID     refguid,
          void*       pData,
          DWORD*      pSizeOfData) {
    return m_d3d9->GetPrivateData(refguid, pData, pSizeOfData);
  }

  HRESULT STDMETHODCALLTYPE D3D8VertexBuffer::SetPrivateData(
          REFGUID     refguid,
    const void*       pData,
          DWORD       pSizeOfData,
          DWORD       Flags) {
    return m_d3d9->SetPrivateData(refguid, pData, pSizeOfData, Flags);
  }

  HRESULT STDMETHODCALLTYPE D3D8VertexBuffer::FreePrivateData(REFGUID refguid) {
    return m_d3d9->FreePrivateData(refguid);
  }

  HRESULT STDMETHODCALLTYPE D3D8VertexBuffer::GetDevice(d3d8::IDirect3DDevice8** ppDevice) {
    IDirect3DDevice9* d3d9Device;
    HRESULT res = m_d3d9->GetDevice(&d3d9Device);
    if (res != D3D_OK) {
      return res;
    }
    //*ppDevice = static_cast<D3D9Device*>(d3d9Device)->GetD3D8Iface();
    return D3D_OK;
  }

  d3d8::D3DRESOURCETYPE STDMETHODCALLTYPE D3D8VertexBuffer::GetType() {
    return d3d8::D3DRESOURCETYPE(m_d3d9->GetType());
  }

  HRESULT STDMETHODCALLTYPE D3D8VertexBuffer::GetDesc(d3d8::D3DVERTEXBUFFER_DESC* pDesc) {
    return S_FALSE;
  }

  HRESULT STDMETHODCALLTYPE D3D8VertexBuffer::Lock(
          UINT   OffsetToLock,
          UINT   SizeToLock,
          BYTE** ppbData,
          DWORD  Flags) {
    return m_d3d9->Lock(OffsetToLock, SizeToLock, reinterpret_cast<void**>(ppbData), Flags);
  }

  HRESULT STDMETHODCALLTYPE D3D8VertexBuffer::Unlock() {
    return m_d3d9->Unlock();
  }

  void STDMETHODCALLTYPE D3D8VertexBuffer::PreLoad() {
    m_d3d9->PreLoad();
  }

  DWORD STDMETHODCALLTYPE D3D8VertexBuffer::GetPriority() {
    return m_d3d9->GetPriority();
  }

  DWORD STDMETHODCALLTYPE D3D8VertexBuffer::SetPriority(DWORD PriorityNew) {
    return m_d3d9->SetPriority(PriorityNew);
  }


  // D3D8IndexBuffer

  HRESULT STDMETHODCALLTYPE D3D8IndexBuffer::QueryInterface(
            REFIID  riid,
            void** ppvObject) {
    return S_FALSE;
  }

  ULONG STDMETHODCALLTYPE D3D8IndexBuffer::AddRef() {
    return m_d3d9->AddRef();
  }

  ULONG STDMETHODCALLTYPE D3D8IndexBuffer::Release() {
    return m_d3d9->Release();
  }

  HRESULT STDMETHODCALLTYPE D3D8IndexBuffer::GetPrivateData(
          REFGUID     refguid,
          void*       pData,
          DWORD*      pSizeOfData) {
    return m_d3d9->GetPrivateData(refguid, pData, pSizeOfData);
  }

  HRESULT STDMETHODCALLTYPE D3D8IndexBuffer::SetPrivateData(
          REFGUID     refguid,
    const void*       pData,
          DWORD       pSizeOfData,
          DWORD       Flags) {
    return m_d3d9->SetPrivateData(refguid, pData, pSizeOfData, Flags);
  }

  HRESULT STDMETHODCALLTYPE D3D8IndexBuffer::FreePrivateData(REFGUID refguid) {
    return m_d3d9->FreePrivateData(refguid);
  }

  HRESULT STDMETHODCALLTYPE D3D8IndexBuffer::GetDevice(d3d8::IDirect3DDevice8** ppDevice) {
    IDirect3DDevice9* d3d9Device;
    HRESULT res = m_d3d9->GetDevice(&d3d9Device);
    if (res != D3D_OK) {
      return res;
    }
    //*ppDevice = static_cast<D3D9Device*>(d3d9Device)->GetD3D8Iface();
    return D3D_OK;
  }

  d3d8::D3DRESOURCETYPE STDMETHODCALLTYPE D3D8IndexBuffer::GetType() {
    return d3d8::D3DRESOURCETYPE(m_d3d9->GetType());
  }

  HRESULT STDMETHODCALLTYPE D3D8IndexBuffer::GetDesc(d3d8::D3DINDEXBUFFER_DESC* pDesc) {
    return S_FALSE;
  }

  HRESULT STDMETHODCALLTYPE D3D8IndexBuffer::Lock(
          UINT   OffsetToLock,
          UINT   SizeToLock,
          BYTE** ppbData,
          DWORD  Flags) {
    return m_d3d9->Lock(OffsetToLock, SizeToLock, reinterpret_cast<void**>(ppbData), Flags);
  }

  HRESULT STDMETHODCALLTYPE D3D8IndexBuffer::Unlock() {
    return m_d3d9->Unlock();
  }

  void STDMETHODCALLTYPE D3D8IndexBuffer::PreLoad() {
    m_d3d9->PreLoad();
  }

  DWORD STDMETHODCALLTYPE D3D8IndexBuffer::GetPriority() {
    return m_d3d9->GetPriority();
  }

  DWORD STDMETHODCALLTYPE D3D8IndexBuffer::SetPriority(DWORD PriorityNew) {
    return m_d3d9->SetPriority(PriorityNew);
  }


}
