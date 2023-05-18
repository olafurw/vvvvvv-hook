#include "validate_dll.h"

#include <windows.h>
#include "detours/detours.h"

#include "get_library.h"

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

std::optional<std::string> isDllValid(const std::string & path) {
    GetLibrary getLib(path);
    if (!getLib.loaded) {
        return {};
    }

    ExportContext ec{};
    ec.fHasOrdinal1 = FALSE;
    ec.nExports = 0;
    DetourEnumerateExports(getLib.dll, &ec, ExportCallback);

    if (!ec.fHasOrdinal1) {
        return {};
    }

    return getLib.dllPath;
}