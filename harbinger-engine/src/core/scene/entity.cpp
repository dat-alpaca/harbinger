#include "entity.hpp"
#include <cassert>

namespace har
{
    entity_t::entity_t (entt::entity entity, NON_OWNING scene_t* scene)
        : rScene(scene)
        , mEntity(entity)
    {
        assert(rScene);
    }
}