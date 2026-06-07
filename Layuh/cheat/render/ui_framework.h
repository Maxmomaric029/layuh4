#pragma once
#include "renderer.h"
#include <windows.h>
#include <string>
#include <vector>
#include <functional>

namespace ui {
    struct InputState {
        float mouse_x, mouse_y;
        bool left_down;
        bool right_down;
        bool left_clicked;
        bool right_clicked;
        int scroll;
    };

    extern InputState input;
    extern bool menu_open;

    void update_input(HWND hwnd);

    // Helpers de diseño (CSS style)
    const D2D1::ColorF ColorBackground = D2D1::ColorF(0x0F1117, 0.95f);
    const D2D1::ColorF ColorPrimary = D2D1::ColorF(0x5A189A, 1.0f);
    const D2D1::ColorF ColorText = D2D1::ColorF(0xC7C7C7, 1.0f);
    const D2D1::ColorF ColorHover = D2D1::ColorF(0x2A2D3A, 0.8f);

    // Dibuja una ventana arrastrable
    bool begin_window(const std::string& title, float& x, float& y, float w, float h);
    void end_window();

    // Elementos
    bool button(const std::string& label, float x, float y, float w, float h);
    bool checkbox(const std::string& label, bool& value, float x, float y);
    void label(const std::string& text, float x, float y);
}
