
// Main: the location of Direct3DCreate8 and other core exported functions

#include "d3d8_interface.h"
//#include "d3d9_shader_validator.h"

#include "../d3d9/d3d9_interface.h"

class D3DFE_PROCESSVERTICES;
using PSGPERRORID = UINT;

extern "C" {
  DLLEXPORT d3d8::IDirect3D8* __stdcall Direct3DCreate8(UINT nSDKVersion) {
    IDirect3D9* d3d9Iface = Direct3DCreate9(nSDKVersion);
    return static_cast<dxvk::D3D9InterfaceEx*>(d3d9Iface)->GetD3D8Iface();
  }
}
