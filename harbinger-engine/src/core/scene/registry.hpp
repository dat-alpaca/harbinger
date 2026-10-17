#pragma once
#include "core/containers/containers.hpp"
#include "core/memory/memory_tag.hpp"
#include "entt/entity/fwd.hpp"
#include <entt/entity/registry.hpp>

namespace har
{
    using registry_t = entt::basic_registry<entt::entity, dynamic_allocator_wrapper<entt::entity, memory_tag::scene>>;
}