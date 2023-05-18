#include "end_scene_override.h"

#include <fstream>
#include <string>
#include <vector>

#include "keyboard_helpers.h"
#include "end_scene_imgui.h"

static std::vector<std::string> input;

static bool overrideInit{ false };

std::string lastInput;
uint32_t delay{ 0 };

void EndSceneOverride(IDirect3DDevice9 * device, const uint32_t frame, const std::chrono::microseconds delta) {
    EndSceneImgui(device, frame, delta, delay);

    if (!overrideInit) {
        std::ifstream inputFile("lr.txt");
        std::ofstream outputFile("output.txt", std::ios::out);

        std::string line;
        while (std::getline(inputFile, line)) {
            input.push_back(line);
        }
    }

    if (frame < input.size()) {
        lastInput = input[frame];
        if (!lastInput.empty() && lastInput[0] != '-') {
            for (const char c : lastInput) {
                if (c == 'S') {
                    PressSpace();
                } else if (c == 's') {
                    ReleaseSpace();
                } else if (c == 'L') {
                    PressLeft();
                } else if (c == 'l') {
                    ReleaseLeft();
                } else if (c == 'R') {
                    PressRight();
                } else if (c == 'r') {
                    ReleaseRight();
                }
            }
        }
    }

    Sleep(delay);
}