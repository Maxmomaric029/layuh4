#include "entry.h"
#include "cheat/aimbot/aimbot.h"
#include "cheat/globals/roblox.h"
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#include <iostream>
#include <string>

// ─── Win32 DeadLock key dialog ────────────────────────────────────────────────
// A proper Win32 dialog that prompts for the DeadLock access key.
// Shows after the startup sequence completes.
// Returns true if the key was accepted, false if denied/closed.
// ─────────────────────────────────────────────────────────────────────────────

static char  g_key_input[256] = {};
static bool  g_key_accepted   = false;
static HWND  g_dlg_hwnd       = NULL;

static INT_PTR CALLBACK KeyDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        // Centre on screen
        RECT rc; GetClientRect(hDlg, &rc);
        int w = rc.right - rc.left, h = rc.bottom - rc.top;
        int sw = GetSystemMetrics(SM_CXSCREEN), sh = GetSystemMetrics(SM_CYSCREEN);
        SetWindowPos(hDlg, HWND_TOP, (sw - w) / 2, (sh - h) / 2, 0, 0, SWP_NOSIZE);
        return TRUE;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            HWND hEdit = GetDlgItem(hDlg, 1001);
            GetWindowTextA(hEdit, g_key_input, sizeof(g_key_input));
            if (strcmp(g_key_input, "deadlock") == 0) {
                g_key_accepted = true;
                EndDialog(hDlg, IDOK);
            } else {
                MessageBoxA(hDlg,
                    "Invalid key. Access denied.",
                    "DeadLock Authentication",
                    MB_OK | MB_ICONERROR);
                EndDialog(hDlg, IDCANCEL);
            }
            return TRUE;
        }
        if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// Build and show the key dialog entirely in code (no .rc file needed)
