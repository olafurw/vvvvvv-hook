#include "end_scene.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

EndScene_t D3DEndScene_orig;

static unsigned int frameCounter{ 0 };

bool imguiInit{ false };
bool showMenu{ true };
struct CurrentProcess {
  HMODULE hModule;
  HWND TargetHwnd;
  WNDPROC TargetWndProc;
} Process;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
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
  ImGui::SetNextWindowSize(ImVec2(100, 60));
  ImGui::SetNextWindowPos(ImVec2(10, 10));
  ImGui::Begin("Debug", &open);

  ImGui::Text("frame: %d", frameCounter);

  ImGui::End();
  ImGui::EndFrame();
  ImGui::Render();
  ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

  const HRESULT result{ D3DEndScene_orig(device) };

  if (frameCounter == 1) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = 0x39; // space
    ip.ki.dwFlags = KEYEVENTF_SCANCODE;
    SendInput(1, &ip, sizeof(INPUT));
  }
  if (frameCounter == 2) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = 0x39;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
  }
  if (frameCounter == 3) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = 0x39;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE;
    SendInput(1, &ip, sizeof(INPUT));
  }
  if (frameCounter == 4) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = 0x39;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
  }

  // uh oh...
  if (frameCounter == 148) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = 0x39;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE;
    SendInput(1, &ip, sizeof(INPUT));
  }
  if (frameCounter == 149) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = 0x39;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
  }

  // is everything ok?
  if (frameCounter == 150) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = 0x39;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE;
    SendInput(1, &ip, sizeof(INPUT));
  }
  if (frameCounter == 151) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = 0x39;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
  }

  // no we hit some sort of interferance
  if (frameCounter == 154) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = 0x39;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE;
    SendInput(1, &ip, sizeof(INPUT));
  }
  if (frameCounter == 155) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = 0x39;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
  }

  // going to crash
  if (frameCounter == 189) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = 0x39;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE;
    SendInput(1, &ip, sizeof(INPUT));
  }
  if (frameCounter == 190) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = 0x39;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
  }

  // that was scary
  if (frameCounter == 460) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = 0x39;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE;
    SendInput(1, &ip, sizeof(INPUT));
  }
  if (frameCounter == 461) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = 0x39;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
  }

  // at least we all escaped
  if (frameCounter == 462) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = 0x39;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE;
    SendInput(1, &ip, sizeof(INPUT));
  }
  if (frameCounter == 463) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = 0x39;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
  }

  // guys?
  if (frameCounter == 560) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = 0x39;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE;
    SendInput(1, &ip, sizeof(INPUT));
  }
  if (frameCounter == 561) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = 0x39;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
  }

  return result;
}