#pragma once

#include <windows.h>

struct Protect
{
  template<typename F>
  Protect(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, F&& work)
    : locked{ FALSE },
      flag{ 0 },
      size{ dwSize },
      address{ lpAddress } {
    locked = VirtualProtect(address, size, flNewProtect, &flag);
    if (!locked) {
      return;
    }

    work();
  }

  ~Protect() {
    if (!locked) {
      return;
    }

    VirtualProtect(address, size, flag, &flag);
  }

  BOOL locked;
  DWORD flag;
  SIZE_T size;
  LPVOID address;
};
