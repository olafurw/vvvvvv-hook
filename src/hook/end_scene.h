#pragma once

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

typedef HRESULT(WINAPI * EndScene_t)(IDirect3DDevice9 * surface);
extern EndScene_t D3DEndScene_orig;

HRESULT WINAPI D3DEndSceneHook(IDirect3DDevice9 * device);