static bool ShowKeyDialog()
{
    // Dialog template in memory:
    // DLGTEMPLATE -> title / controls
    //   STATIC  "Enter DeadLock access key:"
    //   EDIT    (password style)
    //   BUTTON  OK
    //   BUTTON  Cancel

    // We use a simple aligned buffer for the DLGTEMPLATE
    struct alignas(DWORD) DlgBuf {
        DLGTEMPLATE hdr;
        WORD menu, cls;
        WCHAR title[32];
        // --- items follow, each DWORD-aligned
    };

    // Build dialog in memory
    const int BUF = 1024;
    static BYTE buf[BUF];
    memset(buf, 0, BUF);
    BYTE* p = buf;

    // DLGTEMPLATE
    DLGTEMPLATE* dt = (DLGTEMPLATE*)p;
    dt->style          = DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU | DS_SETFONT;
    dt->dwExtendedStyle = 0;
    dt->cdit           = 4;     // 4 controls
    dt->x  = 0; dt->y = 0;
    dt->cx = 220; dt->cy = 90;
    p += sizeof(DLGTEMPLATE);

    // menu (none) = 0x0000
    *(WORD*)p = 0; p += 2;
    // class (none) = 0x0000
    *(WORD*)p = 0; p += 2;
    // title
    const wchar_t* dlgTitle = L"DeadLock Authentication";
    size_t tlen = wcslen(dlgTitle) + 1;
    memcpy(p, dlgTitle, tlen * 2); p += tlen * 2;
    // font size + name (DS_SETFONT)
    *(WORD*)p = 9; p += 2;  // point size
    const wchar_t* fontName = L"Segoe UI";
    size_t flen = wcslen(fontName) + 1;
    memcpy(p, fontName, flen * 2); p += flen * 2;

    auto align4 = [](BYTE*& ptr) { while ((DWORD_PTR)ptr & 3) ptr++; };

    // -- Control: STATIC label ------------------------------------------------
    align4(p);
    {
        DLGITEMTEMPLATE* di = (DLGITEMTEMPLATE*)p;
        di->style = WS_CHILD | WS_VISIBLE | SS_LEFT;
        di->dwExtendedStyle = 0;
        di->x = 10; di->y = 10; di->cx = 200; di->cy = 12;
        di->id = 1000;
        p += sizeof(DLGITEMTEMPLATE);
        *(WORD*)p = 0xFFFF; p += 2;  // class atom
        *(WORD*)p = 0x0082; p += 2;  // STATIC
        const wchar_t* lbl = L"Enter DeadLock access key:";
        size_t ll = wcslen(lbl) + 1;
        memcpy(p, lbl, ll * 2); p += ll * 2;
        *(WORD*)p = 0; p += 2;  // no creation data
    }

    // -- Control: EDIT (password) ---------------------------------------------
    align4(p);
    {
        DLGITEMTEMPLATE* di = (DLGITEMTEMPLATE*)p;
        di->style = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_PASSWORD | ES_AUTOHSCROLL;
        di->dwExtendedStyle = 0;
        di->x = 10; di->y = 26; di->cx = 200; di->cy = 14;
        di->id = 1001;
        p += sizeof(DLGITEMTEMPLATE);
        *(WORD*)p = 0xFFFF; p += 2;
        *(WORD*)p = 0x0081; p += 2;  // EDIT
        *(WORD*)p = 0; p += 2;       // empty text
        *(WORD*)p = 0; p += 2;       // no creation data
    }

    // -- Control: OK button ---------------------------------------------------
    align4(p);
    {
        DLGITEMTEMPLATE* di = (DLGITEMTEMPLATE*)p;
        di->style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON;
        di->dwExtendedStyle = 0;
        di->x = 55; di->y = 50; di->cx = 50; di->cy = 14;
        di->id = IDOK;
        p += sizeof(DLGITEMTEMPLATE);
        *(WORD*)p = 0xFFFF; p += 2;
        *(WORD*)p = 0x0080; p += 2;  // BUTTON
        const wchar_t* txt = L"OK";
        size_t tl = wcslen(txt) + 1;
        memcpy(p, txt, tl * 2); p += tl * 2;
        *(WORD*)p = 0; p += 2;
    }

    // -- Control: Cancel button -----------------------------------------------
    align4(p);
    {
        DLGITEMTEMPLATE* di = (DLGITEMTEMPLATE*)p;
        di->style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON;
        di->dwExtendedStyle = 0;
        di->x = 115; di->y = 50; di->cx = 50; di->cy = 14;
        di->id = IDCANCEL;
        p += sizeof(DLGITEMTEMPLATE);
        *(WORD*)p = 0xFFFF; p += 2;
        *(WORD*)p = 0x0080; p += 2;  // BUTTON
        const wchar_t* txt = L"Cancel";
        size_t tl = wcslen(txt) + 1;
        memcpy(p, txt, tl * 2); p += tl * 2;
        *(WORD*)p = 0; p += 2;
    }

    g_key_accepted = false;
    INT_PTR result = DialogBoxIndirectA(
        GetModuleHandle(NULL),
        (LPDLGTEMPLATE)buf,
        NULL,
        KeyDialogProc);

    return g_key_accepted;
}


// ─── Utility: read null-terminated string from game memory ───────────────────
std::string readstring(std::uint64_t address)
{
    std::string s;
    char ch = 0;
    int offset = 0;
    s.reserve(204);
    while (offset < 200) {
        ch = read<char>(address + offset);
        if (ch == 0) break;
        offset += sizeof(ch);
        s.push_back(ch);
    }
    return s;
}

// ─── Notification system ─────────────────────────────────────────────────────
struct Notification {
    std::string message;
    ImVec4 color;
    float creation_time;
    float duration;
    float alpha;
    float y_offset;
    bool removing;
};

class NotificationSystem {
private:
    std::vector<Notification> notifications;
    float padding = 10.0f;
    float margin = 10.0f;
    float fade_in_time = 0.15f;
    float fade_out_time = 0.15f;
    float animation_speed = 500.0f;

public:
    void add_notification(const std::string& message, ImVec4 color = ImVec4(1,1,1,1), float duration = 3.0f) {
        Notification n;
        n.message = message; n.color = color;
        n.creation_time = ImGui::GetTime(); n.duration = duration;
        n.alpha = 0.0f; n.y_offset = 0.0f; n.removing = false;
        notifications.push_back(n);
    }

