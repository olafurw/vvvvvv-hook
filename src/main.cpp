/**
 * DX9 Hook, based on the
 * DirectX 9 Output interceptor
 * by Tom Gascoigne <tom@gascoigne.me>
 * https://github.com/HuiyingLi/D3D-Hook-Test/blob/master/D3DHook/dllmain.cpp
 */
#include <d3d9.h>
#include <d3dx9.h>

// Prototypes for d3d functions we want to hook
typedef HRESULT(WINAPI * CreateDevice_t)(IDirect3D9 * Direct3D_Object, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS * pPresentationParameters, IDirect3DDevice9 ** ppReturnedDeviceInterface);
typedef HRESULT(WINAPI * EndScene_t)(IDirect3DDevice9 * surface);

// Function pointers to the original functions
CreateDevice_t D3DCreateDevice_orig;
EndScene_t D3DEndScene_orig;

// Our hooking functions
HRESULT WINAPI D3DCreateDevice_hook(IDirect3D9 * Direct3D_Object, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS * pPresentationParameters, IDirect3DDevice9 ** ppReturnedDeviceInterface);
HRESULT WINAPI D3DEndScene_hook(IDirect3DDevice9 * device);

// vtable stuff
PDWORD IDirect3D9_vtable = NULL;

// vtable offsets
#define CREATEDEVICE_VTI 16
#define ENDSCENE_VTI 42

HRESULT WINAPI HookCreateDevice();
DWORD WINAPI VTablePatchThread(LPVOID threadParam);

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
    case DLL_PROCESS_DETACH:
      break;
  }

  return TRUE;
}

/**
 * This function sets up the hook for CreateDevice by replacing the pointer
 * to CreateDevice within IDirect3D9's vtable.
 */
HRESULT WINAPI HookCreateDevice() {
  IDirect3D9 * device{ Direct3DCreate9(D3D_SDK_VERSION) };
  if (!device) {
    return D3DERR_INVALIDCALL;
  }

  IDirect3D9_vtable = (DWORD *)*(DWORD *)device;
  device->Release();

  DWORD protectFlag{ 0 };
  const BOOL unlock{ VirtualProtect(&IDirect3D9_vtable[CREATEDEVICE_VTI], sizeof(DWORD), PAGE_READWRITE, &protectFlag) };
  if (!unlock) {
    MessageBoxA(NULL, "Unable to access vtable", "CreateDevice", MB_ICONEXCLAMATION);
    return D3DERR_INVALIDCALL;
  }

  // Store the original CreateDevice pointer and shove our own function into the vtable
  *(DWORD *)&D3DCreateDevice_orig = IDirect3D9_vtable[CREATEDEVICE_VTI];
  *(DWORD *)&IDirect3D9_vtable[CREATEDEVICE_VTI] = (DWORD)D3DCreateDevice_hook;

  const BOOL lock{ VirtualProtect(&IDirect3D9_vtable[CREATEDEVICE_VTI], sizeof(DWORD), protectFlag, &protectFlag) };
  if (!lock) {
    MessageBoxA(NULL, "Unable to access vtable", "CreateDevice", MB_ICONEXCLAMATION);
    return D3DERR_INVALIDCALL;
  }

  return D3D_OK;
}

/**
 * Thanks to HookCreateDevice(), The program should now call this method instead of
 * Direct3D's CreateDevice method. This allows us to then hook the IDirect3DDevice9
 * methods we need
 */
HRESULT WINAPI D3DCreateDevice_hook(IDirect3D9 * Direct3D_Object, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS * pPresentationParameters, IDirect3DDevice9 ** ppReturnedDeviceInterface) {
  // Append the almighty D3DCREATE_MULTITHREADED flag...
  const HRESULT result{ D3DCreateDevice_orig(Direct3D_Object, Adapter, DeviceType, hFocusWindow, BehaviorFlags | D3DCREATE_MULTITHREADED, pPresentationParameters, ppReturnedDeviceInterface) };
  if (result != D3D_OK) {
    MessageBoxA(NULL, "Unable to create original device", "Unable to create original device", MB_ICONEXCLAMATION);
    return result;
  }

  // Now we've intercepted the program's call to CreateDevice and we have the IDirect3DDevice9 that it uses
  // We can get it's vtable and patch in our own detours
  // Reset the CreateDevice hook since it's no longer needed
  // Unprotect the vtable for writing
  DWORD protectFlag{ 0 };
  const BOOL unlock{ VirtualProtect(&IDirect3D9_vtable[CREATEDEVICE_VTI], sizeof(DWORD), PAGE_READWRITE, &protectFlag) };
  if (!unlock) {
    MessageBoxA(NULL, "Unable to unlock to create device hook", "Unable to unlock to create device hook", MB_ICONEXCLAMATION);
    return D3DERR_INVALIDCALL;
  }

  *(DWORD *)&IDirect3D9_vtable[CREATEDEVICE_VTI] = (DWORD)D3DCreateDevice_orig;

  const BOOL lock{ VirtualProtect(&IDirect3D9_vtable[CREATEDEVICE_VTI], sizeof(DWORD), protectFlag, &protectFlag) };
  if (!lock) {
    MessageBoxA(NULL, "Unable to lock to create device hook", "Unable to lock to create device hook", MB_ICONEXCLAMATION);
    return D3DERR_INVALIDCALL;
  }

  // Load the new vtable
  IDirect3D9_vtable = (DWORD *)*(DWORD *)*ppReturnedDeviceInterface;
  
  // Store pointers to the original functions that we want to hook
  *(PDWORD)&D3DEndScene_orig = (DWORD)IDirect3D9_vtable[ENDSCENE_VTI];
  
  if (!CreateThread(NULL, 0, VTablePatchThread, NULL, NULL, NULL)) {
    MessageBoxA(NULL, "Unable to create thread to patch vtable", "Unable to create thread to patch vtable", MB_ICONEXCLAMATION);
    return D3DERR_INVALIDCALL;
  }

  return result;
}

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
    *(DWORD *)&IDirect3D9_vtable[ENDSCENE_VTI] = (DWORD)D3DEndScene_hook;
  }
}

HRESULT WINAPI D3DEndScene_hook(IDirect3DDevice9 * device) {
  const HRESULT result{ D3DEndScene_orig(device) };
  return result;
}