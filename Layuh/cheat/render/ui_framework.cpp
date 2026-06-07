#include "ui_framework.h"
#include <cmath>
#include <algorithm>

namespace ui {
    InputState input = { 0 };
    bool menu_open = true;

    static bool was_left_down = false;
    static bool was_right_down = false;

    // Layout
    static float cursor_x = 0;
    static float cursor_y = 0;
    static float margin_x = 0;
    static float next_window_w = 600.0f;
    static float next_window_h = 400.0f;
    static float current_window_w = 600.0f;
    static float current_window_h = 400.0f;
    static bool same_line_next = false;
    static float next_offset_x = 0;
    static float last_element_h = 0;
    
    // Animations
    static std::map<size_t, float> anim_map;

    // Util
    float lerp(float a, float b, float t) { return a + t * (b - a); }
    D2D1::ColorF lerp_color(const D2D1::ColorF& a, const D2D1::ColorF& b, float t) {
        return D2D1::ColorF(lerp(a.r, b.r, t), lerp(a.g, b.g, t), lerp(a.b, b.b, t), lerp(a.a, b.a, t));
    }
    float animate(size_t id, bool active, float speed = 12.0f) {
        float& val = anim_map[id];
        float dt = 1.0f / 60.0f; // Approx dt
        val = lerp(val, active ? 1.0f : 0.0f, speed * dt);
        return val;
    }

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

    void set_next_window_size(float w, float h) {
        next_window_w = w;
        next_window_h = h;
    }

    bool begin_window(const std::string& title, float& x, float& y) {
        if (!menu_open) return false;

        float w = next_window_w;
        float h = next_window_h;
        current_window_w = w;
        current_window_h = h;

        float header_height = 45.0f;
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

        // Shadow fake
        render::draw_filled_rounded_rect(x + 5, y + 5, w, h, 12.0f, D2D1::ColorF(0, 0, 0, 0.4f));
        
        // Window Background
        render::draw_filled_rounded_rect(x, y, w, h, 12.0f, ColorBackground);
        render::draw_rounded_rect(x, y, w, h, 12.0f, ColorBorder, 1.0f);
        
        // Header
        render::draw_filled_rounded_rect(x, y, w, header_height, 12.0f, ColorPanel);
        render::draw_filled_rounded_rect(x, y + header_height - 10, w, 10, 0.0f, ColorPanel); 
        render::draw_line(x, y + header_height, x + w, y + header_height, ColorBorder, 1.0f);

        render::draw_text(title, x + 20.0f, y + 13.0f, 18.0f, ColorText);

        margin_x = x + 20.0f;
        cursor_x = margin_x;
        cursor_y = y + header_height + 20.0f;

        return true;
    }

    void end_window() { }

    void same_line(float offset_x) {
        same_line_next = true;
        next_offset_x = offset_x;
    }

    void set_cursor(float x, float y) { cursor_x = x; cursor_y = y; }
    void set_cursor_x(float x) { cursor_x = x; }
    void set_cursor_y(float y) { cursor_y = y; }

    static void advance(float w, float h) {
        if (same_line_next) {
            cursor_x += w + (next_offset_x > 0 ? next_offset_x : 15.0f);
            same_line_next = false;
        } else {
            cursor_y += h + 15.0f;
            cursor_x = margin_x;
        }
        last_element_h = h;
    }

    void begin_group_box(const std::string& title, float width, float height) {
        render::draw_filled_rounded_rect(cursor_x, cursor_y, width, height, 8.0f, ColorPanel);
        render::draw_rounded_rect(cursor_x, cursor_y, width, height, 8.0f, ColorBorder, 1.0f);
        
        D2D1_SIZE_F tsize = render::get_text_size(title, 14.0f);
        render::draw_text(title, cursor_x + 15.0f, cursor_y + 12.0f, 14.0f, ColorText);
        render::draw_line(cursor_x + 10, cursor_y + 35, cursor_x + width - 10, cursor_y + 35, ColorBorder, 1.0f);

        margin_x = cursor_x + 15.0f;
        cursor_x = margin_x;
        cursor_y += 50.0f;
    }

    void end_group_box() {
        // Reset margins needs an actual stack in a real framework, but for now we just hardcode the logic in the menu.
    }

    bool tab(const std::string& label, bool active, float w, float h) {
        size_t id = std::hash<std::string>{}(label + "tab");
        bool hovered = (input.mouse_x >= cursor_x && input.mouse_x <= cursor_x + w &&
                        input.mouse_y >= cursor_y && input.mouse_y <= cursor_y + h);
        bool clicked = hovered && input.left_clicked;
        
        float anim = animate(id, hovered || active);
        D2D1::ColorF bg = lerp_color(ColorBackground, ColorPanel, anim);
        if (active) bg = ColorPanel;

        render::draw_filled_rounded_rect(cursor_x, cursor_y, w, h, 6.0f, bg);
        if (active) {
            render::draw_line(cursor_x + 5, cursor_y + h, cursor_x + w - 5, cursor_y + h, ColorPrimary, 2.0f);
        }

        D2D1_SIZE_F tsize = render::get_text_size(label, 15.0f);
        render::draw_text(label, cursor_x + (w - tsize.width) / 2.0f, cursor_y + (h - tsize.height) / 2.0f, 15.0f, active ? ColorPrimary : ColorText);

        advance(w, h);
        return clicked;
    }

