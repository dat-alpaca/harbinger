#pragma once
#include "core/config/config.hpp"

namespace har
{
    inline u32 get_glsl_version(const har::config_t& config)
	{
		return config.apiMajor * 100 + config.apiMinor * 10;
	}
}