#pragma once
#include <cstring>
#include <glm/glm.hpp>

#include "core/assets/asset.hpp"
#include "core/utils/transform.hpp"
#include "renderer/material.hpp"
#include "renderer/mesh.hpp"

#include "core/defines.hpp"
#include "core/containers/containers.hpp"

#define REGISTER_COMPONENT(componentName) 													\
public:																						\
	constexpr static inline const char* get_component_name() {  return #componentName; }	\

#define FIELD_SET_INTERNALLY
#define FIELD_SET_BY_USER

#include "renderer_component.hpp"

namespace har
{
    struct name_component
    {
        REGISTER_COMPONENT(name_component)

    public:
        name_component() = default;
        name_component(const name_component&) = default;
        name_component(const char* name) { this->name = strdup(name); }

    public:
        FIELD_SET_BY_USER const char* name;
    };

    struct transform_component
    {
        REGISTER_COMPONENT(transform_component)

    public:
        transform_component() = default;
        transform_component(const transform_component&) = default;
        transform_component(const transform_t& other) : transform(other) { }

    public:
        FIELD_SET_INTERNALLY transform_t transform;
    };

    struct script_component
    {
        FIELD_SET_INTERNALLY asset_handle handle;  
    };
}