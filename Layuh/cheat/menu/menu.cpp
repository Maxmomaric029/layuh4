#include "menu.h"
#include "../../cheat/render/ui_framework.h"
#include "../../cheat/aimbot/aimbot.h"
#include "../../cheat/esp/esp.h"
#include "../../cheat/misc/misc.h"
#include "../../cheat/images/image_handler.h"
#include <iostream>

bool logged_in = false;

static float menu_x = 100.0f;
static float menu_y = 100.0f;
static int active_tab = 0;

void CMenu::draw_keybinds() {}
void CMenu::draw_watermark() {}
void CMenu::draw_instance_tree(uintptr_t instance) {}
void CMenu::draw_instance_explorer() {}

bool CMenu::draw_menu() {
    if (!ui::menu_open) return false;

    if (!logged_in) {
        ui::set_next_window_size(350, 200);
        if (ui::begin_window("Login DeadLock", menu_x, menu_y)) {
            ui::label("Verify your access token below:");
            if (ui::button("Login with Auth", 310, 40)) {
                logged_in = true;
            }
            ui::end_window();
        }
        return true;
    }

    ui::set_next_window_size(800, 600);
    if (ui::begin_window("DeadLock Premium", menu_x, menu_y)) {
        
        // Sidebar tabs
        ui::begin_group_box("Navigation", 200, 510);
        if (ui::tab("Aimbot", active_tab == 0, 160, 45)) active_tab = 0;
        if (ui::tab("Visuals", active_tab == 1, 160, 45)) active_tab = 1;
        if (ui::tab("Misc", active_tab == 2, 160, 45)) active_tab = 2;
        ui::end_group_box();

        // Main content area
        ui::set_cursor(menu_x + 230, menu_y + 70); // Move to right of sidebar

        if (active_tab == 0) {
            ui::begin_group_box("Aimbot Settings", 540, 510);
            ui::toggle("Enable Aimbot", vars::aimbot::aimbot_enabled);
            ui::toggle("Draw FOV", vars::aimbot::fov_circle);
            ui::toggle("Target Line", vars::aimbot::target_line);
            ui::slider_float("FOV Size", vars::aimbot::fov_value, 10.0f, 500.0f);
            ui::slider_float("Smoothness", vars::aimbot::aimbot_smoothing, 1.0f, 10.0f);
            ui::end_group_box();
        } 
        else if (active_tab == 1) {
            ui::set_cursor(menu_x + 230, menu_y + 70);
            ui::begin_group_box("ESP Settings", 540, 510);
            ui::toggle("Master Switch", vars::esp::esp_box);
            ui::same_line(); ui::toggle("Filled Box", vars::esp::esp_fill_box);
            ui::toggle("Show Name", vars::esp::esp_name);
            ui::same_line(); ui::toggle("Show Health", vars::esp::esp_health_bar);
            ui::toggle("Show Distance", vars::esp::esp_distance);
            ui::toggle("Show Skeleton", vars::esp::esp_skeleton);
            ui::slider_float("Render Distance", vars::esp::esp_max_distance, 0.0f, 10000.0f);
            ui::end_group_box();
        }
        else if (active_tab == 2) {
            ui::set_cursor(menu_x + 230, menu_y + 70);
            ui::begin_group_box("Miscellaneous", 540, 510);
            ui::toggle("Fly Hack", vars::misc::fly_enabled);
            ui::slider_float("Fly Speed", vars::misc::fly_speed, 1.0f, 250.0f);
            ui::toggle("Speed Hack", vars::misc::speed_hack_enabled);
            ui::slider_float("Walk Speed", vars::misc::speed_multiplier, 1.0f, 10.0f);
            ui::toggle("Instant Prompt", vars::misc::instant_prompt_enabled);
            ui::end_group_box();
        }

        ui::end_window();
    }

    return true;
}
