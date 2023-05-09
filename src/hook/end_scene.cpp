#include "end_scene.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

#include <chrono>
#include <cmath>
#include <vector>
#include <format>

std::vector<std::string> lagFrames;

void PressButton(WORD button) {
  INPUT ip;
  ip.type = INPUT_KEYBOARD;
  ip.ki.time = 0;
  ip.ki.wScan = button; // space
  ip.ki.dwFlags = KEYEVENTF_SCANCODE;
  SendInput(1, &ip, sizeof(INPUT));
}

void ReleaseButton(WORD button) {
  INPUT ip;
  ip.type = INPUT_KEYBOARD;
  ip.ki.time = 0;
  ip.ki.wScan = button;
  ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
  SendInput(1, &ip, sizeof(INPUT));
}

EndScene_t D3DEndScene_orig;

static unsigned int frameCounter{ 0 };
static auto frame_time{ std::chrono::high_resolution_clock::now() };
static std::chrono::microseconds delta{ std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - frame_time) };
static constexpr long long frame_ms{ 33333 };
static long long frame_lag{ 0 };

bool imguiInit{ false };
bool showMenu{ true };
constexpr uint16_t btn_right{ 0x20 };
constexpr uint16_t btn_left{ 0x1e };
constexpr uint16_t btn_space{ 0x39 };

struct CurrentProcess {
  HMODULE hModule;
  HWND TargetHwnd;
  WNDPROC TargetWndProc;
} Process;

LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  if (showMenu && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
    return true;
  }
  return CallWindowProc(Process.TargetWndProc, hWnd, msg, wParam, lParam);
}

HRESULT WINAPI D3DEndSceneHook(IDirect3DDevice9 * device) {
  // if (!logFile.is_open()) {
  //   logFile.open("log.txt", std::ios::out | std::ios::app);
  // }

  if (!imguiInit) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    D3DDEVICE_CREATION_PARAMETERS params;
    device->GetCreationParameters(&params);
    Process.TargetHwnd = params.hFocusWindow;

    if (Process.TargetHwnd != NULL) {
      Process.TargetWndProc = (WNDPROC)SetWindowLongPtr(Process.TargetHwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
      ImGui_ImplWin32_Init(Process.TargetHwnd);
      ImGui_ImplDX9_Init(device);
      ImGui::GetIO().ImeWindowHandle = Process.TargetHwnd;
      imguiInit = true;
    }
  }

  frameCounter++;

  ImGui_ImplDX9_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  bool open{ true };
  ImGui::SetNextWindowSize(ImVec2(120, 200));
  ImGui::SetNextWindowPos(ImVec2(10, 10));
  ImGui::Begin("Debug", &open);

  ImGui::Text("frame: %d", frameCounter);

  const double d{ delta.count() / 1000.0f };
  ImGui::Text("ms: %.3f", d);

  const double fl{ frame_lag / 1000.0f };
  ImGui::Text("drift: %.3f", fl);

  std::vector<const char *> lagFramesC;
  for (const auto & s : lagFrames) {
    lagFramesC.push_back(s.c_str());
  }
  int currentLagFrame = 0;

  ImGui::ListBox("lag frames", &currentLagFrame, lagFramesC.data(), lagFramesC.size());

  ImGui::End();
  ImGui::EndFrame();
  ImGui::Render();
  ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

  const HRESULT result{ D3DEndScene_orig(device) };

  if (frameCounter == 1) {
    PressButton(btn_space);
  } else if (frameCounter == 2) {
    ReleaseButton(btn_space);
  } else if (frameCounter == 3) {
    PressButton(btn_space);
  } else if (frameCounter == 4) {
    ReleaseButton(btn_space);
  } else if (frameCounter == 5) {
    PressButton(btn_space);
  } else if (frameCounter == 6) {
    ReleaseButton(btn_space);
  } else if (frameCounter > 50 && frameCounter < 58) {
    PressButton(btn_right);
  } else if (frameCounter == 58) {
    ReleaseButton(btn_right);
  } else if (frameCounter > 70 && frameCounter < 87) {
    PressButton(btn_left);
  } else if (frameCounter == 87) {
    ReleaseButton(btn_left);
  } else if (frameCounter > 100 && frameCounter < 118) {
    PressButton(btn_right);
  } else if (frameCounter == 118) {
    ReleaseButton(btn_right);
  } else if (frameCounter > 130 && frameCounter < 147) {
    PressButton(btn_left);
  } else if (frameCounter == 147) {
    ReleaseButton(btn_left);
  } else if (frameCounter > 160 && frameCounter < 178) {
    PressButton(btn_right);
  } else if (frameCounter == 178) {
    ReleaseButton(btn_right);
  } else if (frameCounter > 190 && frameCounter < 207) {
    PressButton(btn_left);
  } else if (frameCounter == 207) {
    ReleaseButton(btn_left);
  } else if (frameCounter > 220 && frameCounter < 238) {
    PressButton(btn_right);
  } else if (frameCounter == 238) {
    ReleaseButton(btn_right);
  } else if (frameCounter > 250 && frameCounter < 267) {
    PressButton(btn_left);
  } else if (frameCounter == 267) {
    ReleaseButton(btn_left);
  }

  const auto time_now{ std::chrono::high_resolution_clock::now() };
  delta = std::chrono::duration_cast<std::chrono::microseconds>(time_now - frame_time);
  frame_lag = delta.count() - frame_ms;
  if (frame_lag > 3000) {
    lagFrames.push_back(std::to_string(frame_lag));
  }
  frame_time = time_now;

  return result;
}