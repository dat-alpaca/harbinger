#include "platform/memory.hpp"
#include "core/utils/memory_utils.hpp"

#include <cassert>
#include <cstdlib>

namespace har::platform
{
    void* allocate(u64 size)
    {
        void* address = malloc(size);
        assert(address);
        return address;
    }

    void* reallocate(void* address, u64 newSize)
    {
        assert(address);
        return realloc(address, newSize);
    }

    void deallocate(void* address)
    {
        assert(address);
        free(address);
    }

    void* allocate_aligned(u64 size, u8 alignment)
    {
        assert(is_power_of_two(alignment));

        void* address = aligned_alloc(size, alignment);
        assert(address);

        return address;
    }
}
