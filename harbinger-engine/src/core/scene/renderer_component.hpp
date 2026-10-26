#pragma once
#include "core/defines.hpp"
#include "core/utils/uuid.hpp"
#include "core/assets/asset.hpp"
#include "core/scene/components.hpp"
#include "renderer/material.hpp"
#include "rhi/common/objects.hpp"

namespace har
{
    struct renderable_component
    {
        REGISTER_COMPONENT(renderable_component)

    public:
        renderable_component() = default;
        renderable_component(const renderable_component&) = default;

    public:
        // Set through serialization
        FIELD_SET_INTERNALLY uuid pipelineAssetUUID = invalid_uuid;

        // Set through asset_linker system
        FIELD_SET_INTERNALLY asset_handle pipelineAssetHandle = invalid_small_handle;
        FIELD_SET_INTERNALLY graphics_pipeline_handle pipelineHandle = invalid_small_handle;

        // Set through serialization
        FIELD_SET_INTERNALLY bool isImmutable : 1 = true;
    };

    struct material_instance_component
    {
        REGISTER_COMPONENT(material_instance_component)

    public:
        material_instance_component() = default;
        material_instance_component(const material_instance_component&) = default;

    public:
        // Set through serialization
        FIELD_SET_INTERNALLY uuid assetUUID = invalid_uuid;

        // Set through asset_linker system
        FIELD_SET_INTERNALLY asset_handle assetHandle = invalid_small_handle;
        FIELD_SET_INTERNALLY material_instance_handle materialHandle = invalid_small_handle;
    };
}