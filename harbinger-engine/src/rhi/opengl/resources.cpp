#include <cstring>
#include <glad.h>
#include <initializer_list>
#include <span>

#include "core/defines.hpp"
#include "core/logging/logger.hpp"

#include "core/memory/allocators.hpp"
#include "core/memory/memory_tag.hpp"
#include "rhi_gl.hpp"
#include "rhi/opengl/descriptor_gl.hpp"
#include "rhi/opengl/shader_compat.hpp"
#include "rhi/opengl/texture_gl.hpp"

#include "rhi/common/format.hpp"
#include "rhi/opengl/handle.hpp"
#include "rhi/common/objects.hpp"
#include "rhi/opengl/pipeline_gl.hpp"
#include "rhi/opengl/render_target_gl.hpp"

// Shaders:
namespace har::rhi
{
    static void check_compile_error(gl_handle shader, shader_type type)
	{
		constexpr static u64 BufferSize = 1024;
		i32 success;

		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
            i32 logLength;
    		char infoLog[BufferSize];
			glGetShaderInfoLog(shader, BufferSize, &logLength, infoLog);
            HAR_LOG_ERROR("OpenGL [{}]: {}", get_shader_type_name(type), infoLog);
		}
	}

    static void check_program_link_errors(gl_handle program)
	{
		constexpr u64 BufferSize = 1024;
		i32 success;

        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
		{
            i32 logLength;
            char infoLog[BufferSize];
			glGetProgramInfoLog(program, BufferSize, &logLength, infoLog);
            HAR_LOG_ERROR("OpenGL [Program]: {}", infoLog);
		}
	}

    shader_handle rhi_gl::create_shader(shader_type type, std::span<u32> spirvData)
    {
        gl_handle shaderType = 0;
        switch(type)
        {
            case har::shader_type::invalid:
                break;
                
            case har::shader_type::vertex: 
                shaderType = GL_VERTEX_SHADER;
                break;
                
            case har::shader_type::fragment:
                shaderType = GL_FRAGMENT_SHADER;
                break;
        }

        har_string contents = gl::convert_spv_to_gl_glsl(*rConfig, spirvData);
        gl_handle shaderID = glCreateShader(shaderType);
        {
            const char* const cstring = static_cast<const char*>(contents.data());
            glShaderSource(shaderID, 1, &cstring, nullptr);
            glCompileShader(shaderID);

            check_compile_error(shaderID, type);
        }

        if (mShaders.size() + 1 > mShaders.capacity())
            return invalid_small_handle;

        mShaders.insert(shaderID);
        return mShaders.size() - 1;
    }

    void rhi_gl::destroy_shader(shader_handle& handle)
    {
        glDeleteShader(mShaders[handle]);
        mShaders.remove(handle);
        handle = invalid_small_handle;
    }
}

// Buffers:
namespace har::rhi
{
    buffer_handle rhi_gl::create_buffer(const buffer_descriptor& descriptor)
    {
        gl_handle flags = 0;
        flags |= GL_DYNAMIC_STORAGE_BIT;
        
        if ((descriptor.bufferUsage & buffer_usage::read) != 0)
            flags |= GL_MAP_READ_BIT;
        if ((descriptor.bufferUsage & buffer_usage::write) != 0)
            flags |= GL_MAP_WRITE_BIT;
        if ((descriptor.bufferUsage & buffer_usage::persistent) != 0)
            flags |= GL_MAP_PERSISTENT_BIT;
        if ((descriptor.bufferUsage & buffer_usage::coherent) != 0)
            flags |= GL_MAP_COHERENT_BIT;

        gl_handle bufferID = invalid_handle;
        glCreateBuffers(1, &bufferID);
        glNamedBufferStorage(bufferID, descriptor.sizeBytes, nullptr, flags);

        if (mBuffers.size() + 1 > mBuffers.capacity())
            return invalid_small_handle;

        mBuffers.insert(bufferID);
        return mBuffers.size() - 1;
    }

