#pragma once
#include "core/defines.hpp"
#include "core/assets/texture/texture_asset.hpp"

#include "rhi/common/clear_flags.hpp"
#include "rhi/common/color.hpp"
#include "rhi/common/objects.hpp"
#include "rhi/common/rect.hpp"
#include "rhi/opengl/rhi_gl.hpp"

namespace har::rhi
{
    class command_buffer_gl
    {
        friend class rhi_gl;
        explicit command_buffer_gl(NON_OWNING rhi_gl* rhi);

    public:
        explicit command_buffer_gl() = default;

    public:
        void begin(render_target_handle handle = invalid_small_handle);
        void end();
    
    public:
        void set_render_target(render_target_handle handle = invalid_small_handle);
        void set_viewport(const rect& viewport);
        void set_scissor(const rect& scissor);
        void set_clear_color(const color_t& color, clear_flags flags = clear_flags::color);

    public:
        void bind_graphics_pipeline(graphics_pipeline_handle handle);
        void bind_vertex_buffer(buffer_handle handle, u8 binding);
        void bind_index_buffer(buffer_handle handle);
        void bind_descriptor_set(descriptor_set_handle handle);

    public:
        void update_buffer(buffer_handle handle, std::span<const byte> buffer, u64 offset);
        void update_texture(texture_handle handle, const texture_asset& textureAsset, format dataFormat, data_type dataType);

    public:
        void draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance);
        void draw_indexed(u32 indexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance);

    private:
        void clear();

    private:
        NON_OWNING rhi_gl* rRHI = nullptr;

    private:
        clear_flags mClearFlags = clear_flags::color;
        graphics_pipeline_handle mActiveGraphicsPipeline = invalid_small_handle;
    };
}