#pragma once
#include <filesystem>
#include "core/assets/asset.hpp"
#include "rhi/common/objects.hpp"

namespace har
{
    // TODO: field that require spans
    struct pso_asset : public asset
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