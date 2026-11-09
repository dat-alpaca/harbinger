#pragma once
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include "core/assets/pso/pso_asset.hpp"
#include "core/assets/texture/texture_asset.hpp"
#include "core/utils/transform.hpp"
#include "rhi/common/objects.hpp"

namespace glm
{
    /* vec2 */
    void to_json(nlohmann::json& jsonObject, const glm::vec2& descriptor);
    void from_json(const nlohmann::json& jsonObject, glm::vec2& descriptor);

    /* vec3 */
    void to_json(nlohmann::json& jsonObject, const glm::vec3& descriptor);
    void from_json(const nlohmann::json& jsonObject, glm::vec3& descriptor);

    /* vec4 */
    void to_json(nlohmann::json& jsonObject, const glm::vec4& descriptor);
    void from_json(const nlohmann::json& jsonObject, glm::vec4& descriptor);
}

namespace har
{
    NLOHMANN_JSON_SERIALIZE_ENUM(format_t, 
    {
        { format_t::invalid, "invalid" },
        { format_t::r_ui8, "r_ui8" },
        { format_t::r_snorm8, "r_snorm8" },
        { format_t::rg_snorm8, "rg_snorm8" },
        { format_t::rgb_ui8, "rgb_ui8" },
        { format_t::rgb_snorm8, "rgb_snorm8" },
        { format_t::rgba_ui8, "rgba_ui8" },
        { format_t::rgba_snorm8, "rgba_snorm8" },
        { format_t::d16, "d16" },
        { format_t::d32, "d32" },
        { format_t::d24_s8, "d24_s8" },
        { format_t::rgb_f32, "rgb_f32" },
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(data_type, 
    {
        { data_type::unsigned_byte, "unsigned_byte" },
        { data_type::signed_byte, "signed_byte" },
        { data_type::unsigned_short, "unsigned_short" },
        { data_type::signed_short, "signed_short" },
        { data_type::unsigned_int, "unsigned_int" },
        { data_type::signed_int, "signed_int" },
        { data_type::float_t, "float_t" },
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(texture_dimension, 
    {
        { texture_dimension::invalid, "invalid" },
        { texture_dimension::texture_2d, "texture_2d" },
        { texture_dimension::texture_2d_array, "texture_2d_array" },
        { texture_dimension::texture_3d, "texture_3d" }
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(sampler_wrap_mode, 
    {
        { sampler_wrap_mode::repeat, "repeat" },
        { sampler_wrap_mode::mirrored_repeat, "mirrored_repeat" },
        { sampler_wrap_mode::clamp_to_edge, "clamp_to_edge" },
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(sampler_filter, 
    {
        { sampler_filter::linear, "linear" },
        { sampler_filter::nearest, "nearest" },
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(texture_compression, 
    {
        { texture_compression::none, "none" },
        { texture_compression::dxt1,  "dxt1" },
        { texture_compression::dxt3,  "dxt3" },
        { texture_compression::dxt5,  "dxt5" },
    });

    /* Rasterizer */
    NLOHMANN_JSON_SERIALIZE_ENUM(front_face, 
    {
        { front_face::cw,   "cw" },
        { front_face::ccw,  "ccw" },
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(cull_mode, 
    {
        { cull_mode::none,  "none" },
        { cull_mode::front, "front" },
        { cull_mode::back,  "back" },
        { cull_mode::both,  "both" },
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(polygon_mode, 
    {
        { polygon_mode::point, "point" },
        { polygon_mode::fill,  "fill" },
        { polygon_mode::line,  "line" },
    });

    void to_json(nlohmann::json& jsonObject, const rasterizer_t& descriptor);
    void from_json(const nlohmann::json& jsonObject, rasterizer_t& descriptor);

    /* color blending */
    // TODO: custom blending options
    void to_json(nlohmann::json& jsonObject, const color_blending& descriptor);
    void from_json(const nlohmann::json& jsonObject, color_blending& descriptor);

    /* multisampling */
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(multisampling_t, sampleCount, enableSampling);

    /* input assembly */
    NLOHMANN_JSON_SERIALIZE_ENUM(topology_t, 
    {
        { topology_t::points, "points" },
        { topology_t::lines, "lines" },
        { topology_t::line_strips, "line_strips" },
        { topology_t::triangles, "triangles" },
        { topology_t::triangle_strips, "triangle_strips" },
    });
    void to_json(nlohmann::json& jsonObject, const input_assembly& descriptor);
    void from_json(const nlohmann::json& jsonObject, input_assembly& descriptor);

    /* pso_asset */
    void to_json(nlohmann::json& jsonObject, const pso_asset& descriptor);
    void from_json(const nlohmann::json& jsonObject, pso_asset& descriptor);
    
    /* transform */
    void to_json(nlohmann::json& jsonObject, const transform_t& descriptor);
    void from_json(const nlohmann::json& jsonObject, transform_t& descriptor);

    /* texture_descriptor */
    void to_json(nlohmann::json& jsonObject, const texture_descriptor& descriptor);
    void from_json(const nlohmann::json& jsonObject, texture_descriptor& descriptor);

    /* sampler_descriptor */
    void to_json(nlohmann::json& jsonObject, const sampler_descriptor& descriptor);
    void from_json(const nlohmann::json& jsonObject, sampler_descriptor& descriptor);
}