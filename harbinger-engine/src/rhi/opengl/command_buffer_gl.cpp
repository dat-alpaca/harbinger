#include <glad.h>
#include "command_buffer_gl.hpp"
#include "core/defines.hpp"
#include "core/logging/logger.hpp"
#include "rhi/common/clear_flags.hpp"
#include "rhi/common/objects.hpp"
#include "rhi/opengl/handle.hpp"
#include "rhi_gl.hpp"

namespace har::rhi
{
    command_buffer_gl::command_buffer_gl(rhi_gl* rhi)
        : rRHI(rhi)
    {
    }

    void command_buffer_gl::begin(render_target_handle handle)
    {
        set_render_target(handle);
        clear();
    }
    void command_buffer_gl::end()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void command_buffer_gl::set_render_target(render_target_handle handle)
    {
        if (handle == invalid_small_handle)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, rRHI->render_targets()[handle].renderBufferHandle);
    }
    void command_buffer_gl::set_viewport(const rect_i32& viewport)
    {
        glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    }
    void command_buffer_gl::set_scissor(const rect_i32& scissor)
    {
        glScissor(scissor.x, scissor.y, scissor.width, scissor.height);
    }
    void command_buffer_gl::set_clear_color(const color_t& color, clear_flags flags)
    {
        glClearColor(color.r, color.g, color.b, color.a);
        mClearFlags = flags;
    }
}

namespace har::rhi
{
    static constexpr gl_handle get_front_face(front_face frontFace)
    {
        switch(frontFace)
        {
            case front_face::cw: return GL_CW; 
            case front_face::ccw: return GL_CCW;
            default: 
                return invalid_handle;
        }
    }

    static constexpr gl_handle get_polygon_mode(polygon_mode polygonMode)
    {
        switch(polygonMode)
        {
            case polygon_mode::point: return GL_POINT; 
            case polygon_mode::line: return GL_LINE; 
            case polygon_mode::fill: return GL_FILL; 
            default: 
                return invalid_handle;
        }
    }

    static constexpr gl_handle get_attribute_format(format_t attributeFormat)
    {
        switch(attributeFormat)
        {
            case format_t::r_ui8:
            case format_t::r_snorm8:
            case format_t::rg_ui8:     
            case format_t::rg_snorm8:
            case format_t::rgb_ui8:    
            case format_t::rgb_snorm8: 
            case format_t::rgba_ui8:   
            case format_t::rgba_snorm8:
                return GL_UNSIGNED_BYTE;

            case format_t::rgb_f32:
                return GL_FLOAT;

            default:
                return invalid_handle;
        }
    }

    static constexpr u32 get_attribute_format_size(format_t attributeFormat)
    {
        switch(attributeFormat)
        {
            case format_t::r_ui8:
            case format_t::r_snorm8:
                return 1;
            case format_t::rg_ui8:     
            case format_t::rg_snorm8:
                return 2;

            case format_t::rgb_ui8:    
            case format_t::rgb_snorm8:
                return 3;
            
            case format_t::rgba_ui8:   
            case format_t::rgba_snorm8:
                return 4;
            
            case format_t::rgb_f32:
                return 3;

            default:
                return invalid_handle;
        }
    }

    static constexpr gl_handle get_cull_mode(cull_mode mode)
    {
        switch(mode)
        {
            case cull_mode::none: return GL_NONE;
            case cull_mode::front: return GL_FRONT;
            case cull_mode::back: return GL_BACK;
            case cull_mode::both: return GL_FRONT_AND_BACK;
            default:
            {
                HAR_LOG_CRITICAL("OpenGL: Invalid cull mode");
                return invalid_handle;
            }
        }
    }

    static constexpr gl_handle get_color_blend_op(blend_operation op)
    {
        switch(op)
        {
            case blend_operation::add: return GL_FUNC_ADD;
            case blend_operation::subtract: return GL_FUNC_SUBTRACT;
            case blend_operation::min: return GL_MIN;
            case blend_operation::max: return GL_MAX;
            default:
            {
                HAR_LOG_CRITICAL("OpenGL: Invalid blend operation");
                return invalid_handle;
            }
        }
    }

