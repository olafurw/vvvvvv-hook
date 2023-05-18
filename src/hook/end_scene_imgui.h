#pragma once

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <chrono>

void EndSceneImgui(IDirect3DDevice9 * device, const uint32_t frame, const std::chrono::microseconds delta, uint32_t& delay);
