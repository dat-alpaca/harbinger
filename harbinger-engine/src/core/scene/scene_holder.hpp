#pragma once
#include "core/defines.hpp"
#include "core/scene/scene.hpp"
#include "core/assert/assert.hpp"
#include "core/event/event_queue.hpp"
#include "core/memory/memory_tag.hpp"
#include "core/containers/containers.hpp"

namespace har
{
    enum class scene_options : u8
    {
        none                = 0,
        set_immediate       = 1 << 0
    };
    HAR_DEFINE_ENUM(scene_options);

    struct scene_holder_desc
    {
    public:
        constexpr u64 get_required_static_memory() const
        {
            return sizeof(har_unique<scene_t>) * maxSceneCount;
        }

    public:
        u32 maxSceneCount = 1;
    };

    class scene_holder
    {
    public:
        constexpr static u64 get_required_static_memory(u32 maxSceneCount)
        {
            return sizeof(har_unique<scene_t>) * maxSceneCount;
        }

    public:
        void initialize(NON_OWNING event_queue* eventQueue, const scene_holder_desc& description);

    public:
        scene_handle create_scene(scene_options options = scene_options::none);
        void set_current_scene(scene_handle handle);
        
    public:
        constexpr inline NON_OWNING scene_t* get_current_scene() { return mCurrentScene; }
        inline NON_OWNING scene_t* get_scene(scene_handle handle)
        {
            HAR_ASS(is_valid_scene_handle(handle));
            return mScenes[handle].get();
        }

    public:
        constexpr inline bool is_valid_scene_handle(scene_handle handle) const
        {
            return handle >= 0 && handle < mScenes.size();
        }

    private:
        har_static_vector<memory_tag::scene, har_unique<scene_t>> mScenes;
        NON_OWNING event_queue* rEventQueue = nullptr;
        NON_OWNING scene_t* mCurrentScene = nullptr;
    };
}