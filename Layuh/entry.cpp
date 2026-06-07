#include "entry.h"
#include "cheat/aimbot/aimbot.h"
#include "cheat/globals/roblox.h"
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#include <iostream>
#include <string>

// Win32 dialog removed. Replaced by ImGui login screen.


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
    ImColor color;
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
    void add_notification(const std::string& message, ImColor color = ImColor(255,255,255,255), float duration = 3.0f) {
        Notification n;
        n.message = message; n.color = color;
        n.creation_time = GetTickCount() / 1000.0f; n.duration = duration;
        n.alpha = 0.0f; n.y_offset = 0.0f; n.removing = false;
        notifications.push_back(n);
    }

    void render() {
        if (notifications.empty()) return;
        float current_time = GetTickCount() / 1000.0f;
        float screen_w = (float)GetSystemMetrics(SM_CXSCREEN);
        float current_height = (float)GetSystemMetrics(SM_CYSCREEN) - margin;

        for (int i = (int)notifications.size() - 1; i >= 0; i--) {
            auto& notif = notifications[i];
            float elapsed = current_time - notif.creation_time;

            if (elapsed > notif.duration) {
                notifications.erase(notifications.begin() + i); 
                continue; 
            }

            ImVec2 text_size(notif.message.length() * 8.0f, 15.0f);
            float width = text_size.x + padding * 2;
            float height = text_size.y + padding * 2;
            ImVec2 wp(screen_w - width - margin, current_height - height);

            drawingapi::drawing.filled_box(wp, ImVec2(wp.x+width, wp.y+height), ImColor(0, 0, 0, 200));
            drawingapi::drawing.text(notif.message, ImVec2(wp.x+padding, wp.y+padding), notif.color);

            current_height -= (height + margin);
        }
    }
};

inline NotificationSystem g_notification_system;

void notify(const std::string& message, ImColor color) {
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

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

// ─── Entry point ─────────────────────────────────────────────────────────────
int main()
{
    // Allocate a console window so printf/cout work
    AllocConsole();
    FILE* f = nullptr;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONOUT$", "w", stderr);
    freopen_s(&f, "CONIN$",  "r", stdin);
    SetConsoleTitleA("DeadLock External");
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

    // Retry DataModel a few times if not ready yet
    if (!GetDataModel()) {
        printf("DataModel not ready, retrying...\n");
        for (int i = 0; i < 30 && !globals::datamodel; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            GetDataModel();
        }
    }

    if (globals::datamodel) {
        auto place_id = read<std::uint64_t>(globals::datamodel + offsets::PlaceId);
        auto players_service = utils::find_first_child_byclass(globals::datamodel, oxorany("Players"));
        if (players_service) {
            globals::local_player = read<uintptr_t>(players_service + offsets::LocalPlayer);
        }
    }

    spinner_message(oxorany("Startup complete."), 800, 100);

    printf(oxorany("Opening overlay...\n"));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // 6. Start background rescan thread
    std::thread([]() { for (;;) { rescan_thread(); } }).detach();

    // 7. Open overlay (blocks until closed)
    overlay::render();

    return 0;
}
