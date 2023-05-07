#include <windows.h>
#include "detours/detours.h"
#include <stdio.h>
#include <cstdint>
#include <optional>
#include <string>

#include "validate_dll.h"

struct HookProcess {
  STARTUPINFOA si;
  PROCESS_INFORMATION pi;
  char szCommand[2048]{};

  std::string process;
  std::string dll;

  HookProcess(const char * process, const char * dll);

  bool init();
  void start();
};

HookProcess::HookProcess(const char * process, const char * dll)
  : process{ process }, dll{ dll } {
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);

  ZeroMemory(&pi, sizeof(pi));

  szCommand[0] = L'\0';
}

bool HookProcess::init() {
  const auto dllPathOpt{ isDllValid(dll) };
  if (!dllPathOpt) {
    return false;
  }

  dll = dllPathOpt.value();
  return true;
}

void HookProcess::start() {
  LPCSTR detourDlls[1]{ dll.c_str() };
  const DWORD dwFlags{ CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED };
  if (!DetourCreateProcessWithDllsA(process.c_str(), szCommand, NULL, NULL, TRUE, dwFlags, NULL, NULL, &si, &pi, 1, detourDlls, NULL)) {
    return;
  }

  ResumeThread(pi.hThread);
  WaitForSingleObject(pi.hProcess, INFINITE);

  DWORD dwResult = 0;
  if (!GetExitCodeProcess(pi.hProcess, &dwResult)) {
    printf("withdll.exe: GetExitCodeProcess failed: %ld\n", GetLastError());
    return;
  }
}

int main() {
  const auto dll{ "hook.dll" };
  const auto process{ "C:\\Users\\olafu\\source\\repos\\vvvvvv-hook\\build\\x86-windows\\bin\\Debug\\VVVVVV\\VVVVVV.exe" };

  const auto settingsOrig{ "C:\\Users\\olafu\\Documents\\VVVVVV\\saves\\settings.old" };
  const auto settings{ "C:\\Users\\olafu\\Documents\\VVVVVV\\saves\\settings.vvv" };
  const auto unlockOrig{ "C:\\Users\\olafu\\Documents\\VVVVVV\\saves\\unlock.old" };
  const auto unlock{ "C:\\Users\\olafu\\Documents\\VVVVVV\\saves\\unlock.vvv" };

  CopyFile(settingsOrig, settings, FALSE);
  CopyFile(unlockOrig, unlock, FALSE);

  HookProcess hook(process, dll);
  if (!hook.init()) {
    return 1;
  }

  hook.start();
  
  return 0;
}
