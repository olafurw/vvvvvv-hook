#include "hook_create_device.h"

#include <d3d9.h>
#include <d3dx9.h>

#include "create_device.h"
#include "end_scene.h"
#include "virtual_protect.h"
#include "virtual_table.h"

HRESULT WINAPI HookCreateDevice() {
    IDirect3D9 * device{ Direct3DCreate9(D3D_SDK_VERSION) };
    if (!device) {
        return D3DERR_INVALIDCALL;
    }

    IDirect3D9_vtable = (DWORD *)*(DWORD *)device;
    device->Release();

    Protect protect(&IDirect3D9_vtable[CREATEDEVICE_VTI], sizeof(DWORD), PAGE_READWRITE, [&]() {
        *(DWORD *)&D3DCreateDevice_orig = IDirect3D9_vtable[CREATEDEVICE_VTI];
        *(DWORD *)&IDirect3D9_vtable[CREATEDEVICE_VTI] = (DWORD)D3DCreateDeviceHook;
    });

    return D3D_OK;
}