    static constexpr gl_handle get_color_blend_factor(blend_factor factor)
    {
        switch(factor)
        {
            case blend_factor::zero: return GL_ZERO;
            case blend_factor::one: return GL_ONE;
            case blend_factor::one_minus_src_alpha: return GL_ONE_MINUS_SRC_ALPHA;
            case blend_factor::src_color: return GL_SRC_COLOR;
            case blend_factor::src_alpha: return GL_SRC_ALPHA;
            default:
            {
                HAR_LOG_CRITICAL("OpenGL: Invalid blend factor");
                return invalid_handle;
            }
        }
    }

    void command_buffer_gl::bind_graphics_pipeline(graphics_pipeline_handle handle)
    {
        auto& pipeline = rRHI->pipelines()[handle];
        mActiveGraphicsPipeline = handle; 

        gl_handle vao = rRHI->vao();

        glUseProgram(pipeline.handle);
        glEnable(GL_PROGRAM_POINT_SIZE);

        // Vertex Input:
        const auto& attributes = pipeline.vertexInput.attributes;
        for (u16 i = 0; i < attributes.size(); ++i)
        {
            const auto& attribute = attributes[i];

            glEnableVertexArrayAttrib(vao, attribute.location);

            glVertexArrayAttribFormat
            (
				vao, 
                attribute.location,
                static_cast<i32>(get_attribute_format_size(attribute.vertexFormat)),
				get_attribute_format(attribute.vertexFormat), 
                false, 
                attribute.offset
			);

            glVertexArrayAttribBinding(vao, attribute.location, attribute.binding);

            u32 divisor = !attribute.instanced ? 0 : 1;
			glVertexArrayBindingDivisor(vao, attribute.binding, divisor);
        }

        // Rasterizer:
        glDisable(GL_CULL_FACE);
        glFrontFace(get_front_face(pipeline.rasterizer.frontFace));
        glPolygonMode(GL_FRONT_AND_BACK, get_polygon_mode(pipeline.rasterizer.polygonMode));

        gl_handle cullMode = get_cull_mode(pipeline.rasterizer.cullMode);
        if (cullMode != GL_NONE)
        {
            glCullFace(cullMode);
			glEnable(GL_CULL_FACE);
        }

        // Color blending:
        if (!pipeline.colorBlending.enableBlending)
        {
			glDisable(GL_BLEND);
            return;
        }

        glEnable(GL_BLEND);
        for (const auto& attachment : pipeline.colorBlending.attachments)
        {
            gl_handle colorBlendOp = get_color_blend_op(attachment.colorBlendOp);
            gl_handle alphaBlendOp = get_color_blend_op(attachment.alphaBlendOp);

            gl_handle colorSrcFactor = get_color_blend_factor(attachment.srcColorBlendFactor);
            gl_handle colorDstFactor = get_color_blend_factor(attachment.dstColorBlendFactor);
            
            gl_handle alphaSrcFactor = get_color_blend_factor(attachment.srcAlphaBlendFactor);
            gl_handle alphaDstFactor = get_color_blend_factor(attachment.dstAlphaBlendFactor);

            glBlendFuncSeparate(colorSrcFactor, colorDstFactor, alphaSrcFactor, alphaDstFactor);
		    glBlendEquationSeparate(colorBlendOp, alphaBlendOp);
        }
    }

    void command_buffer_gl::bind_vertex_buffer(buffer_handle handle, u8 binding)
    {
        gl_handle buffer = rRHI->buffers()[handle];
        auto& pipeline = rRHI->pipelines()[mActiveGraphicsPipeline];
        glVertexArrayVertexBuffer(rRHI->vao(), binding, buffer, 0, pipeline.vertexInput.stride);
    }

    void command_buffer_gl::bind_index_buffer(buffer_handle handle)
    {
        gl_handle buffer = rRHI->buffers()[handle];
        glVertexArrayElementBuffer(rRHI->vao(), buffer);
    }