    void rhi_gl::destroy_buffer(buffer_handle& handle)
    {
        glDeleteBuffers(1, &mBuffers[handle]);
        mBuffers.remove(handle);
        handle = invalid_small_handle;
    }
}

// Textures:
namespace har::rhi
{
    constexpr static gl_handle get_wrap_mode(sampler_wrap_mode mode)
    {
        switch(mode)
        {
            case sampler_wrap_mode::repeat:
                return GL_REPEAT;

            case sampler_wrap_mode::mirrored_repeat:
                return GL_MIRRORED_REPEAT;

            case sampler_wrap_mode::clamp_to_edge:
                return GL_CLAMP_TO_EDGE;

            // case sampler_wrap_mode::clamp_to_border:
            //     return GL_CLAMP_TO_BORDER;
        }

        return invalid_handle;
    }

    constexpr static gl_handle get_min_filter_mode(sampler_filter filter, bool enableMipFiltering)
    {
        if (!enableMipFiltering)
        {
            switch(filter)
            {
                case sampler_filter::nearest:
                    return GL_NEAREST;
                
                case sampler_filter::linear:
                    return GL_LINEAR;
            }
        }

        switch(filter)
        {
            case sampler_filter::nearest:
                return GL_LINEAR_MIPMAP_NEAREST;

            case sampler_filter::linear:
                return GL_LINEAR_MIPMAP_LINEAR;
        }

        return invalid_handle;
    }

    constexpr static gl_handle get_mag_filter_mode(sampler_filter filter, bool enableMipFiltering)
    {
        switch(filter)
        {
            case sampler_filter::nearest:
                return GL_NEAREST;
                
            case sampler_filter::linear:
                return GL_LINEAR;
        }

        return invalid_handle;
    }

    constexpr static gl_handle get_internal_format(format_t format)
    {
        switch(format)
        {
            case format_t::r_ui8:
                return GL_R8;
            case format_t::r_snorm8: 
                return GL_R8_SNORM;

            case format_t::rg_ui8:
                return GL_RG8;
            case format_t::rg_snorm8:
                return GL_RG8_SNORM;

            case format_t::rgb_ui8:
                return GL_RGB8;
            case format_t::rgb_snorm8:
                return GL_RGB8_SNORM;
            
            case format_t::rgba_ui8:
                return GL_RGBA8;
            case format_t::rgba_snorm8:
                return GL_RGBA8_SNORM;

            case format_t::d16:
                return GL_DEPTH_COMPONENT16;

            case format_t::d32:
                return GL_DEPTH_COMPONENT32F;

            case format_t::d24_s8:
                return GL_DEPTH24_STENCIL8;

            case format_t::rgb_f32:
                return GL_RGB32F;

            // Compressed:
            case format_t::dxt1_rgba:
                return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; 
            
            case format_t::dxt3_rgba:
                return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;

            case format_t::dxt5_rgba:
                return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; 

            default:
                return invalid_handle;
        }

        return invalid_handle;
    }

