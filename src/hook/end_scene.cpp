#include "end_scene.h"

#include <chrono>
#include <cmath>
#include <format>
#include <fstream>
#include <vector>

#include "end_scene_override.h"

EndScene_t D3DEndScene_orig;

static uint32_t frame{ 0 };
static auto frame_time{ std::chrono::high_resolution_clock::now() };
static std::chrono::microseconds delta{ std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - frame_time) };

HRESULT WINAPI D3DEndSceneHook(IDirect3DDevice9 * device) {
    EndSceneOverride(device, frame, delta);
    const HRESULT result{ D3DEndScene_orig(device) };
    frame++;

    const auto time_now{ std::chrono::high_resolution_clock::now() };
    delta = std::chrono::duration_cast<std::chrono::microseconds>(time_now - frame_time);
    frame_time = time_now;

    return result;
}