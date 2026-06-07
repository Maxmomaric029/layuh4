#include "menu.h"
#include "../../cheat/render/ui_framework.h"
#include "../../cheat/aimbot/aimbot.h"
#include "../../cheat/esp/esp.h"
#include "../../cheat/misc/misc.h"
#include "../../cheat/images/image_handler.h"
#include <iostream>

bool logged_in = false;

// Variables basicas para el arrastre de ventana del menu
static float menu_x = 100.0f;
static float menu_y = 100.0f;
static float menu_w = 400.0f;
static float menu_h = 300.0f;

void CMenu::draw_keybinds() {
    // Implementar si es necesario
}

void CMenu::draw_watermark() {
    // Implementar si es necesario
}

void CMenu::draw_instance_tree(uintptr_t instance) {
    // Implementar explorer en D2D si es necesario
}

void CMenu::draw_instance_explorer() {
    // Implementar explorer en D2D si es necesario
}

bool CMenu::draw_menu() {
    if (!ui::menu_open) return false;

    if (!logged_in) {
        if (ui::begin_window("Login DeadLock", menu_x, menu_y, 300, 150)) {
            ui::label("Por favor verifica tu acceso", menu_x + 20, menu_y + 50);
            if (ui::button("Login (Auto)", menu_x + 20, menu_y + 90, 260, 30)) {
                logged_in = true; // Auto login por ahora
            }
            ui::end_window();
        }
        return true;
    }

    if (ui::begin_window("DeadLock - Puro DX11", menu_x, menu_y, menu_w, menu_h)) {
        
        float current_y = menu_y + 50.0f;
        float x_offset = menu_x + 20.0f;

        ui::label("--- Aimbot ---", x_offset, current_y); current_y += 30.0f;
        ui::checkbox("Enable Aimbot", vars::aimbot::aimbot_enabled, x_offset, current_y); current_y += 25.0f;
        ui::checkbox("Draw FOV Circle", vars::aimbot::fov_circle, x_offset, current_y); current_y += 35.0f;

        ui::label("--- Visuals ---", x_offset, current_y); current_y += 30.0f;
        ui::checkbox("Enable ESP", vars::esp::box, x_offset, current_y); current_y += 25.0f;
        ui::checkbox("Show Name", vars::esp::name, x_offset, current_y); current_y += 25.0f;
        ui::checkbox("Show Distance", vars::esp::distance, x_offset, current_y); current_y += 35.0f;

        ui::label("--- Misc ---", x_offset, current_y); current_y += 30.0f;
        ui::checkbox("Fly Hack", vars::misc::fly_enabled, x_offset, current_y); current_y += 25.0f;
        ui::checkbox("Speed Hack", vars::misc::speed_hack_enabled, x_offset, current_y); current_y += 25.0f;

        ui::end_window();
    }

    return true;
}