    void command_buffer_gl::bind_descriptor_set(descriptor_set_handle handle)
    {
        const auto& set = rRHI->descriptor_sets()[handle];
        for(u32 i = 0; i < set.descriptors.size(); ++i)
        {
            const auto& descriptor = set.descriptors[i];
            u16 amount = descriptor.amount > 0 ? descriptor.amount : 1;

            switch(descriptor.type)
            {
                case descriptor_set_type::uniform_buffer:
                {
                    for (u16 i = 0; i < amount; ++i)
                    {
                        auto& buffer = rRHI->buffers()[descriptor.handles[i]];
                        u32 binding = descriptor.binding + i;

                        glBindBufferBase(GL_UNIFORM_BUFFER, binding, buffer);
                    }
                } break;

                case descriptor_set_type::storage_buffer:
                {
                    for (u16 i = 0; i < amount; ++i)
                    {
                        auto& buffer = rRHI->buffers()[descriptor.handles[i]];
                        u32 binding = descriptor.binding + i;

                        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer);
                    }
                } break;

                case descriptor_set_type::combined_texture:
                {
                    for (u16 i = 0; i < amount; i += 2)
                    {
                        auto& texture = rRHI->textures()[descriptor.handles[i + 0]];
                        auto& sampler = rRHI->samplers()[descriptor.handles[i + 1]];

                        u32 unit = descriptor.binding + i;

                        glBindTextureUnit(unit, texture.handle);
                        glBindSampler(unit, sampler);
                    }
                } break;

                default:
                {
                    HAR_LOG_CRITICAL("OpenGL: Invalid descriptor set type");
                } break;
            }
        }
    }
}