    void render() {
        if (notifications.empty()) return;
        float current_time = ImGui::GetTime();
        ImVec2 screen_size = ImGui::GetIO().DisplaySize;
        float current_height = screen_size.y - margin;
        ImDrawList* draw_list = ImGui::GetOverlayDrawList();

        for (int i = (int)notifications.size() - 1; i >= 0; i--) {
            auto& notif = notifications[i];
            float elapsed = current_time - notif.creation_time;

            if (elapsed > notif.duration && !notif.removing)
                notif.removing = true;

            if (notif.removing) {
                notif.alpha = ImMax(0.0f, notif.alpha - ImGui::GetIO().DeltaTime / fade_out_time);
                notif.y_offset += ImGui::GetIO().DeltaTime * animation_speed;
                if (notif.alpha <= 0.0f) { notifications.erase(notifications.begin() + i); continue; }
            } else {
                notif.alpha = ImMin(1.0f, elapsed / fade_in_time);
                notif.y_offset = ImMax(0.0f, notif.y_offset - ImGui::GetIO().DeltaTime * animation_speed);
            }

            ImVec2 text_size = ImGui::CalcTextSize(notif.message.c_str());
            float width = text_size.x + padding * 2;
            float max_width = screen_size.x * 0.3f;

            if (width > max_width) {
                width = max_width;
                ImVec2 text_size_wrapped;
                float wrap_width = max_width - padding * 2;
                float wrap_spacing = 4.0f;
                const char* text_begin = notif.message.c_str();
                const char* text_end = text_begin + notif.message.length();
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, wrap_spacing));
                text_size_wrapped = ImGui::CalcTextSize(oxorany("A"));
                float line_height = text_size_wrapped.y + wrap_spacing;
                int line_count = 1;
                float line_width = 0.0f;
                for (const char* s = text_begin; s < text_end; s++) {
                    char c = *s;
                    if (c == '\n') { line_count++; line_width = 0; }
                    else {
                        line_width += ImGui::CalcTextSize(&c, &c + 1).x;
                        if (line_width > wrap_width) { line_count++; line_width = 0; }
                    }
                }
                ImGui::PopStyleVar();
                float height = line_count * line_height + padding * 2;
                ImVec2 wp(screen_size.x - width - margin + notif.y_offset, current_height - height);
                draw_list->AddRectFilled(ImVec2(wp.x+2,wp.y+2), ImVec2(wp.x+width+2,wp.y+height+2), ImColor(0.f,0.f,0.f,notif.alpha*0.25f), 4.f);
                draw_list->AddRectFilled(wp, ImVec2(wp.x+width,wp.y+height), ImColor(0.08f,0.08f,0.08f,notif.alpha*0.9f), 4.f);
                draw_list->AddRect(wp, ImVec2(wp.x+width,wp.y+height), ImColor(0.3f,0.3f,0.3f,notif.alpha), 4.f, 0, 1.5f);
                draw_list->AddRectFilled(wp, ImVec2(wp.x+3.f,wp.y+height), ImColor(0.f,0.74f,0.76f,notif.alpha));
                ImGui::PushClipRect(wp, ImVec2(wp.x+width,wp.y+height), true);
                ImGui::PushTextWrapPos(wp.x + width - padding);
                draw_list->AddText(ImVec2(wp.x+padding,wp.y+padding), ImColor(notif.color.x,notif.color.y,notif.color.z,notif.alpha), notif.message.c_str());
                ImGui::PopTextWrapPos();
                ImGui::PopClipRect();
                current_height -= (height + margin);
            } else {
                float height = text_size.y + padding * 2;
                ImVec2 wp(screen_size.x - width - margin + notif.y_offset, current_height - height);
                draw_list->AddRectFilled(ImVec2(wp.x+2,wp.y+2), ImVec2(wp.x+width+2,wp.y+height+2), ImColor(0.f,0.f,0.f,notif.alpha*0.25f), 4.f);
                draw_list->AddRectFilled(wp, ImVec2(wp.x+width,wp.y+height), ImColor(0.08f,0.08f,0.08f,notif.alpha*0.9f), 4.f);
                draw_list->AddRect(wp, ImVec2(wp.x+width,wp.y+height), ImColor(0.3f,0.3f,0.3f,notif.alpha), 4.f, 0, 1.5f);
                draw_list->AddRectFilled(wp, ImVec2(wp.x+3.f,wp.y+height), ImColor(0.f,0.74f,0.76f,notif.alpha));
                draw_list->AddText(ImVec2(wp.x+padding,wp.y+padding), ImColor(notif.color.x,notif.color.y,notif.color.z,notif.alpha), notif.message.c_str());
                current_height -= (height + margin);
            }
        }
    }
};

