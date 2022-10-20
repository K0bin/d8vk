
// Main: the location of Direct3DCreate8 and other core exported functions

#include "d3d8_interface.h"
//#include "d3d9_shader_validator.h"

class D3DFE_PROCESSVERTICES;
using PSGPERRORID = UINT;

namespace dxvk {
  Logger Logger::s_instance("d3d8.log");

  HRESULT CreateD3D8(
          UINT         SDKVersion,
          IDirect3D8** ppDirect3D8) {
    if (!ppDirect3D8)
      return D3DERR_INVALIDCALL;

    D3D9InterfaceEx* d3d9Iface = ref(new D3D9InterfaceEx( Extended ));
    *ppDirect3D8 = d3d9Iface->GetD3D8Iface();

    return D3D_OK;
  }
}

extern "C" {

  DLLEXPORT IDirect3D8* __stdcall Direct3DCreate8(UINT nSDKVersion) {
    //dxvk::CreateD3D9(false, &pDirect3D);

    dxvk::Logger::trace("Direct3DCreate8 called");

    IDirect3D8* pDirect3D = nullptr;
    dxvk::CreateD3D8(false, &pDirect3D);

    return pDirect3D;
  }
}
