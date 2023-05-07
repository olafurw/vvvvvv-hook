#pragma once

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

typedef HRESULT(WINAPI * CreateDevice_t)(IDirect3D9 * Direct3D_Object, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS * pPresentationParameters, IDirect3DDevice9 ** ppReturnedDeviceInterface);
extern CreateDevice_t D3DCreateDevice_orig;

HRESULT WINAPI D3DCreateDeviceHook(IDirect3D9 * Direct3D_Object, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS * pPresentationParameters, IDirect3DDevice9 ** ppReturnedDeviceInterface);
