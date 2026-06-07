#include "entry.h"
#include <dwmapi.h>
#include <d3d11.h>
#include "../../cheat/menu/menu.h"
#include "../../cheat/aimbot/aimbot.h"
#include "../../cheat/esp/esp.h"
#include "../../cheat/images/image_handler.h"
#include <iostream>
#include "../../cheat/misc/misc.h"
#include "imgui_impl_dx11.h"

#pragma comment (lib, "d3d11.lib")

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

void draw_spotify()
{
    static ImVec2 window_pos(20, ImGui::GetIO().DisplaySize.y / 2 - 100);
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_Always);

    static HWND spotify_hwnd = nullptr;
    static std::string artist, song;
    static float last_update = 0.0f;
    float now = ImGui::GetTime();

    if (now - last_update > 1.0f) {
        last_update = now;
        spotify_hwnd = nullptr;
        char window_title[512] = { 0 };
        bool found_spotify = false;

        auto enum_windows = [](HWND hwnd, LPARAM lParam) -> BOOL {
            DWORD pid = 0;
            GetWindowThreadProcessId(hwnd, &pid);
            char class_name[128];
            GetClassNameA(hwnd, class_name, sizeof(class_name));
            if (IsWindowVisible(hwnd) && strstr(class_name, "Chrome_WidgetWin_1")) {
                DWORD spotify_pid = 0;
                HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
                if (hSnap != INVALID_HANDLE_VALUE) {
                    PROCESSENTRY32 pe;
                    pe.dwSize = sizeof(pe);
                    if (Process32First(hSnap, &pe)) {
                        do {
                            if (_stricmp(pe.szExeFile, "Spotify.exe") == 0) {
                                spotify_pid = pe.th32ProcessID;
                                break;
                            }
                        } while (Process32Next(hSnap, &pe));
                    }
                    CloseHandle(hSnap);
                }
                if (pid == spotify_pid) {
                    *((HWND*)lParam) = hwnd;
                    return FALSE;
                }
            }
            return TRUE;
            };
        EnumWindows(enum_windows, (LPARAM)&spotify_hwnd);

        if (spotify_hwnd) {
            found_spotify = true;
            GetWindowTextA(spotify_hwnd, window_title, sizeof(window_title));
            std::string title(window_title);
            if (title == "Spotify") {
                artist = "Spotify";
                song = "Nothing Playing";
            }
            else {
                size_t dash = title.find(" - ");
                if (dash != std::string::npos) {
                    artist = title.substr(0, dash);
                    song = title.substr(dash + 3);
                }
            }
        }

        if (!found_spotify) {
            artist = "Couldn't find Spotify Client";
            song = "";
        }
        else if (artist.empty() || song.empty()) {
            artist = "Spotify";
            song = "Not Playing";
        }
    }

    if (ImGui::Begin("Spotify", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Spotify");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5216f, 0.0118f, 0.1373f, 1.0f));
        ImGui::TextUnformatted(artist.c_str());
        ImGui::PopStyleColor();
        if (!song.empty()) {
            ImGui::SameLine(0, 0);
            ImGui::TextUnformatted(" - ");
            ImGui::SameLine(0, 0);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
            ImGui::TextUnformatted(song.c_str());
            ImGui::PopStyleColor();
        }
    }
    ImGui::End();
}

std::string get_local_player_name_ui()
{
    if (!globals::local_player)
        return oxorany("Unknown");

    std::string player_name = utils::get_instance_name(globals::local_player);
    return player_name.empty() ? oxorany("Unknown") : player_name;
}

