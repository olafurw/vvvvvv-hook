#include <cstdint>
#include <optional>
#include <stdio.h>
#include <string>
#include <format>
#include <windows.h>

#include "detours/detours.h"
#include "validate_dll.h"

struct HookProcess {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    char szCommand[2048]{};

    std::string process;
    std::string dll;

    HookProcess(const std::string & process, const char * dll);

    bool init();
    void start();
};

HookProcess::HookProcess(const std::string & process, const char * dll)
  : process{ process },
    dll{ dll } {
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

    DWORD dwResult{ 0 };
    if (!GetExitCodeProcess(pi.hProcess, &dwResult)) {
        printf("withdll.exe: GetExitCodeProcess failed: %ld\n", GetLastError());
        return;
    }
}

int main(int argc, char ** argv) {
    const auto projectFolder{ "C:\\Users\\olafu\\source\\repos\\vvvvvv-hook\\" };
    const auto binFolder{ "C:\\Users\\olafu\\source\\repos\\vvvvvv-hook\\build\\x86-windows\\bin\\Debug\\" };
    const auto saveFolder{ "C:\\Users\\olafu\\Documents\\VVVVVV\\saves\\" };

    const auto dll{ "hook.dll" };
    const auto process{ std::format("{}{}", binFolder, "VVVVVV\\VVVVVV.exe") };
    const auto tasDataDestination{ std::format("{}{}", binFolder, "tas_data.txt") };

    if (argc > 1) {
        const std::string arg{ argv[1] };
        if (arg == "simple") {
            const auto settingsOrig{ std::format("{}{}", saveFolder, "settings_simple.vvv") };
            const auto settings{ std::format("{}{}", saveFolder, "settings.vvv") };
            const auto unlockOrig{ std::format("{}{}", saveFolder, "unlock_simple.vvv") };
            const auto unlock{ std::format("{}{}", saveFolder, "unlock.vvv") };
            const auto tSaveOrig{ std::format("{}{}", saveFolder, "tsave_simple.vvv") };
            const auto tSave{ std::format("{}{}", saveFolder, "tsave.vvv") };
            const auto quickSave{ std::format("{}{}", saveFolder, "qsave.vvv") };

            CopyFile(settingsOrig.c_str(), settings.c_str(), FALSE);
            CopyFile(unlockOrig.c_str(), unlock.c_str(), FALSE);
            CopyFile(tSaveOrig.c_str(), tSave.c_str(), FALSE);
            DeleteFile(quickSave.c_str());

            const auto tasData{ std::format("{}{}", projectFolder, "simple.txt") };
            CopyFile(tasData.c_str(), tasDataDestination.c_str(), FALSE);
        } else if (arg == "trinket") {
            const auto settingsOrig{ std::format("{}{}", saveFolder, "settings_simple.vvv") };
            const auto settings{ std::format("{}{}", saveFolder, "settings.vvv") };
            const auto unlockOrig{ std::format("{}{}", saveFolder, "unlock_simple.vvv") };
            const auto unlock{ std::format("{}{}", saveFolder, "unlock.vvv") };
            const auto tSaveOrig{ std::format("{}{}", saveFolder, "tsave_simple.vvv") };
            const auto tSave{ std::format("{}{}", saveFolder, "tsave.vvv") };
            const auto quickSave{ std::format("{}{}", saveFolder, "qsave.vvv") };
            const auto quickSaveOrig{ std::format("{}{}", saveFolder, "qsave_trinket.vvv") };

            CopyFile(settingsOrig.c_str(), settings.c_str(), FALSE);
            CopyFile(unlockOrig.c_str(), unlock.c_str(), FALSE);
            CopyFile(tSaveOrig.c_str(), tSave.c_str(), FALSE);
            CopyFile(quickSaveOrig.c_str(), quickSave.c_str(), FALSE);

            const auto tasData{ std::format("{}{}", projectFolder, "trinket.txt") };
            CopyFile(tasData.c_str(), tasDataDestination.c_str(), FALSE);
        }
    }

    HookProcess hook(process.c_str(), dll);
    if (!hook.init()) {
        return 1;
    }

    hook.start();

    return 0;
}
