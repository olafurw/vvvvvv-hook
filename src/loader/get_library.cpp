#include "get_library.h"

#include <cstdint>

GetLibrary::GetLibrary(const std::string & path) {
    constexpr uint32_t bufferSize{ 4096 };
    dllPath.resize(bufferSize);
    TCHAR ** lppPart{ nullptr };

    if (!GetFullPathNameA(path.c_str(), bufferSize, &dllPath[0], lppPart)) {
        dllPath.clear();
        return;
    }

    dll = LoadLibraryExA(&dllPath[0], NULL, DONT_RESOLVE_DLL_REFERENCES);
    if (dll == nullptr) {
        dllPath.clear();
        return;
    }

    loaded = true;
}

GetLibrary::~GetLibrary() {
    if (dll != nullptr) {
        FreeLibrary(dll);
        dll = nullptr;
    }

    dllPath.clear();
}