#pragma once
#include "scene.hpp"
#include "core/defines.hpp"
#include "entt/entity/fwd.hpp"
#include <utility>

namespace har
{
    class entity_t
    {
        friend class scene_t;

    private:
        entity_t (entt::entity entity, NON_OWNING scene_t* scene);
        entity_t (const entity_t&) = default;
        entity_t (entity_t&&) = default;

    public:
        template <typename Type, typename... Args>
        inline Type& emplace(Args&&... args)
        {
            Type& component = rScene->get_registry().emplace<Type>(mEntity, std::forward<Args>(args)...);
            return component;
        }

    private:
        NON_OWNING scene_t* rScene = nullptr;
        entt::entity mEntity;
    };
}