#pragma once
#include "core/assert/assert.hpp"
#include "core/assets/asset_hub.hpp"
#include "core/assets/asset_type.hpp"
#include "core/assets/custom_format.hpp"
#include "core/assets/material_instance/material_instance_asset.hpp"
#include "core/containers/containers.hpp"
#include "core/defines.hpp"
#include "core/event/concrete/asset_events.hpp"
#include "core/event/event.hpp"
#include "core/event/event_queue.hpp"
#include "core/event/event_type.hpp"
#include "core/memory/allocators.hpp"
#include "core/memory/memory_tag.hpp"
#include "core/utils/uuid.hpp"
#include "renderer/renderer.hpp"
#include <cstring>

namespace har
{
    using namespace har::internal;

    class material_instance_proxy
    {
    public:
        void initialize(NON_OWNING asset_hub* assetHub, NON_OWNING harbinger_renderer* renderer, NON_OWNING event_queue* eventQueue)
        {
            HAR_ASS(assetHub);
            HAR_ASS(renderer);
            HAR_ASS(eventQueue);

            rAssetHub = assetHub;
            rRenderer = renderer;
            rEventQueue = eventQueue;
        }

    public:
        void on_event(event& event)
        {
            if (event.type == event_type::asset_loaded)
            {
                auto eventData = event.get<asset_loaded_event_data>();
                if (eventData.type != asset_type::material_instance)
                    return;
                
                auto* asset = static_cast<material_instance_asset*>(rAssetHub->get_asset(eventData.handle));
                handle_t gpuHandle = handle_material_instance_loaded(asset);

                send_asset_uploaded_event
                (   
                    *rEventQueue, 
                    { .assetHandle = eventData.handle, .gpuHandle = gpuHandle, .type = asset_type::material_instance 
                });
            }

            else if (event.type == event_type::asset_uploaded)
            {
                auto eventData = event.get<asset_uploaded_event_data>();
                if (eventData.type != asset_type::texture)
                    return;

                handle_texture_uploaded();
            }
        }

    private:
        handle_t handle_material_instance_loaded(material_instance_asset* asset)
        {
            u64 entrySize = 0;
            for (const auto& entry : asset->entries)
                entrySize += get_format_final_byte_size(entry.format);

            mMaterialInstanceData.push_back
            ({
                static_cast<byte*>(MainDynamicAllocator.allocate(memory_tag::renderer, entrySize)),
                entrySize
            });
            auto* currentData = mMaterialInstanceData.back().first;

            u32 byteOffset = 0;
            for (const auto& entry : asset->entries)
            {
                if (entry.format == custom_format::none)
                    break;

                switch(entry.format)
                {
                    case custom_format::texture:
                    {
                        uuid_t textureUUID = std::get<uuid_t>(entry.value);
                        if (rRenderer->get_texture_id_map().contains(textureUUID))
                        {
                            auto textureID = rRenderer->get_texture_id_map().at(textureUUID);
                            std::memcpy(currentData + byteOffset, &textureID, sizeof(handle_t));
                        }
                        else
                        {
                            auto defaultTextureID = rRenderer->get_default_texture_id_handle();
                            std::memcpy(currentData + byteOffset, &defaultTextureID, sizeof(handle_t));
                        }
                    } break;

                    default:
                        std::visit([currentData, byteOffset](const auto& value)
                        {
                            std::memcpy(currentData + byteOffset, &value, sizeof(value));
                        }, entry.value);
                        break;
                }

                byteOffset += get_format_final_byte_size(entry.format);
            }

            return mMaterialInstanceData.size() - 1;
        }

        void handle_texture_uploaded()
        {
            for (auto it = mPendingMaterialTextures.begin(); it != mPendingMaterialTextures.end();)
            {
                const auto& unresolvedTextureID = *it;
                uuid_t uuid = *reinterpret_cast<uuid_t*>(unresolvedTextureID);

                if (rRenderer->get_texture_id_map().contains(uuid))
                {
                    std::memcpy(unresolvedTextureID, &rRenderer->get_texture_id_map().at(uuid), sizeof(handle_t));
                    it = mPendingMaterialTextures.erase(it);
                }
                else
                    ++it;
            }
        }

    public:
        inline auto get_material_instance_data(handle_t materialInstanceHandle) const { return mMaterialInstanceData[materialInstanceHandle]; }

    private:
        NON_OWNING harbinger_renderer* rRenderer = nullptr;
        NON_OWNING asset_hub* rAssetHub = nullptr;
        NON_OWNING event_queue* rEventQueue = nullptr;

    private:
        har_vector<memory_tag::renderer, texture_id*> mPendingMaterialTextures;
        har_vector<memory_tag::renderer, std::pair<byte*, u64>> mMaterialInstanceData;
    };
}