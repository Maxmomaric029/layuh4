#include "drawing.h"

void Drawing::text(std::string text, ImVec2 pos, ImColor color) {
    render::draw_text(text, pos.x, pos.y, 14.0f, color);
}

void Drawing::line(const ImVec2& from, const ImVec2& to, const ImColor& color, float thickness) {
    render::draw_line(from.x, from.y, to.x, to.y, color, thickness);
}

void Drawing::outlined_line(const ImVec2& from, const ImVec2& to, ImColor line_color, ImColor outline_color, float thickness) {
    render::draw_line(from.x, from.y, to.x, to.y, outline_color, thickness + 2.0f);
    render::draw_line(from.x, from.y, to.x, to.y, line_color, thickness);
}

void Drawing::box(ImVec2 top_left, ImVec2 bottom_right, ImColor color, float thickness) {
    render::draw_rect(top_left.x, top_left.y, bottom_right.x - top_left.x, bottom_right.y - top_left.y, color, thickness);
}

void Drawing::circle(ImVec2 center, float radius, ImColor color, float thickness) {
    render::draw_circle(center.x, center.y, radius, color, thickness);
}

void Drawing::circle_filled(ImVec2 center, float radius, ImColor color) {
    render::draw_circle_filled(center.x, center.y, radius, color);
}

void Drawing::triangle_filled(ImVec2 p1, ImVec2 p2, ImVec2 p3, ImColor color) {
    render::draw_triangle_filled(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, color);
}

void Drawing::outlined_box(ImVec2 top_left, ImVec2 bottom_right, ImColor box_color, ImColor outline_color, float thickness) {
    box(ImVec2(top_left.x - 1, top_left.y - 1), ImVec2(bottom_right.x + 1, bottom_right.y + 1), outline_color, thickness);
    box(ImVec2(top_left.x + 1, top_left.y + 1), ImVec2(bottom_right.x - 1, bottom_right.y - 1), outline_color, thickness);
    box(top_left, bottom_right, box_color, thickness);
}

void Drawing::filled_box(ImVec2 top_left, ImVec2 bottom_right, ImColor fill_color) {
    render::draw_filled_rect(top_left.x, top_left.y, bottom_right.x - top_left.x, bottom_right.y - top_left.y, fill_color);
}

void Drawing::hotkey(const char* label, int* key, int* mode, ImVec2 size) {
    // Stub
}

void Drawing::hotkey_no_modes(const char* label, int* key, ImVec2 size) {
    // Stub
}
