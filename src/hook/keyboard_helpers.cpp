#include "keyboard_helpers.h"

#include <cstdint>

static constexpr uint16_t btn_right{ 0x20 };
static constexpr uint16_t btn_left{ 0x1e };
static constexpr uint16_t btn_space{ 0x39 };

void PressButton(WORD button) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = button;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE;
    SendInput(1, &ip, sizeof(INPUT));
}

void ReleaseButton(WORD button) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.wScan = button;
    ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
}

void PressSpace() {
    PressButton(btn_space);
}

void ReleaseSpace() {
    ReleaseButton(btn_space);
}

void PressLeft() {
    PressButton(btn_left);
}

void ReleaseLeft() {
    ReleaseButton(btn_left);
}

void PressRight() {
    PressButton(btn_right);
}

void ReleaseRight() {
    ReleaseButton(btn_right);
}