    texture_handle rhi_gl::create_texture(const texture_descriptor& descriptor)
    {
        texture_gl textureGL;
        textureGL.arraySize = descriptor.arraySize;
        textureGL.dimension = descriptor.dimension; 
        
        // Texture dimensions:
        gl_handle textureDimension = get_gl_dimension(descriptor.dimension);

        // Creation:
        textureGL.handle = invalid_handle;
        glCreateTextures(textureDimension, 1, &textureGL.handle);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

        {
            glTextureParameteri(textureGL.handle, GL_TEXTURE_MAX_LEVEL, descriptor.arraySize);

            if (descriptor.levels > 1)
                glGenerateTextureMipmap(textureGL.handle);
        }

        // Storage:
        gl_handle internalFormat = get_internal_format(descriptor.internalFormat);
        switch(descriptor.dimension)
        {
            case texture_dimension::texture_2d:
            {
                glTextureStorage2D(textureGL.handle, descriptor.levels, internalFormat, descriptor.width, descriptor.height);
                textureGL.width = descriptor.width;
                textureGL.height = descriptor.height;
            } break;

            case texture_dimension::texture_2d_array:
            {
                glTextureStorage3D(textureGL.handle, descriptor.levels, internalFormat, descriptor.width, descriptor.height, descriptor.arraySize);
                textureGL.layerWidth = descriptor.layerWidth;
                textureGL.layerHeight = descriptor.layerHeight;
            } break;

            case texture_dimension::texture_3d:
            {
                glTextureStorage3D(textureGL.handle, descriptor.levels, internalFormat, descriptor.width, descriptor.height, descriptor.arraySize);
                textureGL.width = descriptor.width;
                textureGL.height = descriptor.height;
            } break;

            default:
            {
                HAR_LOG_CRITICAL("OpenGL: Invalid texture dimension");
            } break;
        }

        return mTextures.insert(textureGL);
    }

    sampler_handle rhi_gl::create_sampler(const sampler_descriptor& descriptor)
    {
        gl_handle samplerID;
        glCreateSamplers(1, &samplerID);

        glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_S, get_wrap_mode(descriptor.wrapS));
        glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_T, get_wrap_mode(descriptor.wrapS));
        glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_R, get_wrap_mode(descriptor.wrapS));

        glSamplerParameteri
        (
			samplerID, GL_TEXTURE_MIN_FILTER, get_min_filter_mode(descriptor.minFilter, descriptor.enableMipFiltering)
		);
		glSamplerParameteri
        (
            samplerID, GL_TEXTURE_MAG_FILTER, get_mag_filter_mode(descriptor.magFilter, descriptor.enableMipFiltering)
        );

        if (mSamplers.size() + 1 > mSamplers.capacity())
            return invalid_small_handle;

        return mSamplers.insert(samplerID);
    }

    void rhi_gl::destroy_texture(texture_handle& handle)
    {
        glDeleteTextures(1, &mTextures[handle].handle);
        mTextures.remove(handle);
        handle = invalid_small_handle;
    }

    void rhi_gl::destroy_sampler(sampler_handle& handle)
    {
        mSamplers.remove(handle);
        handle = invalid_small_handle;
    }
}

// Combined Texture:
namespace har::rhi
{
    combined_texture_handle rhi_gl::create_combined_texture(const combined_texture_descriptor& descriptor)
    {
        return mCombinedTextures.insert(descriptor);
    }
}

