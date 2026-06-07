#pragma once
#include "renderer.h"
#include <windows.h>
#include <string>
#include <vector>
#include <functional>
#include <map>

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

    // Helpers de diseño (CSS style modern dark theme)
    const D2D1::ColorF ColorBackground = D2D1::ColorF(0x08080A, 0.98f);
    const D2D1::ColorF ColorPanel = D2D1::ColorF(0x121216, 1.0f);
    const D2D1::ColorF ColorPrimary = D2D1::ColorF(0x6366F1, 1.0f); // Indigo 500
    const D2D1::ColorF ColorText = D2D1::ColorF(0xF3F4F6, 1.0f); // Gray 100
    const D2D1::ColorF ColorTextMuted = D2D1::ColorF(0x9CA3AF, 1.0f); // Gray 400
    const D2D1::ColorF ColorBorder = D2D1::ColorF(0x1F2937, 1.0f); // Gray 800
    const D2D1::ColorF ColorHover = D2D1::ColorF(0x2A2D3A, 0.8f);

    // System state
    void set_next_window_size(float w, float h);
    bool begin_window(const std::string& title, float& x, float& y);
    void end_window();

    void begin_group_box(const std::string& title, float width, float height);
    void end_group_box();

    void same_line(float offset_x = 0.0f);
    void set_cursor(float x, float y);
    void set_cursor_x(float x);
    void set_cursor_y(float y);
    
    // Tab system
    bool tab(const std::string& label, bool active, float w, float h);

    // Elementos interactivos
    bool button(const std::string& label, float w, float h);
    bool toggle(const std::string& label, bool& value);
    bool slider_float(const std::string& label, float& value, float min_val, float max_val);
    bool slider_int(const std::string& label, int& value, int min_val, int max_val);
    void label(const std::string& text);
}
