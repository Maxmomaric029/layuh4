#pragma once

#include <string>
#include "cheat/render/renderer.h"

// Stub out ImVec2 / ImColor so we don't have to rewrite the whole esp math right away
struct ImVec2 {
    float x, y;
    ImVec2() : x(0), y(0) {}
    ImVec2(float _x, float _y) : x(_x), y(_y) {}
};

struct ImVec4 {
    float x, y, z, w;
    ImVec4() : x(0), y(0), z(0), w(0) {}
    ImVec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
};

struct ImColor {
    ImVec4 Value;
    ImColor() { Value.x = Value.y = Value.z = Value.w = 0.0f; }
    ImColor(int r, int g, int b, int a = 255) {
        float sc = 1.0f / 255.0f;
        Value.x = (float)r * sc;
        Value.y = (float)g * sc;
        Value.z = (float)b * sc;
        Value.w = (float)a * sc;
    }
    ImColor(ImVec4 col) { Value = col; }
    operator D2D1::ColorF() const {
        return D2D1::ColorF(Value.x, Value.y, Value.z, Value.w);
    }
};

class Drawing {
public:
    void text(std::string text, ImVec2 pos, ImColor color);
    void line(const ImVec2& from, const ImVec2& to, const ImColor& color, float thickness);
    void outlined_line(const ImVec2& from, const ImVec2& to, ImColor line_color, ImColor outline_color, float thickness);
    void box(ImVec2 top_left, ImVec2 bottom_right, ImColor color, float thickness);
    void circle(ImVec2 center, float radius, ImColor color, float thickness);
    void outlined_box(ImVec2 top_left, ImVec2 bottom_right, ImColor box_color, ImColor outline_color, float thickness);
    void filled_box(ImVec2 top_left, ImVec2 bottom_right, ImColor fill_color);
    void circle_filled(ImVec2 center, float radius, ImColor color);
    void triangle_filled(ImVec2 p1, ImVec2 p2, ImVec2 p3, ImColor color);
    
    // UI elements stubbed
    void hotkey(const char* label, int* key, int* mode, ImVec2 size);
    void hotkey_no_modes(const char* label, int* key, ImVec2 size);
};

namespace drawingapi { inline Drawing drawing; }
