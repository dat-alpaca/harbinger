#pragma once
#include <span>
#include "core/containers/containers.hpp"
#include "core/defines.hpp"
#include "core/memory/memory_tag.hpp"
#include "rhi/common/format.hpp"

namespace har
{
    // Queue:
    using queue_handle       = small_handle_t;

    // Pipeline Stage:
    enum class pipeline_stage : u8
    {
        none        = 0,
        vertex      = 1,
        fragment    = 1 << 1,
        both        = 2
    };
    HAR_DEFINE_ENUM(pipeline_stage); 

    // Shader:
    enum class shader_type : u8
    {
        invalid     = invalid_type_t<u8>,
        vertex      = 0,
        fragment
    };
    HAR_DEFINE_ENUM(shader_type);
    using shader_handle      = small_handle_t;

    constexpr inline const char* get_shader_type_name(shader_type type)
    {
        switch(type)
        {
            case shader_type::invalid: return "invalid";
            case shader_type::vertex: return "vertex";
            case shader_type::fragment: return "fragment";
        }

        return "";
    }

    // Buffer:
    enum class buffer_usage : u8
    {
        invalid     = invalid_type_t<u8>,
        vertex      = 1 << 0,
        index       = 1 << 1,
        uniform     = 1 << 2,
        storage     = 1 << 3,
        indirect    = 1 << 4,

        persistent  = 1 << 5,
        coherent    = 1 << 5,
        read        = 1 << 5,
        write       = 1 << 6,
    };
    HAR_DEFINE_ENUM(buffer_usage);

    struct buffer_descriptor
    {
        buffer_usage bufferUsage = buffer_usage::invalid;
        u64 sizeBytes = 0;
    };
    using buffer_handle      = small_handle_t;
    
    // Texture:
    enum class texture_dimension : u8
    {
        invalid           = invalid_type_t<u8>,
        texture_2d        = 0,
        texture_2d_array,
        texture_3d,
    };
    HAR_DEFINE_ENUM(texture_dimension);
    
    struct texture_descriptor
    {
        u32 width               = 1;
        u32 height              = 1;
        u32 layerWidth;
        u32 layerHeight;
        
        u16 arraySize           = 1;
        u8 depth               = 1;
        
        texture_dimension dimension = texture_dimension::invalid;
        format_t internalFormat       = format_t::rgba_ui8;
        u8  levels                  = 1;
        
        u8  isRenderTarget : 1;
    };
    using texture_handle     = small_handle_t;

    // Sampler:
    enum class sampler_filter : u8
    {
        nearest = 0,
        linear  = 1,
    };
    HAR_DEFINE_ENUM(sampler_filter);

    enum class sampler_wrap_mode : u8
    {
        repeat              = 0,
        mirrored_repeat     = 1,
        clamp_to_edge       = 2,
        // clamp_to_border  = 3,
    };
    HAR_DEFINE_ENUM(sampler_wrap_mode);

    struct sampler_descriptor
    {
    public:
        sampler_descriptor()
            : wrapS(sampler_wrap_mode::repeat) 
            , wrapT(sampler_wrap_mode::repeat) 
            , wrapR(sampler_wrap_mode::repeat) 
            , magFilter(sampler_filter::nearest)
            , minFilter(sampler_filter::nearest)
            , enableMipFiltering(false)
            { }

    public:
        sampler_wrap_mode wrapS         : 4;
        sampler_wrap_mode wrapT         : 4;
        sampler_wrap_mode wrapR         : 4;

        sampler_filter magFilter        : 1;
        sampler_filter minFilter        : 1;

        u8 enableMipFiltering           : 1;
    };
    using sampler_handle     = small_handle_t;

    // Combined texture:
    struct combined_texture_descriptor
    {
        texture_handle textureHandle;
        sampler_handle samplerHandle;
    };
    using combined_texture_handle = small_handle_t;

    // Render target:
    struct render_target_attachment
    {
        texture_handle texture = invalid_small_handle;
        sampler_handle sampler = invalid_small_handle;
        format_t textureFormat = format_t::invalid;
    };

    struct render_target_descriptor
    {
        std::span<render_target_attachment> colorAttachments;
        render_target_attachment depthAttachment = { };
        u32 width   = 1;
        u32 height  = 1;
    };
    using render_target_handle    = small_handle_t;

    // Graphics Pipeline
    struct vertex_attribute
    {
        u8 binding;
        u8 offset;
        format_t vertexFormat;
        u8 location : 3;
        u8 instanced : 1;
    };

    struct vertex_input
    {
        std::span<vertex_attribute> attributes;
        u8 stride;
    };

    /* Input Assembly */
    enum class topology_t : u8
    {
        points,
        lines,
        line_strips,
        triangles,
        triangle_strips
    };

