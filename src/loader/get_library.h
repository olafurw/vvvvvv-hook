#pragma once

#include <windows.h>
#include <string>

struct GetLibrary {
    GetLibrary(const std::string & path);
    ~GetLibrary();

    bool loaded{ false };
    HMODULE dll{ nullptr };
    std::string dllPath;
};