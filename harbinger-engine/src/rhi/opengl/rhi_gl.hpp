#pragma once
#include "core/config/config.hpp"
#include "core/defines.hpp"

#include "platform/window.hpp"
#include "rhi/common/objects.hpp"
#include "rhi/opengl/containers.hpp"
#include "rhi/common/rhi_arguments.hpp"

namespace har { class harbinger_engine; }
namespace har::rhi
{
    class command_buffer_gl;

    class rhi_gl
    {
    public:
        void initialize(NON_OWNING config_t* config, NON_OWNING window_t* window, const rhi_arguments& arguments);

    public:
        bool begin_frame();
        void end_frame();
        void submit([[maybe_unused]] queue_handle workQueue = invalid_small_handle);
        void present([[maybe_unused]] queue_handle presentQueue = invalid_small_handle);

        void execute(command_buffer_gl& commandBuffer);

    public:
        shader_handle                 create_shader(shader_type type, std::span<u32> spirvData);
        buffer_handle                 create_buffer(const buffer_descriptor& descriptor);
        texture_handle                create_texture(const texture_descriptor& descriptor);
        combined_texture_handle       create_combined_texture(const combined_texture_descriptor& descriptor);
        sampler_handle                create_sampler(const sampler_descriptor& descriptor);
        render_target_handle          create_render_target(const render_target_descriptor& descriptor);
        graphics_pipeline_handle      create_graphics_pipeline(graphics_pipeline_descriptor&& descriptor);
        descriptor_set_layout_handle  create_descriptor_set_layout(const descriptor_set_layout& layout);
        descriptor_set_handle         create_descriptor_set(descriptor_set_layout_handle handle);

        void destroy_shader(shader_handle& handle);
        void destroy_buffer(buffer_handle& handle);
        void destroy_texture(texture_handle& handle);
        void destroy_sampler(sampler_handle& handle);
        void destroy_render_target(render_target_handle& handle);
        void destroy_graphics_pipeline(graphics_pipeline_handle& handle);
        void destroy_descriptor_set(descriptor_set_handle& handle);

        void write_uniform_buffer_to_set(descriptor_set_handle set, buffer_handle buffer, u32 binding);
        void write_storage_buffer_to_set(descriptor_set_handle set, buffer_handle buffer, u32 binding);
        void write_combined_image_to_set(descriptor_set_handle set, texture_handle texture, sampler_handle sampler, u32 binding);

    public:
        command_buffer_gl create_command_buffer();

    public:
        INTERNAL shader_container& shaders()                        { return mShaders; }
        INTERNAL buffer_container& buffers()                        { return mBuffers; }
        INTERNAL texture_container& textures()                      { return mTextures; }
        INTERNAL combined_texture_container& combined_textures()    { return mCombinedTextures; }
        INTERNAL sampler_container& samplers()                      { return mSamplers; }
        INTERNAL render_target_container& render_targets()          { return mRenderTargets; }
        INTERNAL pipeline_container& pipelines()                    { return mPipelines; }
        INTERNAL descriptor_set_container& descriptor_sets()        { return mDescriptorSets; }

        INTERNAL gl_handle vao() const { return mVAO; }

    private:
        shader_container mShaders                       = { };
        buffer_container mBuffers                       = { };
        texture_container mTextures                     = { };
        combined_texture_container mCombinedTextures    = { };
        sampler_container mSamplers                     = { };
        render_target_container mRenderTargets          = { };
        descriptor_set_container mDescriptorSets        = { };
        pipeline_container mPipelines                   = { };

    private:
        NON_OWNING window_t* rWindow = nullptr;
        NON_OWNING config_t* rConfig = nullptr;
        gl_handle mVAO = invalid_handle;
    };
}