// Render Targets:
namespace har::rhi
{
    render_target_handle rhi_gl::create_render_target(const render_target_descriptor& descriptor)
    {
        render_target_gl renderTargetGL;
        glCreateFramebuffers(1, &renderTargetGL.renderTargetHandle);

        const auto& renderTarget = renderTargetGL.renderTargetHandle;

        // Color attachment:
        u32 index = 0;
        for (const auto& [texture, sampler, __format] : descriptor.colorAttachments)
        {
            gl_handle textureID = mTextures[texture].handle;
            
            glNamedFramebufferTexture(renderTarget, GL_COLOR_ATTACHMENT0 + index, textureID, 0);
            renderTargetGL.colorAttachments.push_back({ texture });
            ++index;
        }
        
        // Depth attachment:
        const auto& [texture, sampler, __format] = descriptor.depthAttachment;
        if (texture != invalid_small_handle)
        {
            gl_handle textureID = mTextures[texture].handle;
            glNamedFramebufferTexture(renderTarget, GL_DEPTH_ATTACHMENT, textureID, 0);

    		renderTargetGL.depthAttachment = texture;
        }

        // Render buffer:
        gl_handle& renderBuffer = renderTargetGL.renderBufferHandle;
        glCreateRenderbuffers(1, &renderBuffer);
		glNamedRenderbufferStorage(renderBuffer, GL_DEPTH24_STENCIL8, descriptor.width, descriptor.height);
		glNamedFramebufferRenderbuffer(renderTarget, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
		
        i32 code = glCheckNamedFramebufferStatus(renderTarget, GL_FRAMEBUFFER);
        if (code != GL_FRAMEBUFFER_COMPLETE)
            HAR_LOG_CRITICAL("OpenGL: Incomplete framebuffer. Status code: {}", code);

        if (mRenderTargets.size() + 1 > mRenderTargets.capacity())
            return invalid_small_handle;

        mRenderTargets.insert(renderTargetGL);
        return mRenderTargets.size() - 1;
    }

    void rhi_gl::destroy_render_target(render_target_handle& handle)
    {
        render_target_gl& renderTarget = mRenderTargets[handle];

        for (auto& colorAttachment : renderTarget.colorAttachments)
            destroy_texture(colorAttachment);
        
        destroy_texture(renderTarget.depthAttachment);

        glDeleteFramebuffers(1, &renderTarget.renderTargetHandle);
        glDeleteRenderbuffers(1, &renderTarget.renderBufferHandle);

        mRenderTargets.remove(handle);
        handle = invalid_small_handle;
    }
}

// Pipelines:
namespace har::rhi
{
    graphics_pipeline_handle rhi_gl::create_graphics_pipeline(graphics_pipeline_descriptor&& descriptor)
    {
        UNUSED descriptor.multisampling;

        pipeline_gl pipeline;
        pipeline.handle = glCreateProgram();
        
        // Vertex input copying:
        {
            u32 maxAttributeCount = rConfig->rhiArguments.maxVertexAttributesPerPipeline;
            HAR_ASS(descriptor.vertexInput.attributes.size() <= maxAttributeCount);

            auto& pipelineAttributes = pipeline.vertexInput.attributes;
            pipeline.vertexInput.stride = descriptor.vertexInput.stride;
            
            for (const auto& attribute : descriptor.vertexInput.attributes)
                pipelineAttributes.push_back(attribute);
        }

        // Color Blending:
        {
            // TODO: max color blending (config)
            pipeline.colorBlending.enableBlending = descriptor.colorBlending.enableBlending;

            for (const auto& attachment : descriptor.colorBlending.attachments)
                pipeline.colorBlending.attachments.push_back(attachment);
        }
        
        pipeline.inputAssembly = descriptor.inputAssembly;
        pipeline.rasterizer = descriptor.rasterizer;

        if (descriptor.vertexShader != invalid_small_handle)
            glAttachShader(pipeline.handle, mShaders[descriptor.vertexShader]);

        if (descriptor.fragmentShader != invalid_small_handle)
            glAttachShader(pipeline.handle, mShaders[descriptor.fragmentShader]);

        glLinkProgram(pipeline.handle);

        check_program_link_errors(pipeline.handle);

        destroy_shader(descriptor.vertexShader);
        destroy_shader(descriptor.fragmentShader);

        if (mPipelines.size() + 1 > mPipelines.capacity())
            return invalid_small_handle;

        mPipelines.insert(std::move(pipeline));
        return mPipelines.size() - 1;
    }

    void rhi_gl::destroy_graphics_pipeline(graphics_pipeline_handle& handle)
    {
        mPipelines[handle].vertexInput.attributes.clear();

        glDeleteProgram(mPipelines[handle].handle);
        mPipelines.remove(handle);
        handle = invalid_small_handle;
    }
}

// Descriptor set:
namespace har::rhi
{
    using namespace har::internal;

    descriptor_set_layout_handle rhi_gl::create_descriptor_set_layout(const descriptor_set_layout& layout)
    {
        if (mDescriptorSets.size() + 1 > mDescriptorSets.capacity())
            return invalid_small_handle;

        descriptor_gl descriptor;

        mDescriptorSets.insert(descriptor);
        auto index = mDescriptorSets.size() - 1;

        return index;
    }
    