namespace har::rhi
{
    constexpr static gl_handle get_data_format(format_t format)
    {
        switch(format)
        {
            case format_t::r_ui8:
            case format_t::r_snorm8: 
                return GL_RED;

            case format_t::rg_ui8:
            case format_t::rg_snorm8:
                return GL_RG;

            case format_t::rgb_ui8:
            case format_t::rgb_snorm8:
                return GL_RGB;
            
            case format_t::rgba_ui8:
            case format_t::rgba_snorm8:
                return GL_RGBA;

            case format_t::rgb_f32:
                return GL_RGB;

            // Compressed:
            // TODO: check if i should use GL_RGBA instead
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

    constexpr static gl_handle get_data_type(data_type type)
    {
        switch(type)
        {
            case data_type::unsigned_byte:
                return GL_UNSIGNED_BYTE;

            case data_type::signed_byte:
                return GL_BYTE;

            case data_type::unsigned_short:
                return GL_UNSIGNED_SHORT;
            
            case data_type::signed_short:
                return GL_SHORT;

            case data_type::unsigned_int:
                return GL_UNSIGNED_INT;

            case data_type::signed_int:
                return GL_INT;

            case data_type::float_t:
                return GL_FLOAT;
        }

        return invalid_handle;
    }

    constexpr static u8 get_data_type_size(data_type type)
    {
        switch(type)
        {
            case data_type::unsigned_byte:
                return sizeof(byte);

            case data_type::signed_byte:
                return sizeof(byte);

            case data_type::unsigned_short:
                return sizeof(u16);
            
            case data_type::signed_short:
                return sizeof(u16);

            case data_type::unsigned_int:
                return sizeof(u32);

            case data_type::signed_int:
                return sizeof(u32);

            case data_type::float_t:
                return sizeof(f32);
        }

        return invalid_micro_handle;
    }

    void command_buffer_gl::update_buffer(buffer_handle handle, std::span<const byte> buffer, u64 offset)
    {
        gl_handle bufferHandle = rRHI->buffers()[handle];
        glNamedBufferSubData(bufferHandle, offset, buffer.size(), buffer.data());
    }

    void command_buffer_gl::update_texture(texture_handle handle, const texture_asset& textureAsset, format_t dataFormat, data_type dataType)
    {
        /* Obs: textureAsset must contain the "true width and height", whereas the texture_handle contains the layer width and height "*/

        // TODO: check if buffer handle exists
        const auto& textureGL = rRHI->textures()[handle];
        
        gl_handle textureDataFormat = get_data_format(dataFormat);
        gl_handle textureDataType = get_data_type(dataType);
        
        byte* data = static_cast<byte*>(textureAsset.data);
        
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        switch(textureGL.dimension)
        {
            case texture_dimension::texture_2d:
            {
                glTextureSubImage2D
                (
					textureGL.handle, 0, 0, 0, 
                    textureAsset.width, 
                    textureAsset.height, 
                    textureDataFormat, 
                    textureDataType, 
                    data
				);
            } break;

            case texture_dimension::texture_2d_array:
            {
                for(u32 i = 0; i < textureGL.arraySize; ++i)
                    update_texture_layer(handle, textureAsset, dataFormat, dataType, i);
            } break;

            case texture_dimension::texture_3d:
                glTextureSubImage3D
                (
					textureGL.handle, 0, 0, 0, 0, textureAsset.width, textureAsset.height, 1, textureDataFormat, textureDataType, data
				);
                break;

            default:
            {
                HAR_LOG_CRITICAL("OpenGL: Invalid texture dimension");
            } break;
        }

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }

    void command_buffer_gl::update_texture_layer(texture_handle handle, const texture_asset& textureAsset, format_t dataFormat, data_type dataType, u32 layer)
    {
        const auto& textureGL = rRHI->textures()[handle];

        if (textureGL.dimension != texture_dimension::texture_2d_array)
        {
            HAR_LOG_ERROR("Attempted to modify texture layer of a non array-texture object");
            return;
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, textureAsset.width);

        u8 dataTypeSize = get_data_type_size(dataType);

        gl_handle textureDataFormat = get_data_format(dataFormat);
        gl_handle textureDataType = get_data_type(dataType);
        byte* data = static_cast<byte*>(textureAsset.data);

        glTextureSubImage3D
        (
            textureGL.handle,
            0,
            0, 0, layer,
            textureGL.layerWidth, textureGL.layerHeight, 1,
            textureDataFormat,
            textureDataType,
            data + layer * (textureGL.layerWidth * dataTypeSize)
        );

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }
}

namespace har::rhi
{
    void command_buffer_gl::copy_texture(texture_handle src, texture_handle dst, texture_copy_loc srcLoc, texture_copy_loc dstLoc, texture_copy_dimens srcDimension)
    {
        const auto& srcGL = rRHI->textures()[src];
        const auto& dstGL = rRHI->textures()[dst];
        // TODO: check src and dst internal format compatibility: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glCopyImageSubData.xhtml

        auto srcTarget = get_gl_dimension(srcGL.dimension);
        auto dstTarget = get_gl_dimension(dstGL.dimension);

        glCopyImageSubData
        (
            srcGL.handle,
            srcTarget,
            srcLoc.level,
            srcLoc.x, srcLoc.y, srcLoc.z,

            dstGL.handle,
            dstTarget,
            dstLoc.level,
            dstLoc.x, dstLoc.y, dstLoc.z,

            srcDimension.width, srcDimension.height, srcDimension.depth
        );
    }
}

namespace har::rhi
{
    static gl_handle get_topology(topology_t topology)
    {
        switch (topology)
        {
            case topology_t::points: return GL_POINTS;
            case topology_t::lines: return GL_LINES;
            case topology_t::line_strips: return GL_LINE_STRIP;
            case topology_t::triangles: return GL_TRIANGLES;
            case topology_t::triangle_strips: return GL_TRIANGLE_STRIP;
            default:
            {
                HAR_LOG_CRITICAL("OpenGL: Invalid topology")
                return invalid_handle;
            } break;
        }
    }

    void command_buffer_gl::draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance)
    {
        auto& pipeline = rRHI->pipelines()[mActiveGraphicsPipeline];

        glDrawArraysInstancedBaseInstance
        (
            get_topology(pipeline.inputAssembly.topology),
            firstVertex,
            vertexCount,
            instanceCount,
            firstInstance
        );
    }

    void command_buffer_gl::draw_indexed(u32 indexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance)
    {
        auto& pipeline = rRHI->pipelines()[mActiveGraphicsPipeline];
        
        glDrawElementsInstancedBaseVertexBaseInstance
        (
            get_topology(pipeline.inputAssembly.topology),
            indexCount,
            GL_UNSIGNED_INT, // TODO: allow other element types
            nullptr,
            instanceCount,
            firstVertex,
            firstInstance
        );
    }
}

namespace har::rhi
{
    void command_buffer_gl::clear()
    {
        // TODO: fix

        gl_handle clearBits = 0;
        if ((mClearFlags & clear_flags::color) != 0)
            clearBits |= GL_COLOR_BUFFER_BIT;

        if ((mClearFlags & clear_flags::depth) != 0)
            clearBits |= GL_DEPTH_BUFFER_BIT;
        
        if ((mClearFlags & clear_flags::stencil) != 0)
            clearBits |= GL_STENCIL_BUFFER_BIT;

        glClear(clearBits);
    }
}