#pragma once

/**
* Utility functions for converting
* between DirectX8 and DirectX9 types.
*/

#include "d3d8_include.h"

namespace dxvk {

  // (8<-9) D3DCAPSX: Writes to D3DCAPS8 from D3DCAPS9
  inline void ConvertCaps8(const D3DCAPS9& caps9, d3d8::D3DCAPS8* pCaps8) {

    // should be aligned
    std::memcpy(pCaps8, &caps9, sizeof(d3d8::D3DCAPS8));

    // This was removed by D3D9. We can probably render windowed.
    pCaps8->Caps2 |= D3DCAPS2_CANRENDERWINDOWED;
  }

  // (9<-8) D3DD3DPRESENT_PARAMETERS: Returns D3D9's params given an input for D3D8
  inline D3DPRESENT_PARAMETERS ConvertPresentParameters9(const d3d8::D3DPRESENT_PARAMETERS* pParams) {

    D3DPRESENT_PARAMETERS params;
    params.BackBufferWidth = pParams->BackBufferWidth;
    params.BackBufferHeight = pParams->BackBufferHeight;
    params.BackBufferFormat = D3DFORMAT(pParams->BackBufferFormat);
    params.BackBufferCount = pParams->BackBufferCount;

    params.MultiSampleType = D3DMULTISAMPLE_TYPE(pParams->MultiSampleType);
    params.MultiSampleQuality = 0; // (D3D8: no MultiSampleQuality), TODO: get a value for this


    UINT PresentationInterval = pParams->FullScreen_PresentationInterval;

    if (pParams->Windowed) {

      if (unlikely(PresentationInterval != D3DPRESENT_INTERVAL_DEFAULT)) {
        // TODO: what does dx8 do if windowed app sets FullScreen_PresentationInterval?
        Logger::warn(str::format(
          "D3D8 Application is windowed yet requested FullScreen_PresentationInterval ", PresentationInterval,
          " (should be D3DPRESENT_INTERVAL_DEFAULT). This will be ignored."));
      }

      // D3D8: For windowed swap chain, the back buffer is copied to the window immediately.
      PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    }

    d3d8::D3DSWAPEFFECT SwapEffect = d3d8::D3DSWAPEFFECT(pParams->SwapEffect);

    // D3DSWAPEFFECT_COPY_VSYNC has been removed
    if (SwapEffect == d3d8::D3DSWAPEFFECT_COPY_VSYNC) {

      SwapEffect = d3d8::D3DSWAPEFFECT_COPY;

      // D3D8: In windowed mode, D3DSWAPEFFECT_COPY_VSYNC enables VSYNC.
      // In fullscreen, D3DPRESENT_INTERVAL_IMMEDIATE is meaningless.
      if (pParams->Windowed || (PresentationInterval & D3DPRESENT_INTERVAL_IMMEDIATE) != 0) {
        PresentationInterval = D3DPRESENT_INTERVAL_ONE;
        // TODO: what does dx8 do if multiple D3DPRESENT_INTERVAL flags are set? 
      }
    }

    params.SwapEffect = D3DSWAPEFFECT(SwapEffect);
    params.hDeviceWindow = pParams->hDeviceWindow;
    params.Windowed = pParams->Windowed;
    params.EnableAutoDepthStencil = pParams->EnableAutoDepthStencil;
    params.AutoDepthStencilFormat = D3DFORMAT(pParams->AutoDepthStencilFormat);
    params.Flags = pParams->Flags;

    params.FullScreen_RefreshRateInHz = pParams->FullScreen_RefreshRateInHz;

    // FullScreen_PresentationInterval -> PresentationInterval
    params.PresentationInterval = PresentationInterval;

    return params;
  }

  inline UINT GetFormatBPP(const d3d8::D3DFORMAT fmt) {
    // TODO: get bpp based on format
    return 32;
  }

  // (8<-9) Convert D3DSURFACE_DESC
  inline void ConvertSurfaceDesc8(const D3DSURFACE_DESC* pSurf9, d3d8::D3DSURFACE_DESC* pSurf8) {
    pSurf8->Format = d3d8::D3DFORMAT(pSurf9->Format);
    pSurf8->Type = d3d8::D3DRESOURCETYPE(pSurf9->Type);
    pSurf8->Usage = pSurf9->Usage;
    pSurf8->Pool = d3d8::D3DPOOL(pSurf9->Pool);
    pSurf8->Size = pSurf9->Width * pSurf9->Height * GetFormatBPP(pSurf8->Format);

    pSurf8->MultiSampleType = d3d8::D3DMULTISAMPLE_TYPE(pSurf9->MultiSampleType);
    // DX8: No multisample quality
    pSurf8->Width = pSurf9->Width;
    pSurf8->Height = pSurf9->Height;
  }

  // If this D3DTEXTURESTAGESTATETYPE has been remapped to a D3DSAMPLERSTATETYPE
  // it will be returned, otherwise returns -1
  inline D3DSAMPLERSTATETYPE GetSamplerStateType9(const d3d8::D3DTEXTURESTAGESTATETYPE StageType) {
    switch (StageType) {
      // 13-21:
      case d3d8::D3DTSS_ADDRESSU:       return D3DSAMP_ADDRESSU;
      case d3d8::D3DTSS_ADDRESSV:       return D3DSAMP_ADDRESSV;
      case d3d8::D3DTSS_BORDERCOLOR:    return D3DSAMP_BORDERCOLOR;
      case d3d8::D3DTSS_MAGFILTER:      return D3DSAMP_MAGFILTER;
      case d3d8::D3DTSS_MINFILTER:      return D3DSAMP_MINFILTER;
      case d3d8::D3DTSS_MIPFILTER:      return D3DSAMP_MIPFILTER;
      case d3d8::D3DTSS_MIPMAPLODBIAS:  return D3DSAMP_MIPMAPLODBIAS;
      case d3d8::D3DTSS_MAXMIPLEVEL:    return D3DSAMP_MIPFILTER;
      case d3d8::D3DTSS_MAXANISOTROPY:  return D3DSAMP_MAXANISOTROPY;
      // 25:
      case d3d8::D3DTSS_ADDRESSW:       return D3DSAMP_ADDRESSW;
      default:                    return D3DSAMPLERSTATETYPE(-1);
    }
  }
}

