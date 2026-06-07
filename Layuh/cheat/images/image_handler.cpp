#include "image_handler.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// DX11 texture loader — replaces the old DX9 version
ID3D11ShaderResourceView* LoadTextureFromMemory(ID3D11Device* pDevice, const std::vector<uint8_t>& data) {
    if (data.empty() || !pDevice) return nullptr;

    int width = 0, height = 0, channels = 0;
    unsigned char* image_data = stbi_load_from_memory(data.data(), static_cast<int>(data.size()), &width, &height, &channels, 4);
    if (!image_data) return nullptr;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width            = width;
    desc.Height           = height;
    desc.MipLevels        = 1;
    desc.ArraySize        = 1;
    desc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage            = D3D11_USAGE_DEFAULT;
    desc.BindFlags        = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA subResource = {};
    subResource.pSysMem     = image_data;
    subResource.SysMemPitch = desc.Width * 4;

    ID3D11Texture2D* pTexture = nullptr;
    HRESULT hr = pDevice->CreateTexture2D(&desc, &subResource, &pTexture);
    stbi_image_free(image_data);
    if (FAILED(hr)) return nullptr;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                    = desc.Format;
    srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels       = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;

    ID3D11ShaderResourceView* pSRV = nullptr;
    pDevice->CreateShaderResourceView(pTexture, &srvDesc, &pSRV);
    pTexture->Release();
    return pSRV;
}

void CImageHandler::create_images() {
    if (globals::images_init) return;
    if (!dx_device) return;

    workspace_image          = LoadTextureFromMemory(dx_device, workspace_image_data);
    part_image               = LoadTextureFromMemory(dx_device, part_image_data);
    model_image              = LoadTextureFromMemory(dx_device, model_image_data);
    folder_image             = LoadTextureFromMemory(dx_device, folder_image_data);
    camera_image             = LoadTextureFromMemory(dx_device, camera_image_data);
    script_image             = LoadTextureFromMemory(dx_device, script_image_data);
    local_script_image       = LoadTextureFromMemory(dx_device, local_script_image_data);
    players_image            = LoadTextureFromMemory(dx_device, players_image_data);
    humanoid_image           = LoadTextureFromMemory(dx_device, humanoid_image_data);
    accessory_image          = LoadTextureFromMemory(dx_device, accessory_image_data);
    sound_image              = LoadTextureFromMemory(dx_device, sound_image_data);
    replicated_storage_image = LoadTextureFromMemory(dx_device, replicated_storage_image_data);
    hat_image                = LoadTextureFromMemory(dx_device, hat_image_data);
    player_image             = LoadTextureFromMemory(dx_device, player_image_data);
    module_script_image      = LoadTextureFromMemory(dx_device, module_script_image_data);
    run_service_image        = LoadTextureFromMemory(dx_device, run_service_image_data);
    spawn_location_image     = LoadTextureFromMemory(dx_device, spawn_location_image_data);
    stats_image              = LoadTextureFromMemory(dx_device, stats_image_data);
    starter_gui_image        = LoadTextureFromMemory(dx_device, starter_gui_image_data);
    replicated_first_image   = LoadTextureFromMemory(dx_device, replicated_first_image_data);
    chat_image               = LoadTextureFromMemory(dx_device, chat_image_data);
    starter_pack_image       = LoadTextureFromMemory(dx_device, starter_pack_image_data);
    gui_service_image        = LoadTextureFromMemory(dx_device, gui_service_image_data);
    core_gui_image           = LoadTextureFromMemory(dx_device, core_gui_image_data);
    ui_list_layout_image     = LoadTextureFromMemory(dx_device, ui_list_layout_data);
    remote_event_image       = LoadTextureFromMemory(dx_device, remote_event_data);
    remote_function_image    = LoadTextureFromMemory(dx_device, remote_function_data);
    data_store_image         = LoadTextureFromMemory(dx_device, data_store_data);
    text_label_image         = LoadTextureFromMemory(dx_device, text_label_data);
    text_button_image        = LoadTextureFromMemory(dx_device, text_button_data);
    image_label_image        = LoadTextureFromMemory(dx_device, image_label_data);
    frame_image              = LoadTextureFromMemory(dx_device, frame_data);
    billboard_gui_image      = LoadTextureFromMemory(dx_device, billboard_gui_data);
    surface_gui_image        = LoadTextureFromMemory(dx_device, surface_gui_data);
    http_rbx_api_service_image = LoadTextureFromMemory(dx_device, http_rbx_api_service_data);
    insert_service_image     = LoadTextureFromMemory(dx_device, insert_service_data);

    globals::images_init = true;
}
