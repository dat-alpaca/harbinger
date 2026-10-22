#include "entity.hpp"
#include "core/assert/assert.hpp"

namespace har
{
    entity_t::entity_t (entt::entity entity, NON_OWNING scene_t* scene)
        : rScene(scene)
        , mEntity(entity)
    {
        HAR_ASS(rScene);
    }
}