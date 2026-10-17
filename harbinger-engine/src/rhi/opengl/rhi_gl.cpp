#include <cassert>
#include <glad.h>

#include "core/defines.hpp"
#include "core/engine/engine.hpp"
#include "platform/middleware.hpp"

#include "rhi_gl.hpp"
#include "rhi/opengl/command_buffer_gl.hpp"

namespace har::rhi
{
    rhi_gl::rhi_gl(NON_OWNING harbinger_engine* engine)
    {
        assert(engine);
        rEngine = engine;
    }
    rhi_gl::~rhi_gl()
    {
        /* Blank */
    }

    void rhi_gl::initialize(const rhi_arguments& arguments)
    {
        // VAO:
        glCreateVertexArrays(1, &mVAO);

        // Resources:
        auto* staticAllocator = rEngine->get_main_static_allocator();
        {
            byte* backingBuffer = staticAllocator->push
            (
                mBuffers.required_memory(arguments.maxCombinedTextureCount),
                1
            );
            mCombinedTextures.initialize(backingBuffer, arguments.maxCombinedTextureCount);
        }

        {
            byte* backingBuffer = staticAllocator->push
            (
                mPipelines.required_memory(arguments.maxPipelineCount),
                1
            );
            mPipelines.initialize(backingBuffer, arguments.maxPipelineCount);
        }

        {
            byte* backingBuffer = staticAllocator->push
            (
                mDescriptorSets.required_memory(arguments.maxDescriptorSets),
                1
            );
            mDescriptorSets.initialize(backingBuffer, arguments.maxDescriptorSets);
        }

        {
            byte* backingBuffer = staticAllocator->push
            (
                mRenderTargets.required_memory(arguments.maxRenderTargetCount),
                1
            );
            mRenderTargets.initialize(backingBuffer, arguments.maxRenderTargetCount);
        }

        {
            byte* backingBuffer = staticAllocator->push
            (
                mTextures.required_memory(arguments.maxTextureCount),
                1
            );
            mTextures.initialize(backingBuffer, arguments.maxTextureCount);
        }

        {
            byte* backingBuffer = staticAllocator->push
            (
                mSamplers.required_memory(arguments.maxSamplerCount),
                1
            );
            mSamplers.initialize(backingBuffer, arguments.maxSamplerCount);
        }

        {
            byte* backingBuffer = staticAllocator->push
            (
                mShaders.required_memory(arguments.maxShaderCount),
                1
            );
            mShaders.initialize(backingBuffer, arguments.maxShaderCount);
        }

        {
            byte* backingBuffer = staticAllocator->push
            (
                mBuffers.required_memory(arguments.maxBufferCount),
                1
            );
            mBuffers.initialize(backingBuffer, arguments.maxBufferCount);
        }
    }

    bool rhi_gl::begin_frame()
    {
        glBindVertexArray(mVAO);
        return true;
    }

    void rhi_gl::end_frame()
    {
        /* Blank */
    }

    void rhi_gl::submit([[maybe_unused]] small_handle_t workQueue)
    {
        /* Blank */
    }

    void rhi_gl::present([[maybe_unused]] small_handle_t presentQueue)
    {
        platform::present(rEngine->windowing.window);
    }

    void rhi_gl::execute([[maybe_unused]] command_buffer_gl& commandBuffer)
    {
        /* Blank */
    }

    command_buffer_gl rhi_gl::create_command_buffer()
    {
        return command_buffer_gl(this);
    }
}