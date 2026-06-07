#pragma once

#include "entry.h"

class CMenu {
public:
	void draw_keybinds();
	bool draw_menu();
	void draw_instance_tree(uintptr_t instance);
	void draw_watermark();
	void draw_instance_explorer();
private:
	std::unordered_map<uintptr_t, std::vector<uintptr_t>> cache;
	std::chrono::steady_clock::time_point last_refresh_time;
	const std::chrono::seconds refresh_interval{ 1 };
    std::unordered_map<std::string, ID3D11ShaderResourceView*> class_icons;
};

extern bool logged_in;
namespace d { inline CMenu menu; }
