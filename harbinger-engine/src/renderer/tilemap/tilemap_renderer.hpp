#pragma once
#include "core/containers/containers.hpp"
#include "core/defines.hpp"
#include "core/memory/memory_tag.hpp"
#include "rhi/common/objects.hpp"
#include "rhi/rhi.hpp"

#include <glm/glm.hpp>

namespace har
{
    class tilemap_renderer
    {  
        struct tilemap_shaders
        {
            shader_handle vertexShader;
            shader_handle fragmentShader;
        };

        struct world
        {
            glm::mat4 viewProjection;
        };

    public:
        struct tile
        {
            u32 position[2];
            u32 layer;
        };

        constexpr static inline u32 DefaultMaxTile = 10'000;

    public:
        void initialize(NON_OWNING rhi_t* rhi, u32 maxTileCount = DefaultMaxTile);
        void render();

    public:
        void push_tile(const tile& tile);

    public:
        constexpr inline void set_render_target(render_target_handle handle = invalid_small_handle) { mRenderTarget = handle; } 

    private:
        tilemap_shaders initialize_shaders();
        void initialize_pipeline(tilemap_renderer::tilemap_shaders&& shaders);
        void initialize_buffers();
        void initialize_descriptors();

        void prepare_render();

    private:
        har_transient_vector<memory_tag::renderer, tile> mTiles;
        world mWorldData;
        
        command_buffer_t mCommandBuffer;
        command_buffer_t mTransferBuffer;

        NON_OWNING rhi_t* rRHI = nullptr;

    private:
        graphics_pipeline_handle mPipeline      = invalid_small_handle;
        descriptor_set_layout_handle mSetLayout = invalid_small_handle;
        descriptor_set_handle mSet              = invalid_small_handle;
        render_target_handle mRenderTarget      = invalid_small_handle;
        buffer_handle mSSBO                     = invalid_small_handle;
        buffer_handle mUBO                      = invalid_small_handle;
    };
}