#include "validate_dll.h"

#include <windows.h>
#include "detours/detours.h"

static BOOL CALLBACK ExportCallback(_In_opt_ PVOID pContext,
                                    _In_ ULONG nOrdinal,
                                    _In_opt_ LPCSTR pszSymbol,
                                    _In_opt_ PVOID pbTarget) {
  (void)pbTarget;
  (void)pszSymbol;
  if (!pContext) {
    return FALSE;
  }

  ExportContext * pec{ (ExportContext *)pContext };

  if (nOrdinal == 1) {
    pec->fHasOrdinal1 = TRUE;
  }
  pec->nExports++;

  return TRUE;
}

std::optional<std::string> isDllValid(const std::string& path) {
  constexpr uint32_t bufferSize{ 4096 };
  char dllPath[bufferSize]{};
  TCHAR ** lppPart{ nullptr };

  if (!GetFullPathNameA(path.c_str(), bufferSize, dllPath, lppPart)) {
    return {};
  }

  HMODULE hDll{ LoadLibraryExA(dllPath, NULL, DONT_RESOLVE_DLL_REFERENCES) };
  if (hDll == nullptr) {
    return {};
  }

  ExportContext ec{};
  ec.fHasOrdinal1 = FALSE;
  ec.nExports = 0;
  DetourEnumerateExports(hDll, &ec, ExportCallback);
  FreeLibrary(hDll);

  if (!ec.fHasOrdinal1) {
    return {};
  }

  return dllPath;
}