#include "renderer.h"

namespace render {

    static ID2D1Factory* d2dFactory = nullptr;
    static ID2D1RenderTarget* d2dRenderTarget = nullptr;
    static IDWriteFactory* dWriteFactory = nullptr;
    static IDWriteTextFormat* textFormat = nullptr;

    bool initialize(IDXGISwapChain* swapChain) {
        // Inicializar Direct2D Factory
        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);
        if (FAILED(hr)) return false;

        // Inicializar DirectWrite Factory
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&dWriteFactory)
        );
        if (FAILED(hr)) return false;

        // Obtener el back buffer del swapchain (como DXGISurface)
        IDXGISurface* dxgiBackBuffer = nullptr;
        hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));
        if (FAILED(hr)) return false;

        // Crear el render target de D2D sobre el DXGISurface
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
        );

        hr = d2dFactory->CreateDxgiSurfaceRenderTarget(dxgiBackBuffer, &props, &d2dRenderTarget);
        dxgiBackBuffer->Release();
        
        if (FAILED(hr)) return false;

        // Crear un formato de texto por defecto (Segoe UI)
        hr = dWriteFactory->CreateTextFormat(
            L"Segoe UI",
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            16.0f,
            L"en-us",
            &textFormat
        );
        
        return SUCCEEDED(hr);
    }

    void cleanup() {
        if (textFormat) { textFormat->Release(); textFormat = nullptr; }
        if (dWriteFactory) { dWriteFactory->Release(); dWriteFactory = nullptr; }
        if (d2dRenderTarget) { d2dRenderTarget->Release(); d2dRenderTarget = nullptr; }
        if (d2dFactory) { d2dFactory->Release(); d2dFactory = nullptr; }
    }

    void begin_frame() {
        if (d2dRenderTarget) {
            d2dRenderTarget->BeginDraw();
        }
    }

    void end_frame() {
        if (d2dRenderTarget) {
            d2dRenderTarget->EndDraw();
        }
    }

    void draw_text(const std::string& text, float x, float y, float size, D2D1::ColorF color, bool center) {
        if (!d2dRenderTarget || !dWriteFactory || text.empty()) return;

        // Convertir std::string a std::wstring
        int len = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, NULL, 0);
        std::wstring wtext(len, 0);
        MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, &wtext[0], len);

        IDWriteTextFormat* format = nullptr;
        dWriteFactory->CreateTextFormat(
            L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, L"en-us", &format
        );

        if (!format) return;
        
        if (center) {
            format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        }

        ID2D1SolidColorBrush* brush = nullptr;
        d2dRenderTarget->CreateSolidColorBrush(color, &brush);

        if (brush) {
            D2D1_RECT_F layoutRect = D2D1::RectF(x, y, x + 1000.0f, y + 1000.0f);
            if (center) {
                // Si esta centrado, le damos un ancho fijo y dibujamos desde un offset
                layoutRect = D2D1::RectF(x - 500.0f, y, x + 500.0f, y + 1000.0f);
            }
            d2dRenderTarget->DrawText(wtext.c_str(), wtext.length() - 1, format, layoutRect, brush);
            brush->Release();
        }

        format->Release();
    }

    void draw_rect(float x, float y, float w, float h, D2D1::ColorF color, float thickness) {
        if (!d2dRenderTarget) return;
        ID2D1SolidColorBrush* brush = nullptr;
        d2dRenderTarget->CreateSolidColorBrush(color, &brush);
        if (brush) {
            d2dRenderTarget->DrawRectangle(D2D1::RectF(x, y, x + w, y + h), brush, thickness);
            brush->Release();
        }
    }

    void draw_filled_rect(float x, float y, float w, float h, D2D1::ColorF color) {
        if (!d2dRenderTarget) return;
        ID2D1SolidColorBrush* brush = nullptr;
        d2dRenderTarget->CreateSolidColorBrush(color, &brush);
        if (brush) {
            d2dRenderTarget->FillRectangle(D2D1::RectF(x, y, x + w, y + h), brush);
            brush->Release();
        }
    }

    void draw_rounded_rect(float x, float y, float w, float h, float radius, D2D1::ColorF color, float thickness) {
        if (!d2dRenderTarget) return;
        ID2D1SolidColorBrush* brush = nullptr;
        d2dRenderTarget->CreateSolidColorBrush(color, &brush);
        if (brush) {
            D2D1_ROUNDED_RECT rect = D2D1::RoundedRect(D2D1::RectF(x, y, x + w, y + h), radius, radius);
            d2dRenderTarget->DrawRoundedRectangle(rect, brush, thickness);
            brush->Release();
        }
    }

    void draw_filled_rounded_rect(float x, float y, float w, float h, float radius, D2D1::ColorF color) {
        if (!d2dRenderTarget) return;
        ID2D1SolidColorBrush* brush = nullptr;
        d2dRenderTarget->CreateSolidColorBrush(color, &brush);
        if (brush) {
            D2D1_ROUNDED_RECT rect = D2D1::RoundedRect(D2D1::RectF(x, y, x + w, y + h), radius, radius);
            d2dRenderTarget->FillRoundedRectangle(rect, brush);
            brush->Release();
        }
    }

    void draw_line(float x1, float y1, float x2, float y2, D2D1::ColorF color, float thickness) {
        if (!d2dRenderTarget) return;
        ID2D1SolidColorBrush* brush = nullptr;
        d2dRenderTarget->CreateSolidColorBrush(color, &brush);
        if (brush) {
            d2dRenderTarget->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), brush, thickness);
            brush->Release();
        }
    }

    void draw_circle(float cx, float cy, float radius, D2D1::ColorF color, float thickness) {
        if (!d2dRenderTarget) return;
        ID2D1SolidColorBrush* brush = nullptr;
        d2dRenderTarget->CreateSolidColorBrush(color, &brush);
        if (brush) {
            D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(cx, cy), radius, radius);
            d2dRenderTarget->DrawEllipse(ellipse, brush, thickness);
            brush->Release();
        }
    }

    void draw_circle_filled(float cx, float cy, float radius, D2D1::ColorF color) {
        if (!d2dRenderTarget) return;
        ID2D1SolidColorBrush* brush = nullptr;
        d2dRenderTarget->CreateSolidColorBrush(color, &brush);
        if (brush) {
            D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(cx, cy), radius, radius);
            d2dRenderTarget->FillEllipse(ellipse, brush);
            brush->Release();
        }
    }

    void draw_triangle_filled(float x1, float y1, float x2, float y2, float x3, float y3, D2D1::ColorF color) {
        if (!d2dRenderTarget || !d2dFactory) return;
        ID2D1PathGeometry* path = nullptr;
        HRESULT hr = d2dFactory->CreatePathGeometry(&path);
        if (FAILED(hr) || !path) return;

        ID2D1GeometrySink* sink = nullptr;
        hr = path->Open(&sink);
        if (SUCCEEDED(hr) && sink) {
            sink->BeginFigure(D2D1::Point2F(x1, y1), D2D1_FIGURE_BEGIN_FILLED);
            sink->AddLine(D2D1::Point2F(x2, y2));
            sink->AddLine(D2D1::Point2F(x3, y3));
            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
            sink->Close();
            sink->Release();

            ID2D1SolidColorBrush* brush = nullptr;
            d2dRenderTarget->CreateSolidColorBrush(color, &brush);
            if (brush) {
                d2dRenderTarget->FillGeometry(path, brush);
                brush->Release();
            }
        }
        path->Release();
    }

    D2D1_SIZE_F get_text_size(const std::string& text, float size) {
        if (!dWriteFactory || text.empty()) return D2D1::SizeF(0, 0);

        int len = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, NULL, 0);
        std::wstring wtext(len, 0);
        MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, &wtext[0], len);

        IDWriteTextFormat* format = nullptr;
        dWriteFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, L"en-us", &format);
        if (!format) return D2D1::SizeF(0, 0);

        IDWriteTextLayout* layout = nullptr;
        dWriteFactory->CreateTextLayout(wtext.c_str(), wtext.length() - 1, format, 10000.0f, 10000.0f, &layout);
        
        D2D1_SIZE_F result = D2D1::SizeF(0, 0);
        if (layout) {
            DWRITE_TEXT_METRICS metrics;
            layout->GetMetrics(&metrics);
            result = D2D1::SizeF(metrics.width, metrics.height);
            layout->Release();
        }
        format->Release();
        return result;
    }
}
