/**
 * DX9 Hook, based on the
 * DirectX 9 Output interceptor
 * by Tom Gascoigne <tom@gascoigne.me>
 * https://github.com/HuiyingLi/D3D-Hook-Test/blob/master/D3DHook/dllmain.cpp
 */
#include <d3d9.h>
#include <d3dx9.h>
#include <cstdint>
#include <fstream>

#include "virtual_protect.h"
#include "hook_create_device.h"

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
  (void)hModule;
  (void)lpReserved;

  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
      const auto hook{ HookCreateDevice() };
      if (hook != D3D_OK) {
        MessageBoxA(NULL, "Unable to hook", "Unable to hook", MB_ICONEXCLAMATION);
        return FALSE;
      }
      break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
      break;
    case DLL_PROCESS_DETACH:
      break;
  }

  return TRUE;
}
