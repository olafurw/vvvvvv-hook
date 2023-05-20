#include "end_scene_imgui.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

#include "keyboard_helpers.h"

static bool showMenu{ true };
static bool imguiInit{ false };

static bool guiSpace{ false };
static bool guiLeft{ false };
static bool guiRight{ false };

struct CurrentProcess {
    HMODULE hModule;
    HWND TargetHwnd;
    WNDPROC TargetWndProc;
} Process;

LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (showMenu && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
        return true;
    }
    return CallWindowProc(Process.TargetWndProc, hWnd, msg, wParam, lParam);
}

void EndSceneImguiInit(IDirect3DDevice9 * device) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    D3DDEVICE_CREATION_PARAMETERS params;
    device->GetCreationParameters(&params);
    Process.TargetHwnd = params.hFocusWindow;

    if (Process.TargetHwnd != NULL) {
        Process.TargetWndProc = (WNDPROC)SetWindowLongPtr(Process.TargetHwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
        ImGui_ImplWin32_Init(Process.TargetHwnd);
        ImGui_ImplDX9_Init(device);
        ImGui::GetIO().ImeWindowHandle = Process.TargetHwnd;
        imguiInit = true;
    }
}

void EndSceneImgui(IDirect3DDevice9 * device, const uint32_t frame, const std::chrono::microseconds delta, uint32_t & delay) {
    if (!imguiInit) {
        EndSceneImguiInit(device);
    }

    if (guiSpace) {
        ReleaseSpace();
        guiSpace = false;
    }

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    bool open{ true };
    ImGui::SetNextWindowSize(ImVec2(110, 120));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Debug", &open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    ImGui::SetScrollY(0);
    ImGui::SetScrollX(0);

    ImGui::Text("f: %d", frame);

    const double d{ delta.count() / 1000.0f };
    ImGui::Text("ms: %.2f", d);

    if (ImGui::Button("||")) {
        delay = 3000;
    }

    ImGui::SameLine();

    if (ImGui::Button(">")) {
        delay = 1000;
    }

    ImGui::SameLine();

    if (ImGui::Button(">>")) {
        delay = 100;
    }

    ImGui::SameLine();

    if (ImGui::Button("n")) {
        delay = 0;
    }

    if (ImGui::Button("l")) {
        if (!guiLeft) {
            PressLeft();
            guiLeft = true;
        } else {
            ReleaseLeft();
            guiLeft = false;
        }
    }

    ImGui::SameLine();

    if (ImGui::Button(" ")) {
        PressSpace();
        guiSpace = true;
    }

    ImGui::SameLine();

    if (ImGui::Button("r")) {
        if (!guiRight) {
            PressRight();
            guiRight = true;
        } else {
            ReleaseRight();
            guiRight = false;
        }
    }

    ImGui::End();
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}