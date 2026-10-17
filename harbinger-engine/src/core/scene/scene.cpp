#include "scene.hpp"
#include "entity.hpp"

namespace har
{
    scene_t::scene_t(u64 poolCount)
        : mRegistry(poolCount)
    {
    }

    entity_t scene_t::create_entity(entt::entity entity)
    {
        return { mRegistry.create(entity), this };
    }
}