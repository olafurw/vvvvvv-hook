#include "end_scene.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

#include <chrono>
#include <cmath>
#include <vector>
#include <format>
#include <fstream>

std::vector<std::string> input;
std::string lastInput;
uint32_t delay{ 0 };

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

    std::ifstream inputFile("lr.txt");

    std::string line;
    while (std::getline(inputFile, line)) {
      input.push_back(line);
    }
  }

  frameCounter++;

  ImGui_ImplDX9_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  bool open{ true };
  ImGui::SetNextWindowSize(ImVec2(110, 80));
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::Begin("Debug", &open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
  ImGui::SetScrollY(0);
  ImGui::SetScrollX(0);

  ImGui::Text("f: %d", frameCounter);
  ImGui::Text("i: %s", lastInput.c_str());

  const double d{ delta.count() / 1000.0f };
  ImGui::Text("ms: %.2f", d);

  if (ImGui::Button("||")) {
    delay = 3000;
  }

  ImGui::SameLine();

  if (ImGui::Button(">")) {
    delay = 1000;
  }

  ImGui::SameLine();

  if (ImGui::Button(">>")) {
    delay = 100;
  }

  ImGui::SameLine();

  if (ImGui::Button("n")) {
    delay = 0;
  }

  ImGui::End();
  ImGui::EndFrame();
  ImGui::Render();
  ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

  const HRESULT result{ D3DEndScene_orig(device) };

  if (frameCounter < input.size()) {
    lastInput = input[frameCounter];
    if (!lastInput.empty() && lastInput[0] != '-') {
      for (const char c : lastInput) {
        if (c == 'S') {
          PressButton(btn_space);
        } else if (c == 's') {
          ReleaseButton(btn_space);
        } else if (c == 'L') {
          PressButton(btn_left);
        } else if (c == 'l') {
          ReleaseButton(btn_left);
        } else if (c == 'R') {
          PressButton(btn_right);
        } else if (c == 'r') {
          ReleaseButton(btn_right);
        }
      }
    }
  }

  const auto time_now{ std::chrono::high_resolution_clock::now() };
  delta = std::chrono::duration_cast<std::chrono::microseconds>(time_now - frame_time);
  frame_time = time_now;

  Sleep(delay);

  return result;
}