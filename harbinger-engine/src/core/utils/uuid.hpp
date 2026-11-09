#pragma once
#include "core/defines.hpp"
#include "core/utils/random.hpp"

namespace har
{
    enum class uuid_t : u64 { };
    constexpr inline uuid_t invalid_uuid = static_cast<uuid_t>(std::numeric_limits<u64>::max());

    inline uuid_t generate_uuid()
	{
		return static_cast<uuid_t>(generate_random<u64>
        (
			std::numeric_limits<u64>::min(), 
			std::numeric_limits<u64>::max() - 1
		));
	}

	inline bool is_uuid_valid(uuid_t uuid)
	{
		return uuid != invalid_uuid;
	}
}