inline NotificationSystem g_notification_system;

void notify(const std::string& message, ImVec4 color) {
    g_notification_system.add_notification(message, color, 5.0f);
}
void render_notifications() {
    g_notification_system.render();
}

// ─── Spinner console animation ───────────────────────────────────────────────
void spinner_message(const char* message, int duration_ms = 2000, int interval_ms = 100) {
    const char icons[] = { '/', '-', '\\', '|' };
    int icon_count = sizeof(icons) / sizeof(icons[0]);
    int steps = duration_ms / interval_ms;
    for (int i = 0; i < steps; ++i) {
        printf(oxorany("\r%c %s"), icons[i % icon_count], message);
        fflush(stdout);
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
    }
    printf(oxorany("\r%s\r"), std::string(strlen(message) + 2, ' ').c_str());
}

// ─── DataModel discovery ─────────────────────────────────────────────────────
auto base = 0;

uint64_t GetDataModel() {
    uintptr_t scheduler = read<uintptr_t>(drv::GetBase() + offsets::TaskSchedulerPointer);
    if (!scheduler) return 0;

    uintptr_t jobStart = read<uintptr_t>(scheduler + offsets::JobStart);
    uintptr_t jobEnd   = read<uintptr_t>(scheduler + offsets::JobEnd);
    if (!jobStart || !jobEnd || jobStart >= jobEnd) return 0;

    for (uintptr_t job = jobStart; job < jobEnd; job += 0x10) {
        uintptr_t jobAddress = read<uintptr_t>(job);
        if (!jobAddress) continue;

        std::string jobName = readstring(jobAddress + offsets::Job_Name);
        if (jobName == oxorany("RenderJob")) {
            auto RenderView = read<uintptr_t>(jobAddress + offsets::RenderJobToRenderView);
            if (!RenderView) continue;

            globals::visual_engine = read<uintptr_t>(RenderView + offsets::VisualEngine);
            uintptr_t BaseAddr = drv::GetBase();
            uintptr_t FakeDataModel = read<uintptr_t>(BaseAddr + offsets::FakeDataModelPointer);
            if (!FakeDataModel) continue;

            auto realDataModel = static_cast<uintptr_t>(
                read<std::uint64_t>(FakeDataModel + offsets::FakeDataModelToDataModel));
            if (!realDataModel) continue;

            globals::datamodel = realDataModel;
            return jobAddress;
        }
    }
    return 0;
}

