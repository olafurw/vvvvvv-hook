#include <windows.h>
#include "detours/detours.h"
#include <stdio.h>
#include <cstdint>
#include <array>

struct ExportContext {
  BOOL fHasOrdinal1;
  ULONG nExports;
};

static BOOL CALLBACK ExportCallback(_In_opt_ PVOID pContext,
                                    _In_ ULONG nOrdinal,
                                    _In_opt_ LPCSTR pszSymbol,
                                    _In_opt_ PVOID pbTarget) {
  (void)pContext;
  (void)pbTarget;
  (void)pszSymbol;

  ExportContext * pec = (ExportContext *)pContext;

  if (nOrdinal == 1) {
    pec->fHasOrdinal1 = TRUE;
  }
  pec->nExports++;

  return TRUE;
}

int main() {
  constexpr uint32_t bufferSize{ 4096 };
  char dllPath[bufferSize]{};
  TCHAR** lppPart{ nullptr };
  if (!GetFullPathNameA("hook.dll", bufferSize, dllPath, lppPart)) {
    return 9002;
  }

  HMODULE hDll = LoadLibraryExA(dllPath, NULL, DONT_RESOLVE_DLL_REFERENCES);
  if (hDll == nullptr) {
    return 9003;
  }

  ExportContext ec;
  ec.fHasOrdinal1 = FALSE;
  ec.nExports = 0;
  DetourEnumerateExports(hDll, &ec, ExportCallback);
  FreeLibrary(hDll);

  if (!ec.fHasOrdinal1) {
    return 9004;
  }

  STARTUPINFOA si;
  PROCESS_INFORMATION pi;
  CHAR szCommand[2048];
  CHAR szExe[1024];
  CHAR szFullExe[1024] = "C:\\Users\\olafu\\source\\repos\\vvvvvv-hook\\build\\x86-windows\\bin\\Debug\\VVVVVV\\VVVVVV.exe\0";
  PCHAR pszFileExe = NULL;

  ZeroMemory(&si, sizeof(si));
  ZeroMemory(&pi, sizeof(pi));
  si.cb = sizeof(si);

  szCommand[0] = L'\0';

  DWORD dwFlags = CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED;
  LPCSTR detourDlls[1] = { dllPath };

  SetLastError(0);
  SearchPathA(NULL, szExe, ".exe", ARRAYSIZE(szFullExe), szFullExe, &pszFileExe);
  if (!DetourCreateProcessWithDllsA(szFullExe[0] ? szFullExe : NULL, szCommand, NULL, NULL, TRUE, dwFlags, NULL, NULL, &si, &pi, 1, detourDlls, NULL)) {
    ExitProcess(9009);
  }

  ResumeThread(pi.hThread);
  WaitForSingleObject(pi.hProcess, INFINITE);

  DWORD dwResult = 0;
  if (!GetExitCodeProcess(pi.hProcess, &dwResult)) {
    printf("withdll.exe: GetExitCodeProcess failed: %ld\n", GetLastError());
    return 9010;
  }

  return dwResult;
}
//
///////////////////////////////////////////////////////////////// End of File.
