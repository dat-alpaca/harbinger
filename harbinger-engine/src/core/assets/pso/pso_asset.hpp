#pragma once
#include "rhi/common/objects.hpp"
#include <filesystem>

namespace har
{
    // TODO: field that require spans
    struct pso_asset
    {
        // vertex_input vertexInput    = {};
        rasterizer_t rasterizer;

        // color_blending colorBlending;

        std::filesystem::path vertexShaderPath;
        std::filesystem::path fragmentShaderPath;

        multisampling_t multisampling;
        input_assembly inputAssembly;
    };
}