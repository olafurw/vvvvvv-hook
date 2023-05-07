#include "create_device.h"

#include "virtual_table.h"
#include "virtual_protect.h"

#include "end_scene.h"

CreateDevice_t D3DCreateDevice_orig;

/**
 * This is a thread which indefinately resets the vtable pointers to our own functions
 * This is needed because the program might set these pointers back to
 * their original values at any point
 */
// waage maybe not needed?
DWORD WINAPI VTablePatchThread(LPVOID threadParam) {
  (void)threadParam;
  while (true) {
    Sleep(100);
    *(DWORD *)&IDirect3D9_vtable[ENDSCENE_VTI] = (DWORD)&D3DEndSceneHook;
  }
}

/**
 * Thanks to HookCreateDevice(), The program should now call this method instead of
 * Direct3D's CreateDevice method. This allows us to then hook the IDirect3DDevice9
 * methods we need
 */
HRESULT WINAPI D3DCreateDeviceHook(
    IDirect3D9 * Direct3D_Object, 
    UINT Adapter, 
    D3DDEVTYPE DeviceType, 
    HWND hFocusWindow, 
    DWORD BehaviorFlags,
    D3DPRESENT_PARAMETERS * pPresentationParameters, 
    IDirect3DDevice9 ** ppReturnedDeviceInterface) {
  const HRESULT result{ D3DCreateDevice_orig(Direct3D_Object, Adapter, DeviceType, hFocusWindow, BehaviorFlags | D3DCREATE_MULTITHREADED, pPresentationParameters, ppReturnedDeviceInterface) };
  if (result != D3D_OK) {
    MessageBoxA(NULL, "Unable to create original device", "Unable to create original device", MB_ICONEXCLAMATION);
    return result;
  }

  // Now we've intercepted the program's call to CreateDevice and we have the IDirect3DDevice9 that it uses
  // We can get it's vtable and patch in our own detours
  // Reset the CreateDevice hook since it's no longer needed
  // Unprotect the vtable for writing
  Protect protect(&IDirect3D9_vtable[CREATEDEVICE_VTI], sizeof(DWORD), PAGE_READWRITE, [&]() {
    *(DWORD *)&IDirect3D9_vtable[CREATEDEVICE_VTI] = (DWORD)D3DCreateDevice_orig;
  });

  IDirect3D9_vtable = (DWORD *)*(DWORD *)*ppReturnedDeviceInterface;

  *(PDWORD)&D3DEndScene_orig = (DWORD)IDirect3D9_vtable[ENDSCENE_VTI];

  if (!CreateThread(NULL, 0, VTablePatchThread, NULL, NULL, NULL)) {
    MessageBoxA(NULL, "Unable to create thread to patch vtable", "Unable to create thread to patch vtable", MB_ICONEXCLAMATION);
    return D3DERR_INVALIDCALL;
  }

  return result;
}