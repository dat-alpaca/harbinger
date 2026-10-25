#include "scene_holder.hpp"
#include "core/assert/assert.hpp"
#include "core/event/concrete/scene_events.hpp"

namespace har
{
    void scene_holder::initialize(NON_OWNING event_queue* eventQueue, const scene_holder_desc& description)
    {
        rEventQueue = eventQueue;
        mScenes.reserve(description.maxSceneCount);
    }

    scene_handle scene_holder::create_scene(scene_options options)
    {
        mScenes.emplace_back(har::make_unique<scene_t>());
        scene_handle sceneHandle = static_cast<scene_handle>(mScenes.size() - 1);

        send_scene_created_event(*rEventQueue, { .createdScene = mScenes.back().get() });

        if ((options & scene_options::set_immediate) != 0)
            set_current_scene(sceneHandle);

        return sceneHandle;
    }

    void scene_holder::set_current_scene(scene_handle handle)
    {
        HAR_ASS(is_valid_scene_handle(handle));

        scene_t* previousScene = mCurrentScene;
        mCurrentScene = mScenes[handle].get();
        
        send_scene_switched_event(*rEventQueue, { .previous = previousScene, .current = mCurrentScene });
    }
}