    struct input_assembly
    {
        topology_t topology : 7;
        u8 primitiveRestartEnable : 1;
    };

    /* Rasterizer */
    enum class polygon_mode : u8
    {
        point,
        fill,
        line
    };

    enum class cull_mode : u8
    {
        none,
        front,
        back,
        both
    };

    enum class front_face : u8
    {
        cw,
        ccw,
    };
    
    struct rasterizer_t
    {
    public:
        rasterizer_t()
            : frontFace(front_face::ccw)
            , cullMode(cull_mode::both)
            , polygonMode(polygon_mode::fill)
            , enableDepthClamp(false)
            , enableDiscard(false)
            , enableDepthBias(false)
        {}

    public:
        front_face frontFace        : 1;
        cull_mode cullMode          : 2;
        polygon_mode polygonMode    : 2;

        u8 enableDepthClamp         : 1;
        u8 enableDiscard            : 1;
        u8 enableDepthBias          : 1;
    };

    /* Multisampling */
    struct multisampling_t
    {
    public:
        multisampling_t() 
            : sampleCount(1)
            , enableSampling(true) 
        {}

    public:
        u8 sampleCount : 7;
        u8 enableSampling : 1;
    };

    /* Color blending */
    enum class blend_factor : u8
    {
        zero,
        one,
        src_color,
        src_alpha,
        one_minus_src_alpha  
    };

    enum class blend_operation : u8
    {
        add,
        subtract,
        min,
        max
    };

    struct blend_attachment
    {
    public:
        blend_attachment()
        : srcColorBlendFactor(blend_factor::one)
        , dstColorBlendFactor(blend_factor::zero)
        , colorBlendOp(blend_operation::add)
        , srcAlphaBlendFactor(blend_factor::one)
        , dstAlphaBlendFactor(blend_factor::zero)
        , alphaBlendOp(blend_operation::add)
        {}

        blend_attachment
        (
            blend_factor srcColor, blend_factor dstColor, blend_operation colorOp, 
            blend_factor srcAlpha, blend_factor dstAlpha, blend_operation alphaOp
        )
        : srcColorBlendFactor(srcColor)
        , dstColorBlendFactor(dstColor)
        , colorBlendOp(colorOp)
        , srcAlphaBlendFactor(srcAlpha)
        , dstAlphaBlendFactor(dstAlpha)
        , alphaBlendOp(alphaOp)
        {}

    public:
        static constexpr inline blend_attachment alpha_blending()
        {
            return blend_attachment
            (
                blend_factor::src_alpha,
                blend_factor::one_minus_src_alpha,
                blend_operation::add,
                blend_factor::one,
                blend_factor::zero,
                blend_operation::add
            );
        }

    public:
        blend_factor srcColorBlendFactor : 3;
        blend_factor dstColorBlendFactor : 3;
        blend_operation colorBlendOp : 2;

        blend_factor srcAlphaBlendFactor : 3;
        blend_factor dstAlphaBlendFactor : 3;
        blend_operation alphaBlendOp : 2;
    };
    
    struct color_blending
    {
        std::span<blend_attachment> attachments;
        u8 enableBlending : 1;
    };

    /* Graphics Pipeline*/
    struct graphics_pipeline_descriptor
    {
        vertex_input vertexInput    = {};
        rasterizer_t rasterizer;

        color_blending colorBlending;

        shader_handle vertexShader   = invalid_small_handle;
        shader_handle fragmentShader = invalid_small_handle;

        multisampling_t multisampling;
        input_assembly inputAssembly;
    };
    using graphics_pipeline_handle = small_handle_t;

    // Descriptor Set:
    enum class descriptor_set_type : u8
    {
        invalid,
        combined_texture,
        uniform_buffer,
        storage_buffer
    };

    struct descriptor_set_layout_item
    {
    public:
        constexpr inline bool is_valid() const 
        { 
            return type    != descriptor_set_type::invalid &&
                   stage   != pipeline_stage::none &&
                   amount  != 0 &&
                   binding != invalid_micro_handle;
        }

    public:
        descriptor_set_type type = descriptor_set_type::invalid;
        pipeline_stage stage = pipeline_stage::none;
        u8 binding = invalid_micro_handle;
        u8 amount = 0;
    };

    struct descriptor_set_layout
    {
        har_vector<memory_tag::rhi, descriptor_set_layout_item> items;
    };

    using descriptor_set_layout_handle = small_handle_t;
    using descriptor_set_handle = small_handle_t;

    // Object Type:
    enum class object_type : u8
    {
        invalid = 0,
        buffer,
        shader,
        pipeline,
        texture,
        sampler,
        render_target
    };
}