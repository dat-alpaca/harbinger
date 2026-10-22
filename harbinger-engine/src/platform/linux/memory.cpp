#include "platform/memory.hpp"
#include "core/assert/assert.hpp"
#include "core/utils/memory_utils.hpp"

#include <cstdlib>

namespace har::platform
{
    void* allocate(u64 size)
    {
        void* address = malloc(size);
        HAR_ASS(address);
        return address;
    }

    void* reallocate(void* address, u64 newSize)
    {
        HAR_ASS(address);
        return realloc(address, newSize);
    }

    void deallocate(void* address)
    {
        HAR_ASS(address);
        free(address);
    }

    void* allocate_aligned(u64 size, u8 alignment)
    {
        HAR_ASS(is_power_of_two(alignment));

        void* address = aligned_alloc(size, alignment);
        HAR_ASS(address);

        return address;
    }
}
