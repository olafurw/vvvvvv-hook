#include "end_scene.h"

EndScene_t D3DEndScene_orig;

static unsigned int frameCounter{ 0 };

HRESULT WINAPI D3DEndSceneHook(IDirect3DDevice9 * device) {
  // if (!logFile.is_open()) {
  //   logFile.open("log.txt", std::ios::out | std::ios::app);
  // }

  const HRESULT result{ D3DEndScene_orig(device) };
  frameCounter++;

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

  return result;
}