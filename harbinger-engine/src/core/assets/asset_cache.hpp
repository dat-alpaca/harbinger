#pragma once
#include <string.h>
#include <filesystem>
#include "core/defines.hpp"
#include "core/assets/asset.hpp"
#include "core/containers/containers.hpp"
#include "core/memory/linear_allocator.hpp"
#include "core/containers/nano_free_list.hpp"

namespace har
{
    template <typename Resource> 
    class asset_cache
    {
        struct resource_data
        {
            const char* filepath = nullptr;
            Resource resource;
        };

    public:
        constexpr static inline u64 get_required_static_memory(u32 maxResourceCacheCount)
        {
            return sizeof(har_unique<resource_data>) * maxResourceCacheCount;
        }

    public:
        void initialize(NON_OWNING static_allocator* allocator, u32 maxResourceCacheCount)
        {
            mResources.initialize(allocator->push(get_required_static_memory(maxResourceCacheCount), alignof(u8)), maxResourceCacheCount);
        }

    public:
        asset_handle register_empty_asset()
        {
            asset_handle handle = mResources.insert(har::make_unique<resource_data>());
            return handle;
        }

        void update_asset(asset_handle handle, const std::filesystem::path& filepath, const Resource& resource)
        {
            mResources[handle]->filepath = strdup(filepath.c_str());
            mResources[handle]->resource = resource;
            mFilepathMap[filepath.string()] = handle;
        }

        asset_handle register_asset(const std::filesystem::path& filepath, const Resource& resource)
        {
            asset_handle handle = mResources.insert(har::make_unique<resource_data>(
                resource_data { .filepath = strdup(filepath.c_str()), .resource = resource }
            ));
            mFilepathMap[filepath.string()] = handle;

            return handle;
        }

        void remove_asset(const std::filesystem::path& filepath)
        {
            assert (is_asset_available(filepath));

            mResources.remove(get_asset_handle(filepath));
            mFilepathMap.erase(filepath.string());
        }

        asset_handle get_asset_handle(const std::filesystem::path& filepath)
        {
            if (!is_asset_available(filepath))
                return invalid_small_handle;

            return mFilepathMap[filepath.string()];
        }

        resource_data* get_asset(const std::filesystem::path& filepath)
        {
            if (!is_asset_available(filepath))
                return nullptr;

            asset_handle handle = mFilepathMap[filepath.string()];
            return mResources[handle].get();
        }

        resource_data* get_asset(asset_handle handle)
        {
            return mResources[handle].get();
        }

    public:
        constexpr bool is_asset_available(const std::filesystem::path& filepath)
        {
            return mFilepathMap.contains(filepath.string());
        }

        constexpr bool is_handle_valid(asset_handle handle)
        {
            return handle >= 0 && handle < mResources.size();
        }

    private:
        nano_free_list<har_unique<resource_data>> mResources;
        har_unordered_map<std::string, asset_handle> mFilepathMap;
    };
}