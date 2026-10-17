#pragma once
#include "registry.hpp"
#include "core/defines.hpp"
#include "entt/entity/fwd.hpp"

namespace har
{
    constexpr static inline u32 DefaultScenePoolCount = 0;

    class entity_t;

    class scene_t
    {
    public:
        scene_t(u64 poolCount = DefaultScenePoolCount);

    public:
        entity_t create_entity(entt::entity entity);
        
    public:
        const registry_t& get_registry() const { return mRegistry; }
        registry_t& get_registry() { return mRegistry; }

    private:
        registry_t mRegistry;
    };

    using scene_handle = handle_t;
}