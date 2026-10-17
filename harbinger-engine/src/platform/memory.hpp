#pragma once
#include "core/defines.hpp"

namespace har::platform
{
    void* allocate(u64 size);
    void* reallocate(void* address, u64 newSize);
    void  deallocate(void* address);

    void* allocate_aligned(u64 size, u8 alignment);
}
