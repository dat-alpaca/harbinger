#pragma once
#include <concepts>
#include "core/assets/asset.hpp"
#include "core/utils/uuid.hpp"

namespace har
{
    template <typename T>
    concept is_component_linkable_through_load = requires (T component) 
    {
        { component.assetUUID }   -> std::same_as<uuid_t&>;
        { component.assetHandle } -> std::same_as<asset_handle&>;
    };

    template <typename T>
    concept is_component_linkable_through_upload = requires (T component) 
    {
        { component.assetHandle }     -> std::same_as<asset_handle&>;
        component.gpuObjectHandle;
    };
}