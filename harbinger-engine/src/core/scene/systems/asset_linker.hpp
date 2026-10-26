#pragma once
#include "core/assets/asset.hpp"
#include "core/defines.hpp"
#include "core/assets/asset_hub.hpp"
#include "core/scene/scene_holder.hpp"

namespace har
{
    class asset_linker
    {
    public:
        void initialize(NON_OWNING asset_hub* assetHub, NON_OWNING scene_holder* sceneHolder);
        void on_event(event& event);

    private:
        void handle_material_instance_asset_load(uuid eventUUID, asset_handle handle);
        void handle_material_instance_asset_upload(asset_handle assetHandle, handle_t gpuHandle);

        void handle_pso_asset_load(uuid eventUUID, asset_handle handle);
        void handle_pso_asset_upload(asset_handle assetHandle, handle_t gpuHandle);
    
    private:
        NON_OWNING asset_hub* rAssetHub = nullptr;
        NON_OWNING scene_holder* rSceneHolder = nullptr;
    };
}