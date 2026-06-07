#include "image_handler.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

LPDIRECT3DTEXTURE9 LoadTextureFromMemory(LPDIRECT3DDEVICE9 pDevice, const std::vector<uint8_t>& data) {
    if (data.empty()) return nullptr; 

    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* image_data = stbi_load_from_memory(data.data(), static_cast<int>(data.size()), &width, &height, &channels, 4);
    if (!image_data) return nullptr;

    LPDIRECT3DTEXTURE9 texture = nullptr;
    HRESULT hr = pDevice->CreateTexture(width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture, nullptr);
    if (FAILED(hr)) {
        stbi_image_free(image_data);
        return nullptr;
    }

    D3DLOCKED_RECT rect;
    if (SUCCEEDED(texture->LockRect(0, &rect, nullptr, 0))) {
        unsigned char* dest = static_cast<unsigned char*>(rect.pBits);
        for (int y = 0; y < height; ++y) {
            unsigned char* src_row = image_data + y * width * 4;
            unsigned char* dest_row = dest + y * rect.Pitch;
            for (int x = 0; x < width; ++x) {
                // Convert RGBA to BGRA
                dest_row[x * 4 + 0] = src_row[x * 4 + 2]; // B
                dest_row[x * 4 + 1] = src_row[x * 4 + 1]; // G
                dest_row[x * 4 + 2] = src_row[x * 4 + 0]; // R
                dest_row[x * 4 + 3] = src_row[x * 4 + 3]; // A
            }
        }
        texture->UnlockRect(0);
    } else {
        texture->Release();
        texture = nullptr;
    }

    stbi_image_free(image_data);
    return texture;
}

void CImageHandler::create_images() {
    if (globals::images_init) return;

    workspace_image = LoadTextureFromMemory(dx_device, workspace_image_data);
    part_image = LoadTextureFromMemory(dx_device, part_image_data);
    model_image = LoadTextureFromMemory(dx_device, model_image_data);
    folder_image = LoadTextureFromMemory(dx_device, folder_image_data);
    camera_image = LoadTextureFromMemory(dx_device, camera_image_data);
    script_image = LoadTextureFromMemory(dx_device, script_image_data);
    local_script_image = LoadTextureFromMemory(dx_device, local_script_image_data);
    players_image = LoadTextureFromMemory(dx_device, players_image_data);
    humanoid_image = LoadTextureFromMemory(dx_device, humanoid_image_data);
    accessory_image = LoadTextureFromMemory(dx_device, accessory_image_data);
    sound_image = LoadTextureFromMemory(dx_device, sound_image_data);
    replicated_storage_image = LoadTextureFromMemory(dx_device, replicated_storage_image_data);
    hat_image = LoadTextureFromMemory(dx_device, hat_image_data);
    player_image = LoadTextureFromMemory(dx_device, player_image_data);
    module_script_image = LoadTextureFromMemory(dx_device, module_script_image_data);
    run_service_image = LoadTextureFromMemory(dx_device, run_service_image_data);
    spawn_location_image = LoadTextureFromMemory(dx_device, spawn_location_image_data);
    stats_image = LoadTextureFromMemory(dx_device, stats_image_data);
    starter_gui_image = LoadTextureFromMemory(dx_device, starter_gui_image_data);
    replicated_first_image = LoadTextureFromMemory(dx_device, replicated_first_image_data);
    chat_image = LoadTextureFromMemory(dx_device, chat_image_data);
    starter_pack_image = LoadTextureFromMemory(dx_device, starter_pack_image_data);
    gui_service_image = LoadTextureFromMemory(dx_device, gui_service_image_data);
    core_gui_image = LoadTextureFromMemory(dx_device, core_gui_image_data);
	ui_list_layout_image = LoadTextureFromMemory(dx_device, ui_list_layout_data);
	remote_event_image = LoadTextureFromMemory(dx_device, remote_event_data);
	remote_function_image = LoadTextureFromMemory(dx_device, remote_function_data);
	data_store_image = LoadTextureFromMemory(dx_device, data_store_data);
	text_label_image = LoadTextureFromMemory(dx_device, text_label_data);
	text_button_image = LoadTextureFromMemory(dx_device, text_button_data);
	image_label_image = LoadTextureFromMemory(dx_device, image_label_data);
	frame_image = LoadTextureFromMemory(dx_device, frame_data);
	billboard_gui_image = LoadTextureFromMemory(dx_device, billboard_gui_data);
	surface_gui_image = LoadTextureFromMemory(dx_device, surface_gui_data);
	http_rbx_api_service_image = LoadTextureFromMemory(dx_device, http_rbx_api_service_data);
	insert_service_image = LoadTextureFromMemory(dx_device, insert_service_data);

    globals::images_init = true;
}
