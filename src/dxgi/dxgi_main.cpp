#include "dxgi_factory.h"
#include "dxgi_include.h"
#include <d3dcommon.h>

namespace dxvk {
  
  Logger Logger::s_instance("dxgi.log");
  
  HRESULT createDxgiFactory(UINT Flags, REFIID riid, void **ppFactory) {
    try {
      Com<DxgiFactory> factory = new DxgiFactory(Flags);
      HRESULT hr = factory->QueryInterface(riid, ppFactory);

      if (FAILED(hr))
        return hr;
      
      return S_OK;
    } catch (const DxvkError& e) {
      Logger::err(e.message());
      return E_FAIL;
    }
  }
}

extern "C" {
  DLLEXPORT HRESULT __stdcall CreateDXGIFactory2(UINT Flags, REFIID riid, void **ppFactory) {
    dxvk::Logger::warn("CreateDXGIFactory2: Ignoring flags");
    return dxvk::createDxgiFactory(Flags, riid, ppFactory);
  }

  DLLEXPORT HRESULT __stdcall CreateDXGIFactory1(REFIID riid, void **ppFactory) {
    return dxvk::createDxgiFactory(0, riid, ppFactory);
  }
  
  DLLEXPORT HRESULT __stdcall CreateDXGIFactory(REFIID riid, void **ppFactory) {
    return dxvk::createDxgiFactory(0, riid, ppFactory);
  }

  typedef HRESULT __stdcall (*PFN_D3D11CoreCreateDevice)(
            IDXGIFactory*       pFactory,
            IDXGIAdapter*       pAdapter,
            UINT                Flags,
      const D3D_FEATURE_LEVEL*  pFeatureLevels,
            UINT                FeatureLevels,
            void**              ppDevice);

  DLLEXPORT HRESULT __stdcall DXGID3D10CreateDevice(
          HMODULE             d3d10core,
          IDXGIFactory*       pFactory,
          IDXGIAdapter*       pAdapter,
          UINT                Flags,
    const D3D_FEATURE_LEVEL*  pFeatureLevels,
          UINT                FeatureLevels,
          void**              ppDevice) {
    PFN_D3D11CoreCreateDevice pD3D11CoreCreateDevice = (PFN_D3D11CoreCreateDevice)GetProcAddress(d3d10core, "D3D11CoreCreateDevice");
    return pD3D11CoreCreateDevice(pFactory, pAdapter, Flags, pFeatureLevels, FeatureLevels, ppDevice);
  }

  DLLEXPORT HRESULT __stdcall DXGIDeclareAdapterRemovalSupport() {
    static bool enabled = false;

    if (std::exchange(enabled, true))
      return 0x887a0036; // DXGI_ERROR_ALREADY_EXISTS;

    dxvk::Logger::warn("DXGIDeclareAdapterRemovalSupport: Stub");
    return S_OK;
  }

  DLLEXPORT HRESULT __stdcall DXGIGetDebugInterface1(UINT Flags, REFIID riid, void **ppDebug) {
    static bool errorShown = false;

    if (!std::exchange(errorShown, true))
      dxvk::Logger::warn("DXGIGetDebugInterface1: Stub");

    return E_NOINTERFACE;
  }

}
