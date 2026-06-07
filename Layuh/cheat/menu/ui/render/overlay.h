#pragma once
#define d_toggle_bind 0x2D /* VK_INSERT */

namespace overlay
{
    inline void setup_style()
    {
        ImGui::StyleColorsDark();
        ImGuiStyle* style = &ImGui::GetStyle();
        style->Colors[ImGuiCol_WindowBg] = ImColor(15, 17, 23, 245);
        style->Colors[ImGuiCol_Border] = ImColor(45, 45, 45, 255);
        style->Colors[ImGuiCol_Separator] = ImColor(90, 24, 154);
        style->Colors[ImGuiCol_SeparatorHovered] = ImColor(90, 24, 154);
        style->Colors[ImGuiCol_SeparatorActive] = ImColor(90, 24, 154);
        style->Colors[ImGuiCol_ChildBg] = ImColor(10, 11, 15, 255);
        style->Colors[ImGuiCol_FrameBg] = ImColor(25, 25, 30, 255);
        style->Colors[ImGuiCol_PopupBg] = ImColor(15, 17, 23, 255);
        style->Colors[ImGuiCol_Text] = ImColor(199, 199, 199, 255);
        style->Colors[ImGuiCol_Header] = ImColor(0, 0, 0, 0);
        style->Colors[ImGuiCol_HeaderHovered] = ImColor(0, 0, 0, 0);
        style->Colors[ImGuiCol_HeaderActive] = ImColor(0, 0, 0, 0);
        style->Colors[ImGuiCol_Button] = ImColor(25, 25, 30, 255);
        style->Colors[ImGuiCol_ButtonActive] = ImColor(25, 25, 30, 255);
        style->Colors[ImGuiCol_ButtonHovered] = ImColor(25, 25, 30, 255);
        style->Colors[ImGuiCol_CheckMark] = ImColor(90, 24, 154);
        style->Colors[ImGuiCol_SliderGrab] = ImColor(90, 24, 154);
        style->WindowTitleAlign = { 0.5f, 0.5f };
        style->WindowRounding = 6.0f;
        style->ChildRounding = 6.0f;
        style->FrameRounding = 6.0f;
        style->WindowBorderSize = 1.0f;
        style->FrameBorderSize = 1.0f;
        style->ChildBorderSize = 1.0f;
        style->AntiAliasedFill = true;
        style->AntiAliasedLines = true;
        style->Alpha = 1.0f;
    }


	void render();

	inline bool enabled = true;
}