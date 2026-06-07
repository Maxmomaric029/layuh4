#include "entry.h"
#include <dwmapi.h>
#include <d3d11.h>
#include "../../cheat/menu/menu.h"
#include "../../cheat/aimbot/aimbot.h"
#include "../../cheat/esp/esp.h"
#include "../../cheat/images/image_handler.h"
#include <iostream>
#include "../../cheat/misc/misc.h"
#include "../../cheat/render/renderer.h"
#include "../../cheat/render/ui_framework.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dwmapi.lib")
#pragma comment (lib, "d2d1.lib")

ID3D11Device*               g_pd3dDevice = nullptr;  // non-static: extern'd by menu.cpp
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*          g_pSwapChain = nullptr;
static ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;
static UINT                     g_resizeWidth = 0, g_resizeHeight = 0;

bool create_device_d3d(HWND hWnd);
void cleanup_device_d3d();
void create_render_target();
void cleanup_render_target();
LRESULT WINAPI wnd_proc(HWND h_wnd, UINT msg, WPARAM w_param, LPARAM l_param);



void set_streamproof(HWND hwnd, bool enable) {
    static bool last = false;
    if (last == enable) return;
    last = enable;
    if (enable) {
        SetWindowDisplayAffinity(hwnd, 0x11);
    }
    else {
        SetWindowDisplayAffinity(hwnd, 0x00);
    }
}

void overlay::render()
{
    WNDCLASSEXW wc = { sizeof(wc) , CS_CLASSDC , wnd_proc , 0L , 0L , GetModuleHandle(nullptr) , nullptr , LoadCursor(nullptr, IDC_ARROW) , nullptr , nullptr , oxorany(L"Task Manager") , nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        wc.lpszClassName, oxorany(L"Task Manager"), WS_POPUP,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        nullptr, nullptr, wc.hInstance, nullptr);

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margin);

    if (!create_device_d3d(hwnd))
    {
        cleanup_device_d3d();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    if (!render::initialize(g_pSwapChain)) {
        cleanup_device_d3d();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return;
    }

    dx_device = g_pd3dDevice;

    // Load embedded image assets (needs DX device)
    image_handler.create_images();

    // Menu starts visible — make window interactable immediately
    {
        LONG ex_style = GetWindowLong(hwnd, GWL_EXSTYLE);
        ex_style &= ~WS_EX_TRANSPARENT;
        SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);
    }

    float clear_color[4] = { 0.f, 0.f, 0.f, 0.f };

    bool done = false;
    while (!done)
    {
        set_streamproof(hwnd, vars::misc::streamproof);

        if (GetAsyncKeyState(vars::menu::menukey) & 1)
        {
            overlay::enabled = !overlay::enabled;
            ui::menu_open = overlay::enabled;
            if (overlay::enabled) {
                SetForegroundWindow(hwnd);
                SetFocus(hwnd);
                // Make window receive input
                LONG ex_style = GetWindowLong(hwnd, GWL_EXSTYLE);
                ex_style &= ~WS_EX_TRANSPARENT;
                SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);
            } else {
                // Pass input through to game
                LONG ex_style = GetWindowLong(hwnd, GWL_EXSTYLE);
                ex_style |= WS_EX_TRANSPARENT;
                SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);
            }
        }

        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
            {
                done = true;
                break;
            }
        }
        if (done) break;

        if (g_resizeWidth != 0 && g_resizeHeight != 0)
        {
            render::cleanup_render_target();
            cleanup_render_target();
            g_pSwapChain->ResizeBuffers(0, g_resizeWidth, g_resizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_resizeWidth = g_resizeHeight = 0;
            create_render_target();
            render::create_render_target(g_pSwapChain);
        }

        ui::update_input(hwnd);

        HWND roblox_window = FindWindowA(NULL, oxorany("Roblox"));
        bool roblox_focused = (GetForegroundWindow() == roblox_window);

        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);

        render::begin_frame();

        if (globals::local_player && globals::datamodel && (!vars::misc::roblox_focused_check || roblox_focused))
        {
            if (globals::visual_engine) {
                view_matrix = read<matrix>(globals::visual_engine + offsets::viewmatrix);
            }

            fs::esp.draw_players(view_matrix);
            fs::esp.draw_radar(view_matrix);

            fs::aimbot.aim_at_closest_player(view_matrix);
            fs::aimbot.triggerbot(view_matrix);
            fs::aimbot.circle_target();
            fs::aimbot.sex_target();

            fs::misc.teleport_to_nearest(view_matrix);
            fs::misc.noclip();
            fs::misc.fly();
            fs::misc.spinbot();
            fs::misc.speed_hack();
            fs::misc.jump_power();
            fs::misc.custom_fov();
            fs::misc.gravity_modifier();
            fs::misc.rapid_fire();
            fs::misc.headless();
            fs::misc.NoJumpCoolDown();
            fs::misc.antistomp_realud();
        }

        if (overlay::enabled)
        {
            d::menu.draw_menu();
        }

        render::end_frame();
        g_pSwapChain->Present(0, 0);
    }

    render::cleanup();

    cleanup_device_d3d();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

bool create_device_d3d(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    create_render_target();
    return true;
}

void cleanup_device_d3d()
{
    cleanup_render_target();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void create_render_target()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void cleanup_render_target()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

LRESULT WINAPI wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ui::menu_open) {
        // Return 1 if we consumed a mouse message, so it doesn't propagate
        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_MOUSEMOVE) {
            // we could return 1 here if we wanted to block game input
        }
    }

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_resizeWidth = (UINT)LOWORD(lParam);
        g_resizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
