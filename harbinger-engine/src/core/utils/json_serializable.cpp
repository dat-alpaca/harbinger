#include "json_serializable.hpp"
#include "nlohmann/json.hpp"
#include "rhi/common/objects.hpp"

/* vec2, vec3 */
namespace glm
{
    void to_json(nlohmann::json& jsonObject, const glm::vec2& descriptor)
    {
        jsonObject = nlohmann::json
        {
            { "x", descriptor.x },  
            { "y", descriptor.y },  
        };
    }
    void from_json(const nlohmann::json& jsonObject, glm::vec2& descriptor)
    {
        jsonObject.at("x").get_to(descriptor.x);
        jsonObject.at("y").get_to(descriptor.y);
    }

    void to_json(nlohmann::json& jsonObject, const glm::vec3& descriptor)
    {
        jsonObject = nlohmann::json
        {
            { "x", descriptor.x },  
            { "y", descriptor.y },  
            { "z", descriptor.z },  
        };
    }
    void from_json(const nlohmann::json& jsonObject, glm::vec3& descriptor)
    {
        jsonObject.at("x").get_to(descriptor.x);
        jsonObject.at("y").get_to(descriptor.y);
        jsonObject.at("z").get_to(descriptor.z);
    }
}

/* rasterizer */
namespace har
{
    void to_json(nlohmann::json& jsonObject, const rasterizer_t& descriptor)
    {
        jsonObject = nlohmann::json
        {
            { "front_face", descriptor.frontFace },  
            { "cull_mode", descriptor.cullMode },
            { "polygon_mode", descriptor.polygonMode },  
            { "enable_depth_clamp", static_cast<u8>(descriptor.enableDepthClamp) },
            { "enable_discard", static_cast<u8>(descriptor.enableDiscard) },
            { "enable_depth_bias", static_cast<u8>(descriptor.enableDepthBias) },
        };
    }
    void from_json(const nlohmann::json& jsonObject, rasterizer_t& descriptor)
    {
        descriptor.frontFace   = static_cast<front_face>(jsonObject.at("front_face").get<front_face>());
        descriptor.cullMode    = static_cast<cull_mode>(jsonObject.at("cull_mode").get<cull_mode>());
        descriptor.polygonMode = static_cast<polygon_mode>(jsonObject.at("polygon_mode").get<polygon_mode>());
        
        descriptor.enableDepthClamp = static_cast<u8>(jsonObject.at("enable_depth_clamp").get<u8>());
        descriptor.enableDiscard    = static_cast<u8>(jsonObject.at("enable_discard").get<u8>());
        descriptor.enableDepthBias  = static_cast<u8>(jsonObject.at("enable_depth_bias").get<u8>());
    }
}

/* color blending */
namespace har
{
    void to_json(nlohmann::json& jsonObject, const color_blending& descriptor)
    {
        jsonObject = nlohmann::json
        {
            { "enable_blending", descriptor.enableBlending },
        };
        jsonObject["attachments"] = nlohmann::json::array();
    }
    void from_json(const nlohmann::json& jsonObject, color_blending& descriptor)
    {
        descriptor.enableBlending = static_cast<u8>(jsonObject.at("enable_blending").get<u8>());
        
        if (!jsonObject.contains("attachments"))
            return;

        for (const auto& attachment : jsonObject["attachments"])
        {
            if (attachment.contains("inherit") && attachment["inherit"] == "alpha_blending_default")
            {
                // TODO: do it
                // descriptor.attachments;
            }
        }
    }
}

/* input assembly */
namespace har
{
    void to_json(nlohmann::json& jsonObject, const input_assembly& descriptor)
    {
        jsonObject = nlohmann::json
        {
            { "topology", descriptor.topology },  
            { "primitive_restart_enable", static_cast<u8>(descriptor.primitiveRestartEnable) }
        };
    }
    void from_json(const nlohmann::json& jsonObject, input_assembly& descriptor)
    {
        descriptor.topology = static_cast<topology_t>(jsonObject.at("topology").get<topology_t>());
        descriptor.primitiveRestartEnable = static_cast<u8>(jsonObject.at("primitive_restart_enable").get<u8>());
    }
}

