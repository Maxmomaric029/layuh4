#include "ui_framework.h"

namespace ui {
    InputState input = { 0 };
    bool menu_open = true;

    static bool was_left_down = false;
    static bool was_right_down = false;

    void update_input(HWND hwnd) {
        POINT p;
        if (GetCursorPos(&p)) {
            ScreenToClient(hwnd, &p);
            input.mouse_x = (float)p.x;
            input.mouse_y = (float)p.y;
        }

        input.left_down = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        input.right_down = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

        input.left_clicked = input.left_down && !was_left_down;
        input.right_clicked = input.right_down && !was_right_down;

        was_left_down = input.left_down;
        was_right_down = input.right_down;
    }

    static std::string active_window = "";
    static float drag_offset_x = 0;
    static float drag_offset_y = 0;

    bool begin_window(const std::string& title, float& x, float& y, float w, float h) {
        if (!menu_open) return false;

        // Header logic
        float header_height = 30.0f;
        
        bool is_hovering_header = (input.mouse_x >= x && input.mouse_x <= x + w &&
                                   input.mouse_y >= y && input.mouse_y <= y + header_height);

        if (is_hovering_header && input.left_clicked) {
            active_window = title;
            drag_offset_x = input.mouse_x - x;
            drag_offset_y = input.mouse_y - y;
        }

        if (input.left_down && active_window == title) {
            x = input.mouse_x - drag_offset_x;
            y = input.mouse_y - drag_offset_y;
        } else if (!input.left_down && active_window == title) {
            active_window = "";
        }

        // Draw shadow (fake)
        render::draw_filled_rounded_rect(x + 5, y + 5, w, h, 10.0f, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.5f));
        
        // Draw background
        render::draw_filled_rounded_rect(x, y, w, h, 10.0f, ColorBackground);
        
        // Draw header
        render::draw_filled_rounded_rect(x, y, w, header_height, 10.0f, ColorPrimary);
        
        // Header Text
        render::draw_text(title, x + 10.0f, y + 5.0f, 16.0f, D2D1::ColorF(D2D1::ColorF::White));

        return true;
    }

    void end_window() {
        // Nothing for now
    }

    bool button(const std::string& label_str, float x, float y, float w, float h) {
        bool is_hovered = (input.mouse_x >= x && input.mouse_x <= x + w &&
                           input.mouse_y >= y && input.mouse_y <= y + h);

        D2D1::ColorF bg = is_hovered ? ColorHover : D2D1::ColorF(0x1F222E, 1.0f);
        if (is_hovered && input.left_down) {
            bg = ColorPrimary;
        }

        render::draw_filled_rounded_rect(x, y, w, h, 5.0f, bg);
        
        // Centered text roughly
        D2D1_SIZE_F tsize = render::get_text_size(label_str, 14.0f);
        render::draw_text(label_str, x + (w - tsize.width) / 2.0f, y + (h - tsize.height) / 2.0f, 14.0f, ColorText);

        return is_hovered && input.left_clicked;
    }

    bool checkbox(const std::string& label_str, bool& value, float x, float y) {
        float size = 18.0f;
        bool is_hovered = (input.mouse_x >= x && input.mouse_x <= x + size + 10.0f + 100.0f &&
                           input.mouse_y >= y && input.mouse_y <= y + size);

        if (is_hovered && input.left_clicked) {
            value = !value;
        }

        render::draw_rounded_rect(x, y, size, size, 4.0f, ColorPrimary, 1.5f);
        if (value) {
            render::draw_filled_rounded_rect(x + 3, y + 3, size - 6, size - 6, 2.0f, ColorPrimary);
        }

        render::draw_text(label_str, x + size + 8.0f, y, 14.0f, ColorText);

        return is_hovered && input.left_clicked;
    }

    void label(const std::string& text, float x, float y) {
        render::draw_text(text, x, y, 14.0f, ColorText);
    }
}
