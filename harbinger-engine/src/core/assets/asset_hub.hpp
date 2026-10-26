#pragma once
#include <functional>

#include "core/defines.hpp"
#include "core/assets/asset.hpp"
#include "core/assets/asset_bank.hpp"
#include "core/event/event_queue.hpp"
#include "core/scene/scene_holder.hpp"
#include "core/utils/uuid.hpp"
#include "core/worker/worker_pool.hpp"
#include "core/containers/nano_free_list.hpp"

namespace har
{
    using asset_loader_function = std::function<void(const std::filesystem::path&, asset*, NON_OWNING asset_bank_option*)>;
    using asset_unloader_function = std::function<void(asset*)>;
    using asset_loader_callback = std::function<void(asset_handle)>;

    constexpr static inline u32 DefaultMaxAssetCount = 10; 

    struct asset_hub_context
    {
        NON_OWNING worker_pool* workerPool   = nullptr;
        NON_OWNING event_queue* eventQueue   = nullptr;
        NON_OWNING scene_holder* sceneHolder = nullptr;
    };

    struct asset_hub_desc
    {
    public:
        constexpr u64 get_required_static_memory() const
        {
            return nano_free_list<asset*>::required_memory(maxAssets);
        }

    public:
        u32 maxAssets = DefaultMaxAssetCount;
    };

    class asset_hub
    {
    public:
        void initialize(asset_hub_context&& context, const asset_hub_desc& desc, const std::filesystem::path& assetBankFilepath);

    public:
        asset_handle load_asset(asset_type type, const std::filesystem::path& filepath, asset_loader_callback&& callback = {}, bool inPlace = false, uuid assetUUID = invalid_uuid);
        void unload_asset(asset_type type, const std::filesystem::path& filepath, bool inPlace = false);
        void unload_asset(asset_type type, asset_handle handle, bool inPlace = false);
        
        void call_asset_loader(asset_type type, const std::filesystem::path& filepath);

        asset* get_asset(const std::filesystem::path& filepath);
        asset* get_asset(asset_handle handle);

    private:
        void register_asset_loader(asset_type type, asset_loader_function&& function);
        void register_asset_unloader(asset_type type, asset_unloader_function&& function);
        asset* allocate_appropriate_asset_type(asset_type type, const std::filesystem::path& filepath, uuid assetUUID = invalid_uuid);

    public:
        constexpr const asset_bank& get_asset_bank() const { return mAssetBank; }
        constexpr asset_bank& get_asset_bank() { return mAssetBank; }
        
    private:
        void issue_loader_call(asset_handle handle, asset_type type, const std::filesystem::path& filepath, asset_loader_callback&& callback);
        void issue_loader_call_in_place(asset_handle handle, asset_type type, const std::filesystem::path& filepath, asset_loader_callback&& callback);

    private:
        har_unordered_map<asset_type, asset_loader_function> mLoaders;
        har_unordered_map<asset_type, asset_unloader_function> mUnloaders;
        har_unordered_map<std::string, asset_handle> mCache;

        nano_free_list<asset*> mAssets;
        asset_bank mAssetBank;
       
    private:
        asset_hub_context mContext;
    };
}