/* pso_asset */
namespace har
{
    void to_json(nlohmann::json& jsonObject, const pso_asset& descriptor)
    {
        jsonObject = nlohmann::json
        {
            { "vertex_shader", descriptor.vertexShaderPath },
            { "fragment_shader", descriptor.fragmentShaderPath },  
            { "multisampling", descriptor.multisampling },
            { "input_assembly", descriptor.inputAssembly },
            { "rasterizer", descriptor.rasterizer },  
        };
    }
    void from_json(const nlohmann::json& jsonObject, pso_asset& descriptor)
    {
        descriptor.vertexShaderPath = jsonObject.at("vertex_shader").get<har_string>();
        descriptor.fragmentShaderPath = jsonObject.at("fragment_shader").get<har_string>();
        descriptor.multisampling = jsonObject.at("multisampling").get<multisampling_t>();
        descriptor.inputAssembly = jsonObject.at("input_assembly").get<input_assembly>();
        descriptor.rasterizer = jsonObject.at("rasterizer").get<rasterizer_t>();
    }
}

/* transform */
namespace har
{
    void to_json(nlohmann::json& jsonObject, const transform_t& descriptor)
    {
        jsonObject = nlohmann::json
        {
            { "position", descriptor.position },  
            { "rotation", descriptor.rotation },  
            { "scale", descriptor.scale },  
        };
    }
    void from_json(const nlohmann::json& jsonObject, transform_t& descriptor)
    {
        jsonObject.at("position").get_to(descriptor.position);
        jsonObject.at("rotation").get_to(descriptor.rotation);
        jsonObject.at("scale").get_to(descriptor.scale);
    }
}

/* texture_descriptor */
namespace har
{
    void to_json(nlohmann::json& jsonObject, const texture_descriptor& descriptor)
    {
        jsonObject = nlohmann::json
        {
            { "width", descriptor.width },  
            { "height", descriptor.height },
            { "layerWidth", descriptor.width },  
            { "layerHeight", descriptor.height },

            { "depth", descriptor.depth },
            { "arraySize", descriptor.arraySize },

            { "dimension", descriptor.dimension },
            { "internalFormat", descriptor.internalFormat },
            { "levels", descriptor.levels },
        };
    }

    void from_json(const nlohmann::json& jsonObject, texture_descriptor& descriptor) 
    {
        jsonObject.at("width").get_to(descriptor.width);
        jsonObject.at("height").get_to(descriptor.width);
        jsonObject.at("layerWidth").get_to(descriptor.layerWidth);
        jsonObject.at("layerHeight").get_to(descriptor.layerHeight);
        
        jsonObject.at("depth").get_to(descriptor.depth);
        jsonObject.at("arraySize").get_to(descriptor.arraySize);
        jsonObject.at("dimension").get_to(descriptor.dimension);
        jsonObject.at("internalFormat").get_to(descriptor.internalFormat);
        jsonObject.at("levels").get_to(descriptor.levels);
    }
}

/* sampler_descriptor */
namespace har
{
    void to_json(nlohmann::json& jsonObject, const sampler_descriptor& descriptor)
    {
        jsonObject = nlohmann::json
        {
            { "wrapS", descriptor.wrapS },  
            { "wrapT", descriptor.wrapT },
            { "wrapR", descriptor.wrapR },  

            { "magFilter", descriptor.magFilter },
            { "minFilter", descriptor.minFilter },

            { "enableMipFiltering", static_cast<u8>(descriptor.enableMipFiltering) },
        };
    }

    void from_json(const nlohmann::json& jsonObject, sampler_descriptor& descriptor) 
    {
        descriptor.wrapS = static_cast<sampler_wrap_mode>(jsonObject.at("wrapS").get<sampler_wrap_mode>());
        descriptor.wrapT = static_cast<sampler_wrap_mode>(jsonObject.at("wrapS").get<sampler_wrap_mode>());
        descriptor.wrapR = static_cast<sampler_wrap_mode>(jsonObject.at("wrapS").get<sampler_wrap_mode>());

        descriptor.magFilter = static_cast<sampler_filter>(jsonObject.at("magFilter").get<sampler_filter>());
        descriptor.minFilter = static_cast<sampler_filter>(jsonObject.at("minFilter").get<sampler_filter>());

        descriptor.enableMipFiltering = static_cast<u8>(jsonObject.at("enableMipFiltering").get<u32>());
    }
}