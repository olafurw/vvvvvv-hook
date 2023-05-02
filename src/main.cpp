//////////////////////////////////////////////////////////////////////////////
//
//  Detours Test Program (simple.cpp of simple.dll)
//
//  Microsoft Research Detours Package
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  This DLL will detour the Windows SleepEx API so that TimedSleep function
//  gets called instead.  TimedSleepEx records the before and after times, and
//  calls the real SleepEx API through the TrueSleepEx function pointer.
//
#include <windows.h>
#include "detours/detours.h"
#include <stdio.h>
#include <format>
#include <d3d9.h>
#include "directxsdk/d3dx9.h"

// todo
// get the original device, hook on those functions
// https://github.com/NoShotz/ElDorito/blob/a6d2d681e999d2d21f3f8bb3bd272f0771f2d1fe/ElDorito/Source/Patches/DirectXHook.cpp#L3
// https://github.com/AlvyPiper/Source-VAC-Banner/blob/be8e28e530e039ca4024621560404e65e6a535b6/D3D9Patch/EndScene_Detour/main.cpp#L4

static HWND window;

namespace d3d {
    BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM) {
      DWORD wndProcId;
      GetWindowThreadProcessId(handle, &wndProcId);

      if (GetCurrentProcessId() != wndProcId) {
        return TRUE;
      }

      window = handle;
      return FALSE;
    }

    HWND GetProcessWindow() {
      window = NULL;
      EnumWindows(EnumWindowsCallback, NULL);
      return window;
    }

    bool GetD3D9Device(void** pTable, size_t Size) {
      if (!pTable) {
        return false;
      }

      IDirect3D9 * pD3D = Direct3DCreate9(D3D_SDK_VERSION);
      if (!pD3D) {
        MessageBox(NULL, "err1", "err1", MB_ICONWARNING);
        return false;
      }

      IDirect3DDevice9 * pDummyDevice = NULL;

      // options to create dummy device
      D3DPRESENT_PARAMETERS d3dpp = {};
      d3dpp.Windowed = true;
      d3dpp.BackBufferCount = 1;
      d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
      d3dpp.BackBufferWidth = 1;
      d3dpp.BackBufferHeight = 1;
      d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
      d3dpp.hDeviceWindow = GetProcessWindow();
      if (d3dpp.hDeviceWindow == NULL) {
        MessageBox(NULL, "err2", "err2", MB_ICONWARNING);
        return false;
      }

      HRESULT dummyDeviceCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_FPU_PRESERVE | D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

      if (dummyDeviceCreated != S_OK) {
        MessageBox(NULL, "err3", "err3", MB_ICONWARNING);
        pD3D->Release();
        return false;
      }

      memcpy(pTable, *reinterpret_cast<void ***>(pDummyDevice), Size);

      pDummyDevice->Release();
      pD3D->Release();
      return true;
    }
}

typedef HRESULT(__thiscall * BeginSceneFn)(LPDIRECT3DDEVICE9 thisdevice);
BeginSceneFn org_beginscene;
HRESULT __stdcall hk_beginscene(LPDIRECT3DDEVICE9 thisdevice) {
    return org_beginscene(thisdevice);
}

typedef HRESULT(__thiscall * EndSceneFn)(LPDIRECT3DDEVICE9 thisdevice);
EndSceneFn org_endscene;
HRESULT __stdcall hk_endscene(LPDIRECT3DDEVICE9 thisdevice) {
    return org_endscene(thisdevice);
}

void initDX9() {
  void* d3dDevice[119];
  if (d3d::GetD3D9Device(d3dDevice, sizeof(d3dDevice))) {
    DetourRestoreAfterWith();

    auto transactionBegin = DetourTransactionBegin();
    std::string transactionBeginError = std::format("{}", transactionBegin);
    MessageBox(NULL, transactionBeginError.c_str(), transactionBeginError.c_str(), MB_ICONWARNING);

    org_beginscene = (BeginSceneFn)((DWORD *)d3dDevice[41]); // 41 is BeginScene
    org_endscene = (EndSceneFn)((DWORD *)d3dDevice[42]); //42 is EndScene

    auto attachBegin = DetourAttach(&(void *&)org_beginscene, hk_beginscene);
    std::string attachBeginError = std::format("{}", attachBegin);
    MessageBox(NULL, attachBeginError.c_str(), attachBeginError.c_str(), MB_ICONWARNING);

    auto attachEnd = DetourAttach(&(void *&)org_endscene, hk_endscene);
    std::string attachEndError = std::format("{}", attachEnd);
    MessageBox(NULL, attachEndError.c_str(), attachEndError.c_str(), MB_ICONWARNING);

    auto transactionEnd = DetourTransactionCommit();
    std::string transactionEndError = std::format("{}", transactionEnd);
    MessageBox(NULL, transactionEndError.c_str(), transactionEndError.c_str(), MB_ICONWARNING);
  }
}

DWORD WINAPI HookingThread(LPARAM) {
  Sleep(2000);
  initDX9();
  return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
  (void)hinst;
  (void)reserved;

  if (DetourIsHelperProcess()) {
    return TRUE;
  }

  switch (dwReason) {
    case DLL_PROCESS_ATTACH: {
      DisableThreadLibraryCalls((HMODULE)hinst);
      CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HookingThread, hinst, 0, nullptr));
    }
    case DLL_PROCESS_DETACH:
    default:
      break;
  }

  return TRUE;
}

//
///////////////////////////////////////////////////////////////// End of File.
