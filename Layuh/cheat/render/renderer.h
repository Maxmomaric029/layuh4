#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <d3d11.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <string>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

namespace render {
    // Inicializa Direct2D y DirectWrite sobre un SwapChain existente de DX11
    bool initialize(IDXGISwapChain* swapChain);
    void cleanup();

    bool create_render_target(IDXGISwapChain* swapChain);
    void cleanup_render_target();

    // Empieza y termina los dibujos de un frame
    void begin_frame();
    void end_frame();

    // Dibuja texto simple
    void draw_text(const std::string& text, float x, float y, float size = 16.0f, D2D1::ColorF color = D2D1::ColorF(D2D1::ColorF::White), bool center = false);
    
    // Dibuja rectangulos
    void draw_rect(float x, float y, float w, float h, D2D1::ColorF color, float thickness = 1.0f);
    void draw_filled_rect(float x, float y, float w, float h, D2D1::ColorF color);
    
    // Dibuja rectangulos con bordes redondeados (Estilo CSS)
    void draw_rounded_rect(float x, float y, float w, float h, float radius, D2D1::ColorF color, float thickness = 1.0f);
    void draw_filled_rounded_rect(float x, float y, float w, float h, float radius, D2D1::ColorF color);

    // Lineas
    void draw_line(float x1, float y1, float x2, float y2, D2D1::ColorF color, float thickness = 1.0f);

    // Circulos / Elipses
    void draw_circle(float cx, float cy, float radius, D2D1::ColorF color, float thickness = 1.0f);
    void draw_circle_filled(float cx, float cy, float radius, D2D1::ColorF color);

    // Triangulos
    void draw_triangle_filled(float x1, float y1, float x2, float y2, float x3, float y3, D2D1::ColorF color);

    // Helpers
    D2D1_SIZE_F get_text_size(const std::string& text, float size = 16.0f);
}