    bool button(const std::string& label, float w, float h) {
        size_t id = std::hash<std::string>{}(label + "btn");
        bool hovered = (input.mouse_x >= cursor_x && input.mouse_x <= cursor_x + w &&
                        input.mouse_y >= cursor_y && input.mouse_y <= cursor_y + h);
        bool clicked = hovered && input.left_clicked;
        
        float anim = animate(id, hovered);
        D2D1::ColorF bg = lerp_color(ColorPanel, ColorHover, anim);
        if (hovered && input.left_down) bg = ColorPrimary;

        render::draw_filled_rounded_rect(cursor_x, cursor_y, w, h, 6.0f, bg);
        render::draw_rounded_rect(cursor_x, cursor_y, w, h, 6.0f, ColorBorder, 1.0f);

        D2D1_SIZE_F tsize = render::get_text_size(label, 14.0f);
        render::draw_text(label, cursor_x + (w - tsize.width) / 2.0f, cursor_y + (h - tsize.height) / 2.0f, 14.0f, ColorText);

        advance(w, h);
        return clicked;
    }

    bool toggle(const std::string& label, bool& value) {
        size_t id = std::hash<std::string>{}(label + "tgl");
        float w = 40.0f, h = 20.0f;
        
        D2D1_SIZE_F tsize = render::get_text_size(label, 15.0f);
        float total_w = w + 15.0f + tsize.width;

        bool hovered = (input.mouse_x >= cursor_x && input.mouse_x <= cursor_x + total_w &&
                        input.mouse_y >= cursor_y && input.mouse_y <= cursor_y + std::max(h, tsize.height));
        
        if (hovered && input.left_clicked) value = !value;

        float anim = animate(id, value);
        
        D2D1::ColorF bg = lerp_color(ColorBorder, ColorPrimary, anim);
        render::draw_filled_rounded_rect(cursor_x, cursor_y, w, h, h/2.0f, bg);
        
        float circle_x = lerp(cursor_x + h/2.0f, cursor_x + w - h/2.0f, anim);
        render::draw_circle_filled(circle_x, cursor_y + h/2.0f, h/2.0f - 2.0f, D2D1::ColorF(1,1,1,1));

        render::draw_text(label, cursor_x + w + 15.0f, cursor_y + (h - tsize.height) / 2.0f, 15.0f, ColorText);

        advance(total_w, std::max(h, tsize.height));
        return hovered && input.left_clicked;
    }

    static std::string active_slider = "";
    bool slider_float(const std::string& label, float& value, float min_val, float max_val) {
        float w = 200.0f, h = 10.0f;
        D2D1_SIZE_F tsize = render::get_text_size(label, 15.0f);
        
        float total_h = tsize.height + 10.0f + h;
        
        render::draw_text(label, cursor_x, cursor_y, 15.0f, ColorText);
        
        char buf[32];
        snprintf(buf, sizeof(buf), "%.2f", value);
        D2D1_SIZE_F val_size = render::get_text_size(buf, 15.0f);
        render::draw_text(buf, cursor_x + w - val_size.width, cursor_y, 15.0f, ColorTextMuted);

        float slider_y = cursor_y + tsize.height + 10.0f;
        
        bool hovered = (input.mouse_x >= cursor_x && input.mouse_x <= cursor_x + w &&
                        input.mouse_y >= slider_y - 5.0f && input.mouse_y <= slider_y + h + 5.0f);

        if (hovered && input.left_clicked) active_slider = label;
        if (!input.left_down && active_slider == label) active_slider = "";

        if (active_slider == label) {
            float pct = std::clamp((input.mouse_x - cursor_x) / w, 0.0f, 1.0f);
            value = min_val + pct * (max_val - min_val);
        }

        float pct = std::clamp((value - min_val) / (max_val - min_val), 0.0f, 1.0f);
        
        render::draw_filled_rounded_rect(cursor_x, slider_y, w, h, h/2.0f, ColorPanel);
        render::draw_rounded_rect(cursor_x, slider_y, w, h, h/2.0f, ColorBorder, 1.0f);
        render::draw_filled_rounded_rect(cursor_x, slider_y, w * pct, h, h/2.0f, ColorPrimary);
        render::draw_circle_filled(cursor_x + w * pct, slider_y + h/2.0f, h, ColorText);

        advance(w, total_h);
        return active_slider == label;
    }

    bool slider_int(const std::string& label, int& value, int min_val, int max_val) {
        float float_val = (float)value;
        bool active = slider_float(label, float_val, (float)min_val, (float)max_val);
        if (active) value = (int)std::round(float_val);
        return active;
    }

    void label(const std::string& text) {
        D2D1_SIZE_F tsize = render::get_text_size(text, 15.0f);
        render::draw_text(text, cursor_x, cursor_y, 15.0f, ColorText);
        advance(tsize.width, tsize.height);
    }
}