// ─── Background rescan thread ─────────────────────────────────────────────────
void rescan_thread()
{
    std::uint64_t last_place_id = 0;
    if (globals::datamodel)
        last_place_id = read<std::uint64_t>(globals::datamodel + offsets::PlaceId);

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        uintptr_t scheduler = read<uintptr_t>(drv::GetBase() + offsets::TaskSchedulerPointer);
        if (!scheduler) continue;

        uintptr_t jobStart = read<uintptr_t>(scheduler + offsets::JobStart);
        uintptr_t jobEnd   = read<uintptr_t>(scheduler + offsets::JobEnd);
        if (!jobStart || !jobEnd || jobStart >= jobEnd) continue;

        uintptr_t datamodel = 0;

        for (uintptr_t job = jobStart; job < jobEnd; job += 0x10) {
            uintptr_t jobAddress = read<uintptr_t>(job);
            if (!jobAddress) continue;

            std::string jobName = readstring(jobAddress + offsets::Job_Name);
            if (jobName == oxorany("RenderJob")) {
                auto renderView = read<uintptr_t>(jobAddress + offsets::RenderJobToRenderView);
                if (!renderView) continue;
                uintptr_t FakeDataModel = read<uintptr_t>(drv::GetBase() + offsets::FakeDataModelPointer);
                if (!FakeDataModel) continue;
                datamodel = read<std::uint64_t>(FakeDataModel + offsets::FakeDataModelToDataModel);
                if (datamodel) break;
            }
        }

        if (!datamodel) continue;

        std::uint64_t current_place_id = read<std::uint64_t>(datamodel + offsets::PlaceId);
        if (current_place_id == 0) continue;

        if (datamodel != globals::datamodel || current_place_id != last_place_id)
        {
            drv::procid = drv::FindProcess(oxorany("RobloxPlayerBeta.exe"));
            drv::GetBase();
            drv::get_module(oxorany(L"RobloxPlayerBeta.dll"));
            globals::datamodel = datamodel;
            globals::visual_engine = read<uintptr_t>(drv::GetBase() + offsets::VisualEnginePointer);

            uintptr_t players = utils::find_first_child_byclass(globals::datamodel, oxorany("Players"));
            if (!players) continue;

            uintptr_t new_local_player = 0;
            int attempts = 0;
            while (!new_local_player && attempts < 15) {
                new_local_player = read<uintptr_t>(players + offsets::LocalPlayer);
                if (!new_local_player)
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                attempts++;
            }
            if (new_local_player) globals::local_player = new_local_player;
            last_place_id = current_place_id;
        }
    }
}

// ─── Entry point ─────────────────────────────────────────────────────────────
int main()
{
    utils::initialize_console();

    // 1. Auto-update offsets (version-cached, fast after first run)
    offsets::autoupdate();

    // 2. Set console title
    std::string title = oxorany("DeadLock External - https://discord.gg/StZUmwDQnJ - ");
    if (!offsets::RobloxVersionString.empty())
        title += offsets::RobloxVersionString;
    SetConsoleTitleA(title.c_str());

    spinner_message(oxorany("Initializing..."), 1000, 100);

    // 3. Create config folder in %LOCALAPPDATA%\Roblox
    std::string localappdata;
    char* lp = nullptr; size_t sz = 0;
    _dupenv_s(&lp, &sz, oxorany("LOCALAPPDATA"));
    if (lp) { localappdata = lp; free(lp); }
    std::string config_path = localappdata + oxorany("\\Roblox");
    std::filesystem::create_directories(config_path);
    printf(oxorany("Config folder: %s\n"), config_path.c_str());

    spinner_message(oxorany("Waiting for Roblox..."), 500, 100);

    // 4. Wait for Roblox to start
    while (drv::procid == 0) {
        printf(oxorany("Waiting for Roblox to start...\n"));
        drv::procid = drv::FindProcess(oxorany("RobloxPlayerBeta.exe"));
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        system(oxorany("cls"));
    }

    drv::procid = drv::FindProcess(oxorany("RobloxPlayerBeta.exe"));
    if (!drv::Init()) {
        utils::console_print_color(__FILE__, oxorany("Failed to open process handle.\n"));
        return 1;
    }

    auto rbxBase   = drv::GetBase();
    auto rbxModule = drv::get_module(oxorany(L"RobloxPlayerBeta.dll"));

    GetDataModel();

    auto place_id    = read<std::uint64_t>(globals::datamodel + offsets::PlaceId);
    auto local_player = read<uintptr_t>(
        utils::find_first_child_byclass(globals::datamodel, oxorany("Players"))
        + offsets::LocalPlayer);
    globals::local_player = local_player;

    spinner_message(oxorany("Startup complete."), 800, 100);

    // 5. DeadLock key authentication — shown AFTER startup, before overlay opens
    printf(oxorany("\nStartup complete. Awaiting key authentication...\n"));

    if (!ShowKeyDialog()) {
        printf(oxorany("Access denied. Exiting.\n"));
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        return 0;
    }

    printf(oxorany("Access granted. Opening overlay...\n"));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // 6. Start background rescan thread
    std::thread([]() { for (;;) { rescan_thread(); } }).detach();

    // 7. Open overlay (blocks until closed)
    overlay::render();

    return 0;
}
