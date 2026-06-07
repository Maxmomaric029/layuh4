#include "esp.h"
#include < functional >

#include < unordered_map >
#include < string >

ImVec2 screen_center(GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2);

struct PlayerCache {
    uintptr_t character = 0;
    std::unordered_map<std::string, uintptr_t> primitives;
};
static std::unordered_map<uintptr_t, PlayerCache> g_player_cache;

void draw_skeleton(uintptr_t player_instance, uintptr_t character, const matrix& view_matrix, int rig_type, ImColor color)
{
    struct bone_part
    {
        const char* name;
        Vector position;
        Vector2D screen_pos;
        bool valid = false;
    };

    auto get_part_position = [&](uintptr_t player_instance, uintptr_t character, const char* name) -> std::pair<Vector, bool>
        {
            if (character == 0) return { Vector(), false };

            auto& cache = g_player_cache[player_instance];
            if (cache.character != character) {
                cache.character = character;
                cache.primitives.clear();
            }

            uintptr_t primitive = 0;
            auto it = cache.primitives.find(name);
            if (it != cache.primitives.end()) {
                primitive = it->second;
            } else {
                auto part = utils::find_first_child(character, name);
                if (part) {
                    primitive = read<uintptr_t>(part + offsets::Primitive);
                    if (primitive) cache.primitives[name] = primitive;
                }
            }

            if (!primitive) return { Vector(), false };
            Vector pos = read<Vector>(primitive + offsets::Position);
            return { pos, true };
        };

    std::vector<bone_part> bones;
    if (rig_type == 1)
    {
        bones = {
            { "Head", Vector(), Vector2D() },
            { "UpperTorso", Vector(), Vector2D() },
            { "LowerTorso", Vector(), Vector2D() },
            { "LeftUpperArm", Vector(), Vector2D() },
            { "LeftLowerArm", Vector(), Vector2D() },
            { "LeftHand", Vector(), Vector2D() },
            { "RightUpperArm", Vector(), Vector2D() },
            { "RightLowerArm", Vector(), Vector2D() },
            { "RightHand", Vector(), Vector2D() },
            { "LeftUpperLeg", Vector(), Vector2D() },
            { "LeftLowerLeg", Vector(), Vector2D() },
            { "LeftFoot", Vector(), Vector2D() },
            { "RightUpperLeg", Vector(), Vector2D() },
            { "RightLowerLeg", Vector(), Vector2D() },
            { "RightFoot", Vector(), Vector2D() }
        };
    }
    else
    {
        bones = {
            { "Head", Vector(), Vector2D() },
            { "Torso", Vector(), Vector2D() },
            { "Left Arm", Vector(), Vector2D() },
            { "Right Arm", Vector(), Vector2D() },
            { "Left Leg", Vector(), Vector2D() },
            { "Right Leg", Vector(), Vector2D() }
        };
    }

    for (auto& bone : bones)
    {
        auto [pos, valid] = get_part_position(player_instance, character, bone.name);
        if (valid)
        {
            bone.position = pos;
            bone.valid = utils::world_to_screen(bone.position, bone.screen_pos, view_matrix, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
        }
    }

    auto find_bone = [&](const char* name) -> const bone_part*
        {
            for (const auto& bone : bones)
            {
                if (bone.name && (strcmp(bone.name, name) == 0) && bone.valid)
                {
                    return &bone;
                }
            }
            return nullptr;
        };

    const float line_thickness = vars::esp::esp_skeleton_thickness;

    auto draw_bone = [&](const char* from, const char* to)
        {
            const bone_part* bone_from = find_bone(from);
            const bone_part* bone_to = find_bone(to);
            if (bone_from && bone_to)
            {
                drawingapi::drawing.outlined_line(
                    ImVec2(bone_from->screen_pos.x, bone_from->screen_pos.y),
                    ImVec2(bone_to->screen_pos.x, bone_to->screen_pos.y),
                    color,
                    ImColor(0, 0, 0),
                    line_thickness
                );
            }
        };

    if (rig_type == 1)
    {
        draw_bone("Head", "UpperTorso");
        draw_bone("UpperTorso", "LowerTorso");

        draw_bone("UpperTorso", "LeftUpperArm");
        draw_bone("LeftUpperArm", "LeftLowerArm");
        draw_bone("LeftLowerArm", "LeftHand");

        draw_bone("UpperTorso", "RightUpperArm");
        draw_bone("RightUpperArm", "RightLowerArm");
        draw_bone("RightLowerArm", "RightHand");

        draw_bone("LowerTorso", "LeftUpperLeg");
        draw_bone("LeftUpperLeg", "LeftLowerLeg"); // LMAOIIIIIIIOOAOAOOAOAO
        draw_bone("LeftLowerLeg", "LeftFoot");

        draw_bone("LowerTorso", "RightUpperLeg");
        draw_bone("RightUpperLeg", "RightLowerLeg");
        draw_bone("RightLowerLeg", "RightFoot");
    }
    else
    {
        draw_bone("Head", "Torso");
        draw_bone("Torso", "Left Arm");
        draw_bone("Torso", "Right Arm");
        draw_bone("Torso", "Left Leg");
        draw_bone("Torso", "Right Leg");
    }
}

void calc_radar_point(Vector player_pos, int& screenx, int& screeny)
{
    auto workspace = utils::find_first_child_byclass(globals::datamodel, "Workspace");
    if (!workspace)
        return;

    auto camera = read<uintptr_t>(workspace + offsets::Camera);
    if (!camera)
        return;

    Vector camera_pos = read<Vector>(camera + offsets::CameraPos);
    Matrix3 camera_rot = read<Matrix3>(camera + offsets::CameraRotation);

    // Get direction vector from camera to player
    Vector rel_pos = player_pos - camera_pos;

    // Extract camera orientation
    float camera_yaw = atan2f(camera_rot.data[6], camera_rot.data[8]);

    // Radar position and dimensions
    int radar_x = vars::esp::radar_pos_x;
    int radar_y = vars::esp::radar_pos_y;
    int radar_center_x = radar_x + 100;
    int radar_center_y = radar_y + 100;
    float radar_radius = 100.0f;
    float max_range = vars::esp::radar_radius;

    // Calculate 2D distance (ignoring y-axis height)
    float distance_2d = sqrtf(rel_pos.x * rel_pos.x + rel_pos.z * rel_pos.z);

    // Handle the case when player is directly at camera position
    if (distance_2d < 0.1f) {
        screenx = radar_center_x;
        screeny = radar_center_y;
        return;
    }

    // Calculate angle between player and camera forward direction
    float player_angle = atan2f(rel_pos.x, rel_pos.z);

    // Calculate relative angle (this is what we need for the radar)
    float rel_angle = player_angle - camera_yaw;

    // Scale the dot position based on distance, with maximum at radar_radius
    float scaled_distance = distance_2d / max_range * radar_radius;
    if (scaled_distance > radar_radius)
        scaled_distance = radar_radius;

    // Convert polar coordinates (angle & distance) to Cartesian (x,y) for radar
    screenx = radar_center_x + static_cast<int>(sinf(rel_angle) * scaled_distance);
    screeny = radar_center_y - static_cast<int>(cosf(rel_angle) * scaled_distance);
}

bool CESP::draw_radar(matrix view_matrix)
{
    if (!vars::esp::esp_radar)
        return false;

    ImVec2 center = ImVec2(vars::esp::radar_pos_x + 100, vars::esp::radar_pos_y + 100);
    float radius = 100;

    ImColor inner_color = ImColor(25, 25, 25, 230);
    ImColor line_color = vars::esp::radar_color;
    ImColor circle_color = vars::esp::radar_color;
    ImColor border_color = vars::esp::radar_color;

    drawingapi::drawing.circle_filled(center, radius, inner_color);

    float time = GetTickCount() / 1000.0f;
    float pulse_strength = (sinf(time * 2.0f) * 0.15f) + 0.85f;
    
    ImColor glow_color = vars::esp::radar_color;
    glow_color.Value.w = 0.3f * pulse_strength;

    drawingapi::drawing.circle(center, radius * pulse_strength, glow_color, 2.0f);
    drawingapi::drawing.circle(center, radius, border_color, 1.5f);

    float range_circles[] = { 0.33f, 0.66f, 1.0f };
    for (int i = 0; i < 3; ++i)
    {
        float circle_radius = radius * range_circles[i];
        drawingapi::drawing.circle(center, circle_radius, circle_color, 1.0f);
    }

    const int num_lines = 8;
    const char* directions[8] = { "N", "NE", "E", "SE", "S", "SW", "W", "NW" };

    for (int i = 0; i < num_lines; ++i)
    {
        float angle = (3.14159f * 2.0f * i) / num_lines;
        ImVec2 start = ImVec2(center.x + cosf(angle) * (radius * 0.1f), center.y + sinf(angle) * (radius * 0.1f));
        ImVec2 end = ImVec2(center.x + cosf(angle) * radius, center.y + sinf(angle) * radius);
        drawingapi::drawing.line(start, end, line_color, 1.0f);

        ImVec2 text_pos = ImVec2(
            center.x + cosf(angle) * (radius * 1.1f) - 5,
            center.y + sinf(angle) * (radius * 1.1f) - 5
        );

        drawingapi::drawing.text(directions[i], ImVec2(text_pos.x + 1, text_pos.y + 1), ImColor(0, 0, 0, 255));
        drawingapi::drawing.text(directions[i], text_pos, line_color);
    }

    auto workspace = utils::find_first_child_byclass(globals::datamodel, "Workspace");
    if (!workspace)
        return false;

    auto camera = read<uintptr_t>(workspace + offsets::Camera);
    if (!camera)
        return false;

    Matrix3 camera_rot = read<Matrix3>(camera + offsets::CameraRotation);

    float camera_yaw = atan2f(camera_rot.data[6], camera_rot.data[8]);
    camera_yaw = -camera_yaw;

    float cone_width = 0.3f;

    ImVec2 left_beam = ImVec2(
        center.x + cosf(camera_yaw - cone_width) * radius,
        center.y + sinf(camera_yaw - cone_width) * radius
    );

    ImVec2 right_beam = ImVec2(
        center.x + cosf(camera_yaw + cone_width) * radius,
        center.y + sinf(camera_yaw + cone_width) * radius
    );

    ImColor beam_color = vars::esp::radar_color;
    beam_color.Value.w = 0.5f * pulse_strength;

    drawingapi::drawing.line(center, left_beam, beam_color, 1.5f);
    drawingapi::drawing.line(center, right_beam, beam_color, 1.5f);

    ImColor fill_beam_color = vars::esp::radar_color;
    fill_beam_color.Value.w = 0.15f * pulse_strength;
    drawingapi::drawing.triangle_filled(center, left_beam, right_beam, fill_beam_color);

    float player_circle_radius = 4.0f;
    ImColor player_color = ImColor(255, 255, 255, 255);
    drawingapi::drawing.circle_filled(center, player_circle_radius, player_color);

    auto get_part_position = [](uintptr_t character, const char* name) -> std::pair<Vector, bool>
        {
            if (character == 0)
                return { Vector(), false };

            auto part = utils::find_first_child(character, name);
            if (!part)
                return { Vector(), false };

            auto primitive = read<uintptr_t>(part + offsets::Primitive);
            if (!primitive)
                return { Vector(), false };

            Vector pos = read<Vector>(primitive + offsets::Position);
            return { pos, true };
        };

    auto local_character = utils::get_model_instance(globals::local_player);
    if (!local_character)
        return false;

    auto [local_pos, local_valid] = get_part_position(local_character, "HumanoidRootPart");
    if (!local_valid)
        return false;

    int players_displayed = 0;

    static auto last_time = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsed = now - last_time;
    static std::vector<uintptr_t> player_instances;

    if (elapsed.count() >= 0.5f)
    {
        last_time = now;
        player_instances = utils::get_players(globals::datamodel);
    }

    for (auto& player_instance : player_instances)
    {
        if (!player_instance || player_instance == globals::local_player)
            continue;

        if (globals::local_player)
        {
            int player_team = read<int>(player_instance + offsets::Team);
            int local_team = read<int>(globals::local_player + offsets::Team);

            if (player_team == local_team && vars::esp::team_check)
                continue;
        }

        auto character = utils::get_model_instance(player_instance);
        if (!character)
            continue;

        auto humanoid = utils::find_first_child_byclass(character, "Humanoid");
        if (!humanoid)
            continue;

        float health = read<float>(humanoid + offsets::Health);
        if (health <= 0.0f)
            continue;

        auto [player_pos, player_valid] = get_part_position(character, "HumanoidRootPart");
        if (!player_valid)
            continue;

        int screen_x, screen_y;
        calc_radar_point(player_pos, screen_x, screen_y);

        float dist_to_player = sqrt(
            (player_pos.x - local_pos.x) * (player_pos.x - local_pos.x) +
            (player_pos.y - local_pos.y) * (player_pos.y - local_pos.y) +
            (player_pos.z - local_pos.z) * (player_pos.z - local_pos.z)
        );

        float dot_size = 5.0f;
        ImColor player_dot_color = vars::esp::box_color;

        drawingapi::drawing.circle_filled(
            ImVec2((float)screen_x, (float)screen_y),
            dot_size,
            player_dot_color
        );

        float glow_pulse = sinf(time * 3.0f + dist_to_player * 0.05f) * 0.7f;
        ImColor glow_dot_color = player_dot_color;
        glow_dot_color.Value.w = 0.5f * pulse_strength;

        drawingapi::drawing.circle(
            ImVec2((float)screen_x, (float)screen_y),
            dot_size + 1.0f + glow_pulse,
            glow_dot_color,
            1.0f
        );

        if (vars::esp::radar_show_names)
        {
            std::string player_name = utils::get_instance_name(player_instance);
            ImVec2 name_pos = ImVec2((float)screen_x - 10, (float)screen_y - dot_size - 15 - 2);

            drawingapi::drawing.text(player_name, ImVec2(name_pos.x + 1, name_pos.y + 1), ImColor(0, 0, 0, 255));
            drawingapi::drawing.text(player_name, name_pos, vars::esp::radar_names_color);
        }

        if (vars::esp::radar_show_distance)
        {
            int distance = static_cast<int>(dist_to_player);
            std::string dist_text = std::to_string(distance) + "m";
            ImVec2 dist_pos = ImVec2((float)screen_x - 10, (float)screen_y + dot_size + 2);

            drawingapi::drawing.text(dist_text, ImVec2(dist_pos.x + 1, dist_pos.y + 1), ImColor(0, 0, 0, 255));
            drawingapi::drawing.text(dist_text, dist_pos, vars::esp::radar_distance_color);
        }

        players_displayed++;
    }

    if (vars::esp::radar_show_debug)
    {
        std::string debug_text = "Players: " + std::to_string(players_displayed);
        ImVec2 text_pos = ImVec2(vars::esp::radar_pos_x + 5, vars::esp::radar_pos_y + radius * 2 + 5);
        drawingapi::drawing.text(debug_text, ImVec2(text_pos.x + 1, text_pos.y + 1), ImColor(0, 0, 0, 255));
        drawingapi::drawing.text(debug_text, text_pos, ImColor(255, 255, 255, 255));
    }

    return true;
}

bool CESP::DrawArrow(ImVec2 center, Vector2D targetScreenPos, float radius, ImColor color, bool outlined)
{
    Vector2D direction(targetScreenPos.x - center.x, targetScreenPos.y - center.y);
    float distance = sqrt(direction.x * direction.x + direction.y * direction.y);
    if (distance < 0.001f) return false;

    Vector2D normalizedDir(direction.x / distance, direction.y / distance);
    float angle = atan2f(normalizedDir.y, normalizedDir.x);

    Vector2D arrowPos(center.x + radius * normalizedDir.x, center.y + radius * normalizedDir.y);

    float arrowSize = 16.0f;
    ImVec2 p1(arrowPos.x, arrowPos.y);
    ImVec2 p2(arrowPos.x - arrowSize * cosf(angle - 0.5f), arrowPos.y - arrowSize * sinf(angle - 0.5f));
    ImVec2 p3(arrowPos.x - arrowSize * cosf(angle + 0.5f), arrowPos.y - arrowSize * sinf(angle + 0.5f));

    drawingapi::drawing.triangle_filled(p1, p2, p3, color);

    return true;
}

bool CESP::draw_players(matrix view_matrix)
{
    if (!vars::esp::esp_enabled)
        return false;
    if (!globals::datamodel)
        return false;
    static auto last_time = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsed = now - last_time;
    static std::vector<uintptr_t> player_instances;
    if (elapsed.count() >= 1.f)
    {
        last_time = now;
        auto new_player_instances = utils::get_players(globals::datamodel);
        if (!new_player_instances.empty() && (player_instances != new_player_instances))
        {
            player_instances = new_player_instances;
        }
    }
    auto get_part_position = [](uintptr_t player_instance, uintptr_t character, const char* name) -> std::pair<Vector, bool>
        {
            if (character == 0) return { Vector(), false };

            auto& cache = g_player_cache[player_instance];
            if (cache.character != character) {
                cache.character = character;
                cache.primitives.clear();
            }

            uintptr_t primitive = 0;
            auto it = cache.primitives.find(name);
            if (it != cache.primitives.end()) {
                primitive = it->second;
            } else {
                auto part = utils::find_first_child(character, name);
                if (part) {
                    primitive = read<uintptr_t>(part + offsets::Primitive);
                    if (primitive) cache.primitives[name] = primitive;
                }
            }

            if (!primitive) return { Vector(), false };
            Vector pos = read<Vector>(primitive + offsets::Position);
            return { pos, true };
        };
    auto get_rig_type = [](uintptr_t character) -> int
        {
            if (character == 0)
                return -1;
            if (utils::find_first_child(character, "UpperTorso"))
                return 1;
            else if (utils::find_first_child(character, "Torso"))
                return 0;
            return -1;
        };
    ImVec2 screen_size(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    ImVec2 screen_center(screen_size.x / 2, screen_size.y / 2);
    for (auto& player_instance : player_instances)
    {
        if (player_instance == 0)
            continue;
        if (!vars::esp::esp_local && player_instance == globals::local_player)
            continue;
        std::string player_name = utils::get_instance_name(player_instance);
        bool is_whitelisted = vars::whitelist::users.count(player_name) > 0;
        bool is_target = vars::whitelist::targets.count(player_name) > 0;
        ImColor green_color = ImColor(0, 255, 0, 255);
        ImColor box_color = is_target ? ImColor(255, 0, 0, 255) : (is_whitelisted ? green_color : vars::esp::box_color);
        ImColor fill_box_color = is_target ? ImColor(255, 0, 0, 100) : (is_whitelisted ? ImColor(0, 255, 0, 100) : vars::esp::fill_box_color);
        ImColor name_color = is_target ? ImColor(255, 0, 0, 255) : (is_whitelisted ? green_color : vars::esp::name_color);
        ImColor skeleton_color = is_target ? ImColor(255, 0, 0, 255) : (is_whitelisted ? green_color : vars::esp::skeleton_color);
        ImColor health_bar_color = is_target ? ImColor(255, 0, 0, 255) : (is_whitelisted ? green_color : vars::esp::health_bar_color);
        ImColor oof_arrow_color = is_target ? ImColor(255, 0, 0, 255) : (is_whitelisted ? green_color : vars::esp::oof_arrow_color);
        ImColor distance_color = is_target ? ImColor(255, 0, 0, 255) : (is_whitelisted ? green_color : vars::esp::distance_color);

        auto character = utils::get_model_instance(player_instance);
        if (!character)
            continue;
        auto humanoid = utils::find_first_child_byclass(character, "Humanoid");
        if (!humanoid)
            continue;
        if (globals::local_player)
        {
            int player_team = read<int>(player_instance + offsets::Team);
            int local_team = read<int>(globals::local_player + offsets::Team);
            if (player_team == local_team && vars::esp::team_check)
                continue;
        }
        float health = read<float>(humanoid + offsets::Health);
        if (health <= 0.0f)
            continue;
        int rig_type = get_rig_type(character);
        if (rig_type == -1)
            continue;
        if (vars::esp::esp_skeleton)
        {
            draw_skeleton(player_instance, character, view_matrix, rig_type, skeleton_color);
        }
        Vector head_pos, mid_pos, bot_pos;
        Vector2D head_screen, mid_screen, bot_screen;
        bool head_valid = false, mid_valid = false, bot_valid = false;
        if (vars::esp::esp_mode == 1)
        {
            auto head_result = get_part_position(player_instance, character, "Head");
            if (!head_result.second)
                continue;
            head_pos = head_result.first;
            head_valid = utils::world_to_screen(head_pos, head_screen, view_matrix, screen_size.x, screen_size.y);
            if (rig_type == 1)
            {
                auto mid_result = get_part_position(player_instance, character, "LowerTorso");
                if (mid_result.second)
                {
                    mid_pos = mid_result.first;
                    mid_valid = utils::world_to_screen(mid_pos, mid_screen, view_matrix, screen_size.x, screen_size.y);
                }
                auto bot_result = get_part_position(player_instance, character, "LeftFoot");
                if (bot_result.second)
                {
                    bot_pos = bot_result.first;
                    bot_valid = utils::world_to_screen(bot_pos, bot_screen, view_matrix, screen_size.x, screen_size.y);
                }
            }
            else
            {
                auto mid_result = get_part_position(player_instance, character, "Torso");
                if (mid_result.second)
                {
                    mid_pos = mid_result.first;
                    mid_valid = utils::world_to_screen(mid_pos, mid_screen, view_matrix, screen_size.x, screen_size.y);
                }
                auto bot_result = get_part_position(player_instance, character, "Left Leg");
                if (bot_result.second)
                {
                    bot_pos = bot_result.first;
                    bot_valid = utils::world_to_screen(bot_pos, bot_screen, view_matrix, screen_size.x, screen_size.y);
                }
            }
        }
        else if (vars::esp::esp_mode == 0)
        {
            auto head_result = get_part_position(player_instance, character, "Head");
            if (!head_result.second)
                continue;
            head_pos = head_result.first + Vector(0, 1.0f, 0);
            head_valid = utils::world_to_screen(head_pos, head_screen, view_matrix, screen_size.x, screen_size.y);
            mid_pos = head_result.first - Vector(0, 2.0f, 0);
            mid_valid = utils::world_to_screen(mid_pos, mid_screen, view_matrix, screen_size.x, screen_size.y);
            bot_pos = head_result.first - Vector(0, 5.0f, 0);
            bot_valid = utils::world_to_screen(bot_pos, bot_screen, view_matrix, screen_size.x, screen_size.y);
        }
        bool is_off_screen = !head_valid || head_screen.x < 0 || head_screen.x > screen_size.x || head_screen.y < 0 || head_screen.y > screen_size.y;
        if (vars::esp::esp_oof_arrows && is_off_screen)
        {
            Vector2D arrow_target = head_screen;
            if (!head_valid)
            {
                float w = view_matrix.m[3][0] * head_pos.x + view_matrix.m[3][1] * head_pos.y + view_matrix.m[3][2] * head_pos.z + view_matrix.m[3][3];
                float inv_w = 1.0f / (w == 0.0f ? 0.001f : w);
                float x = (view_matrix.m[0][0] * head_pos.x + view_matrix.m[0][1] * head_pos.y + view_matrix.m[0][2] * head_pos.z + view_matrix.m[0][3]) * inv_w;
                float y = (view_matrix.m[1][0] * head_pos.x + view_matrix.m[1][1] * head_pos.y + view_matrix.m[1][2] * head_pos.z + view_matrix.m[1][3]) * inv_w;
                arrow_target.x = (screen_size.x * 0.5f) * (x + 1.0f);
                arrow_target.y = (screen_size.y * 0.5f) * (1.0f - y);
                arrow_target.x = screen_center.x - (arrow_target.x - screen_center.x);
                arrow_target.y = screen_center.y - (arrow_target.y - screen_center.y);
            }
            DrawArrow(screen_center, arrow_target, vars::aimbot::fov_value + 20.0f, oof_arrow_color, false);
        }
        if (!head_valid)
            continue;
        float box_height = (bot_valid ? bot_screen.y : (mid_valid ? mid_screen.y : head_screen.y + 50.0f)) - head_screen.y;
        float box_width = box_height / 2.0f;
        if (vars::esp::esp_mode == 0)
            box_width *= 1.35f;
        ImVec2 box_top_left(head_screen.x - box_width / 2, head_screen.y);
        ImVec2 box_bottom_right(head_screen.x + box_width / 2, bot_valid ? bot_screen.y : (mid_valid ? mid_screen.y : head_screen.y + box_height));
        if (vars::esp::esp_fill_box)
        {
            drawingapi::drawing.filled_box(box_top_left, box_bottom_right, fill_box_color);
        }
        if (vars::esp::box_glow)
        {
            ImColor base_col = box_color;
            ImColor glow_col = ImColor(base_col.Value.x, base_col.Value.y, base_col.Value.z, 1.0f);
            const int glow_layers = 18;
            const float max_glow = 14.0f;
            const float min_glow = -7.0f;
            const float glow_step = (max_glow - min_glow) / (glow_layers - 1);
            for (int i = 0; i < glow_layers; ++i)
            {
                float glow = min_glow + i * glow_step;
                float t = (float)i / (glow_layers - 1);
                float alpha = 0.18f * powf(1.0f - t, 2.5f);
                ImColor layer_col = glow_col;
                layer_col.Value.w = alpha;
                drawingapi::drawing.outlined_box(
                    ImVec2(box_top_left.x - glow, box_top_left.y - glow),
                    ImVec2(box_bottom_right.x + glow, box_bottom_right.y + glow),
                    layer_col, ImColor(0, 0, 0, 0), 1.0f + glow * 0.13f
                );
            }
        }
        if (vars::esp::esp_box) {
            drawingapi::drawing.outlined_box(box_top_left, box_bottom_right, box_color, ImColor(0, 0, 0), 1.0f);
        }
        health = std::clamp(health, 0.0f, 100.0f);
        ImColor health_color;
        if (is_target)
            health_color = ImColor(255, 0, 0, 255);
        else if (is_whitelisted)
            health_color = green_color;
        else if (health >= 75.0f)
            health_color = ImColor(0, 255, 0);
        else if (health >= 30.0f)
            health_color = ImColor(255, 255, 0);
        else
            health_color = ImColor(255, 0, 0);
        float health_bar_width = 2.2f;
        float health_bar_height = box_height * (health / 100.0f);
        ImVec2 health_bar_bottom_left(box_top_left.x - health_bar_width - 3.5f, box_top_left.y + box_height);
        ImVec2 health_bar_top_right(health_bar_bottom_left.x + health_bar_width, health_bar_bottom_left.y - health_bar_height);
        if (vars::esp::esp_health_bar)
        {
            drawingapi::drawing.outlined_box(
                ImVec2(health_bar_bottom_left.x, box_top_left.y),
                ImVec2(health_bar_top_right.x, health_bar_bottom_left.y),
                ImColor(0, 0, 0, 255),
                ImColor(0, 0, 0, 255),
                1.0f
            );
            drawingapi::drawing.filled_box(health_bar_top_right, health_bar_bottom_left, health_bar_color);
            std::string health_text = std::to_string(static_cast<int>(health));
            ImVec2 text_size(20.0f, 15.0f); // Approximate size
            ImVec2 health_text_pos(
                health_bar_bottom_left.x + (health_bar_width - text_size.x) * 0.5f,
                health_bar_top_right.y - (text_size.y * 0.5f)
            );
            if (vars::esp::esp_health_text && static_cast<int>(health) != 100)
            {
                drawingapi::drawing.text(health_text, health_text_pos, vars::esp::health_text_color);
            }
        }
        if (vars::esp::esp_name)
        {
            if (!player_name.empty())
            {
                ImVec2 text_size(player_name.length() * 8.0f, 15.0f); // Approximate
                float text_x = head_screen.x - (text_size.x / 2);
                float text_y = head_screen.y - 15;
                drawingapi::drawing.text(player_name.c_str(), ImVec2(text_x, text_y), name_color);
            }
        }
        auto local_character = utils::get_model_instance(globals::local_player);
        if (local_character)
        {
            if (vars::esp::esp_distance)
            {
                auto local_root_part = utils::find_first_child(local_character, "HumanoidRootPart");
                auto player_root_part = utils::find_first_child(character, "HumanoidRootPart");
                if (local_root_part && player_root_part)
                {
                    auto local_primitive = read<uintptr_t>(local_root_part + offsets::Primitive);
                    auto player_primitive = read<uintptr_t>(player_root_part + offsets::Primitive);
                    if (local_primitive && player_primitive)
                    {
                        Vector local_pos = read<Vector>(local_primitive + offsets::Position);
                        Vector player_pos = read<Vector>(player_primitive + offsets::Position);
                        float distance = (local_pos - player_pos).Length();
                        if (distance > 0.0f)
                        {
                            std::string distance_str = "" + std::to_string(static_cast<int>(distance)) + "m";
                            ImVec2 dist_text_size(distance_str.length() * 8.0f, 15.0f); // Approximate
                            float dist_text_x = (box_top_left.x + box_bottom_right.x) / 2 - (dist_text_size.x / 2);
                            float dist_text_y = box_bottom_right.y + 2;
                            drawingapi::drawing.text(distance_str.c_str(), ImVec2(dist_text_x, dist_text_y), distance_color);
                        }
                    }
                }
            }
        }
        if (vars::esp::esp_tracer)
        {
            ImVec2 tracer_start_pos, tracer_end_pos;
            if (vars::esp::tracer_start == 0)
                tracer_start_pos = ImVec2(screen_size.x / 2, 0);
            else if (vars::esp::tracer_start == 1)
                tracer_start_pos = ImVec2(screen_size.x / 2, screen_size.y / 2);
            else
                tracer_start_pos = ImVec2(screen_size.x / 2, screen_size.y);
            if (vars::esp::tracer_end == 0 && head_valid)
                tracer_end_pos = ImVec2(head_screen.x, head_screen.y);
            else if (vars::esp::tracer_end == 1 && mid_valid)
                tracer_end_pos = ImVec2(mid_screen.x, mid_screen.y);
            else if (vars::esp::tracer_end == 2 && bot_valid)
                tracer_end_pos = ImVec2(bot_screen.x, bot_screen.y);
            else if (head_valid)
                tracer_end_pos = ImVec2(head_screen.x, head_screen.y);
            else
                continue;
            drawingapi::drawing.outlined_line(tracer_start_pos, tracer_end_pos, box_color, ImColor(0, 0, 0), vars::esp::tracer_thickness);
        }
    }
    if (globals::teleport_target && vars::misc::teleport_arrow)
    {
        auto teleport_character = utils::get_model_instance(globals::teleport_target);
        if (!teleport_character)
            return true;
        std::string player_name = utils::get_instance_name(globals::teleport_target);
        if (player_name.empty())
            return true;
        ImVec2 text_size(player_name.length() * 8.0f, 15.0f);
        Vector head_pos;
        Vector2D head_screen;
        bool head_valid = false;
        if (vars::esp::esp_mode == 1)
        {
            auto head_result = get_part_position(globals::teleport_target, teleport_character, "Head");
            if (!head_result.second)
                return true;
            head_pos = head_result.first;
            head_valid = utils::world_to_screen(head_pos, head_screen, view_matrix, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
        }
        else
        {
            auto head_result = get_part_position(globals::teleport_target, teleport_character, "Head");
            if (!head_result.second)
                return true;
            head_pos = head_result.first + Vector(0, 1.0f, 0);
            head_valid = utils::world_to_screen(head_pos, head_screen, view_matrix, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
        }
        if (!head_valid)
            return true;
        float name_margin = std::max(15.0f, text_size.y * 1.5f);
        float name_text_y = head_screen.y - name_margin;
        const float arrow_size = 11.0f;
        ImVec2 arrow_tip(head_screen.x, name_text_y + arrow_size);
        ImVec2 arrow_base_left(head_screen.x - arrow_size, name_text_y);
        ImVec2 arrow_base_right(head_screen.x + arrow_size, name_text_y);
        const float outline_thickness = 2.0f;
        drawingapi::drawing.triangle_filled(
            arrow_tip,
            arrow_base_left,
            arrow_base_right,
            ImColor(0, 0, 0, 255)
        );
        ImVec2 inner_tip(head_screen.x, name_text_y + arrow_size - outline_thickness);
        ImVec2 inner_base_left(head_screen.x - arrow_size + outline_thickness, name_text_y + outline_thickness);
        ImVec2 inner_base_right(head_screen.x + arrow_size - outline_thickness, name_text_y + outline_thickness);
        drawingapi::drawing.triangle_filled(
            inner_tip,
            inner_base_left,
            inner_base_right,
            vars::misc::teleport_arrow_color
        );
    }
    return true;
}