void draw_keybinds() {
    static ImVec2 window_pos(20, ImGui::GetIO().DisplaySize.y / 2 - 100);
    static bool is_dragging = false;
    static ImVec2 drag_offset(0, 0);

    ImGui::SetNextWindowPos(window_pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(230, 0), ImGuiCond_Always);

    if (ImGui::Begin("", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text(oxorany("Keybinds"));
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        bool any_keybinds = false;

        auto GetKeyName = [](int keyCode) -> const char* {
            switch (keyCode) {
            case VK_LBUTTON:   return "mouse1";
            case VK_RBUTTON:   return "mouse2";
            case VK_MBUTTON:   return "mouse3";
            case VK_XBUTTON1:  return "mouse4";
            case VK_XBUTTON2:  return "mouse5";
            case VK_SHIFT:     return "shift";
            case VK_CONTROL:   return "ctrl";
            case VK_MENU:      return "alt";
            case VK_TAB:       return "tab";
            case VK_ESCAPE:    return "escape";
            case VK_SPACE:     return "space";
            default:
                if (keyCode > 0 && keyCode < 256) {
                    static char buffer[2];
                    buffer[0] = static_cast<char>(keyCode);
                    buffer[1] = '\0';
                    return buffer;
                }
                return "unknown";
            }
            };

        if (vars::aimbot::aimbot_enabled && vars::aimbot::aimbot_key != 0) {
            ImGui::Text(oxorany("Aimbot"));
            ImGui::SameLine(120);
            ImGui::TextColored(ImVec4(0.5216f, 0.0118f, 0.1373f, 1.0f), ("[%s] %s"),
                GetKeyName(vars::aimbot::aimbot_key),
                vars::aimbot::aimbot_mode == 0 ? oxorany("Hold") : oxorany("Toggle"));
            any_keybinds = true;
        }

        if (vars::aimbot::triggerbot && vars::aimbot::triggerbot_key != 0) {
            ImGui::Text(oxorany("Triggerbot"));
            ImGui::SameLine(120);
            ImGui::TextColored(ImVec4(0.5216f, 0.0118f, 0.1373f, 1.0f), ("[%s] %s"),
                GetKeyName(vars::aimbot::triggerbot_key),
                vars::aimbot::triggerbot_key_mode == 0 ? ("Hold") : ("Toggle"));
            any_keybinds = true;
        }

        if (vars::misc::noclip_enabled && vars::misc::noclip_key != 0) {
            ImGui::Text(oxorany("Noclip"));
            ImGui::SameLine(120);
            ImGui::TextColored(ImVec4(0.5216f, 0.0118f, 0.1373f, 1.0f), ("[%s] %s"),
                GetKeyName(vars::misc::noclip_key),
                vars::misc::noclip_mode == 0 ? ("Hold") : ("Toggle"));
            any_keybinds = true;
        }

        if (vars::misc::fly_enabled && vars::misc::fly_toggle_key != 0) {
            ImGui::Text(oxorany("Fly"));
            ImGui::SameLine(120);
            ImGui::TextColored(ImVec4(0.5216f, 0.0118f, 0.1373f, 1.0f), ("[%s] %s"),
                GetKeyName(vars::misc::fly_toggle_key),
                vars::misc::fly_mode == 0 ? ("Hold") : ("Toggle"));
            any_keybinds = true;
        }

		if (vars::misc::speed_hack_enabled && vars::misc::speed_hack_toggle_key != 0) {
			ImGui::Text(oxorany("Speed Hack"));
			ImGui::SameLine(120);
			ImGui::TextColored(ImVec4(0.5216f, 0.0118f, 0.1373f, 1.0f), ("[%s] %s"),
                GetKeyName(vars::misc::speed_hack_toggle_key),
                vars::misc::speed_hack_keybind_mode == 0 ? ("Hold") : ("Toggle"));
			any_keybinds = true;
		}

        if (!any_keybinds) {
            ImGui::TextColored(ImVec4(0.5216f, 0.0118f, 0.1373f, 1.0f), ("No active keybinds"));
        }

    }
    ImGui::End();
}

void draw_watermark_ui()
{
    ImDrawList* draw_list = ImGui::GetOverlayDrawList();
    std::string player_name = get_local_player_name_ui();
    auto watermark = oxorany("DeadLock UI");
    auto fps_text = std::format(" -> fps: {:.1f}", ImGui::GetIO().Framerate);
    auto name_text = std::format(" | {}", player_name);
    std::string full_text = watermark + fps_text + name_text;

    auto full_text_size = ImGui::CalcTextSize(full_text.c_str());
    ImVec2 rect_min(6, 9), rect_max(6 + full_text_size.x + 12, 9 + full_text_size.y + 8);

    draw_list->AddRectFilled(rect_min, ImVec2(rect_max.x, rect_min.y + 2), ImColor(0x5A, 0x18, 0x9A)); // Deep Purple
    draw_list->AddRectFilled(ImVec2(rect_min.x, rect_min.y + 2), rect_max, ImColor(15, 17, 23, 230)); // Blue-black ish
    draw_list->AddText(ImVec2(rect_min.x + 6, rect_min.y + 4), ImColor(255, 255, 255, 255), full_text.c_str());
}

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
    WNDCLASSEXW wc = { sizeof(wc) , CS_CLASSDC , wnd_proc , 0L , 0L , GetModuleHandle(nullptr) , nullptr , nullptr , nullptr , nullptr , oxorany(L"Task Manager") , nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT, wc.lpszClassName, oxorany(L"Task Manager"), WS_POPUP,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, wc.hInstance, nullptr);

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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // We will setup modern styling in menu.cpp, calling it here
    setup_style();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    dx_device = g_pd3dDevice; // Expose to rest of codebase (image_handler, menu)
    image_handler.create_images(); // Load instance explorer icons

    ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 0.f);

    bool done = false;
    while (!done)
    {
        set_streamproof(hwnd, vars::misc::streamproof);

        if (GetAsyncKeyState(vars::menu::menukey) & 1)
        {
            overlay::enabled = !overlay::enabled;
            // Manage window transparency so we can click through it when disabled, and click on it when enabled
            if (overlay::enabled) {
                // Focus window
                SetForegroundWindow(hwnd);
                SetFocus(hwnd);
                LONG ex_style = GetWindowLong(hwnd, GWL_EXSTYLE);
                ex_style &= ~WS_EX_TRANSPARENT; // Remove transparent so we can click on ImGui
                SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);
            } else {
                LONG ex_style = GetWindowLong(hwnd, GWL_EXSTYLE);
                ex_style |= WS_EX_TRANSPARENT; // Add transparent so clicks pass through to game
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
        if (done)
            break;

        if (g_resizeWidth != 0 && g_resizeHeight != 0)
        {
            cleanup_render_target();
            g_pSwapChain->ResizeBuffers(0, g_resizeWidth, g_resizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_resizeWidth = g_resizeHeight = 0;
            create_render_target();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        HWND roblox_window = FindWindowA(NULL, oxorany("Roblox"));
        bool roblox_focused = (GetForegroundWindow() == roblox_window);

        // Drawing loops
        if (
            globals::local_player &&
            globals::datamodel &&
            (
                !vars::misc::roblox_focused_check || // If the check is off, always draw
                roblox_focused                      // If the check is on, only draw if focused
                )
            )
        {
            if (globals::visual_engine)
            {
                view_matrix = read<matrix>(globals::visual_engine + offsets::viewmatrix);
            }
            if (vars::aimbot::circle_target::enabled)
            {
                fs::aimbot.circle_target();
            }
            if (vars::misc::sex_enabled)
            {
                fs::aimbot.sex_target();
            }
            if (vars::aimbot::aimbot_enabled)
            {
                fs::aimbot.aim_at_closest_player(view_matrix);
            }
            if (vars::aimbot::fov_circle && vars::aimbot::aimbot_enabled)
            {
                POINT cursor;
                GetCursorPos(&cursor);
                ImDrawList* draw_list = ImGui::GetOverlayDrawList();
				ImVec2 CursorPosition = ImVec2(static_cast<float>(cursor.x), static_cast<float>(cursor.y));
                draw_list->AddCircle(CursorPosition, vars::aimbot::fov_value, vars::visuals::fov_circle_color, 100, 1.5f);
            }
            if (vars::esp::watermark)
            {
                draw_watermark_ui();
            }
            if (vars::esp::keybindlist) {
                draw_keybinds();
            }
			if (vars::misc::spotify) {
				draw_spotify();
			}
            if (!overlay::enabled) {
                fs::esp.draw_players(view_matrix);
            }

            render_notifications();
            fs::esp.draw_radar(view_matrix);
            fs::misc.noclip();
            fs::misc.spinbot();
            fs::misc.speed_hack();
            fs::misc.jump_power();
            fs::aimbot.triggerbot(view_matrix);
            fs::misc.antistomp_realud();
            fs::misc.headless();
            fs::misc.rapid_fire();
            fs::misc.fly();
			fs::misc.NoJumpCoolDown();
        }

        if (overlay::enabled)
        {
            d::menu.draw_menu();
        }

        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(0, 0); // Present without vsync
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

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

    UINT createDeviceFlags = 0;
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

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

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
