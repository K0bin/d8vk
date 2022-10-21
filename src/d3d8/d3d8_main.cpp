
// Main: the location of Direct3DCreate8 and other core exported functions

#include "d3d8_include.h"

extern "C" {
  DLLEXPORT d3d8::IDirect3D8* __stdcall Direct3DCreate8(UINT nSDKVersion) {
    IDirect3D9* d3d9Iface = Direct3DCreate9(nSDKVersion);
    d3d8::IDirect3D8* d3d8Iface = nullptr;
    d3d9Iface->QueryInterface(__uuidof(d3d8::IDirect3D8), reinterpret_cast<void**>(&d3d8Iface));
    return d3d8Iface;
  }
}