    descriptor_set_handle rhi_gl::create_descriptor_set(descriptor_set_layout_handle handle)
    {
        return static_cast<descriptor_set_handle>(handle);
    }

    void rhi_gl::write_uniform_buffer_to_set(descriptor_set_handle set, std::initializer_list<buffer_handle> buffers, u32 binding)
    {   
        u32 amount = buffers.size();

        auto* handles = static_cast<buffer_handle*>(MainDynamicAllocator.allocate(memory_tag::rhi, amount * sizeof(buffer_handle)));
        std::memcpy(handles, buffers.begin(), amount * sizeof(buffer_handle));

        mDescriptorSets[set].descriptors.push_back
        ({ 
            .handles = handles, 
            .binding = static_cast<u16>(binding),
            .amount = static_cast<u16>(amount),
            .type = descriptor_set_type::uniform_buffer,
        });
    }
    void rhi_gl::write_storage_buffer_to_set(descriptor_set_handle set, std::initializer_list<buffer_handle> buffers, u32 binding)
    {
        u32 amount = buffers.size();
        auto* handles = static_cast<small_handle_t*>(MainDynamicAllocator.allocate(memory_tag::rhi, amount * sizeof(small_handle_t)));
        std::memcpy(handles, buffers.begin(), amount * sizeof(buffer_handle));

        mDescriptorSets[set].descriptors.push_back
        ({ 
            .handles = handles,
            .binding = static_cast<u16>(binding),
            .amount = static_cast<u16>(amount),
            .type = descriptor_set_type::storage_buffer,
        });
    }

    void rhi_gl::write_combined_image_to_set(descriptor_set_handle set, std::initializer_list<texture_handle> textures, std::initializer_list<sampler_handle> samplers, u32 binding)
    {
        HAR_ASS(textures.size() == samplers.size());
        u32 amount = textures.size();

        auto* handles = static_cast<small_handle_t*>(MainDynamicAllocator.allocate(memory_tag::rhi, 2 * amount * sizeof(small_handle_t)));
        auto* handleIt = handles;

        auto textureIt = textures.begin();
        auto samplerIt = samplers.begin();
        for (u64 i = 0; i < amount; ++i)
        {
            *handleIt++ = *textureIt++;
            *handleIt++ = *samplerIt++;
        }

        mDescriptorSets[set].descriptors.push_back
        ({ 
            .handles = handles,
            .binding = static_cast<u16>(binding),
            .amount = static_cast<u16>(2 * amount),
            .type = descriptor_set_type::combined_texture,
        });
    }

    void rhi_gl::write_combined_image_to_set(descriptor_set_handle set, std::initializer_list<combined_texture_handle> textures, u32 binding)
    {
        u32 amount = textures.size();

        auto* handles = static_cast<small_handle_t*>(MainDynamicAllocator.allocate(memory_tag::rhi, 2 * amount * sizeof(small_handle_t)));
        auto* handleIt = handles;

        for (const auto& texture : textures)
        {
            const auto& textureHandle = combined_textures()[texture].textureHandle;
            const auto& samplerHandle = combined_textures()[texture].samplerHandle;
        
            *handleIt++ = textureHandle;
            *handleIt++ = samplerHandle;
        }

        mDescriptorSets[set].descriptors.push_back
        ({ 
            .handles = handles,
            .binding = static_cast<u16>(binding),
            .amount = static_cast<u16>(2 * amount),
            .type = descriptor_set_type::combined_texture,
        });
    }
    
    void rhi_gl::destroy_descriptor_set(descriptor_set_handle& handle)
    {   
        for (u64 i = 0; i < mDescriptorSets.size(); ++i)
        {
            auto& set = mDescriptorSets[i];
            for (auto& descriptor : set.descriptors)
                MainDynamicAllocator.deallocate(memory_tag::rhi, descriptor.handles);
        }

        mDescriptorSets[handle].descriptors.clear();

        mDescriptorSets.remove(handle);
        handle = invalid_small_handle;
    }
}