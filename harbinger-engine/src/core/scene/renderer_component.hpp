#pragma once
#include "core/assets/mesh/mesh_asset.hpp"
#include "core/defines.hpp"
#include "core/utils/uuid.hpp"
#include "core/assets/asset.hpp"
#include "core/scene/components.hpp"
#include "renderer/material.hpp"
#include "renderer/texture_id.hpp"
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
        // Set through serialization (pipeline asset uuid)
        FIELD_SET_INTERNALLY uuid_t assetUUID = invalid_uuid;

        // Set through asset_linker system (pipeline asset handle)
        FIELD_SET_INTERNALLY asset_handle assetHandle = invalid_small_handle;
        FIELD_SET_INTERNALLY graphics_pipeline_handle gpuObjectHandle = invalid_small_handle;

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
        FIELD_SET_INTERNALLY uuid_t assetUUID = invalid_uuid;

        // Set through asset_linker system
        FIELD_SET_INTERNALLY asset_handle assetHandle = invalid_small_handle;
        FIELD_SET_INTERNALLY handle_t gpuObjectHandle = invalid_small_handle;
    };

    struct mesh_component
    {
        REGISTER_COMPONENT(mesh_component)

    public:
        mesh_component() = default;
        mesh_component(const mesh_component&) = default;

    public:
        // Set through serialization
        FIELD_SET_INTERNALLY uuid_t assetUUID = invalid_uuid;

        // Set through asset_linker system
        FIELD_SET_INTERNALLY asset_handle assetHandle = invalid_small_handle;
        FIELD_SET_INTERNALLY mesh_handle gpuObjectHandle = invalid_small_handle;
    };
}