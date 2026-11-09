#pragma once

extern "C"
{
    #include <tlsf.h>
    #include <tlsf_thread.h>
}

#include "core/defines.hpp"
#include "core/memory/memory_tag.hpp"

namespace har
{
    class dynamic_allocator
    {
    public:
        void initialize(void* externalPool, u64 size);

    public:
        void* allocate(memory_tag tag, u64 size, [[maybe_unused]] u8 alignment = 16);
        void* reallocate(memory_tag tag, void* address, u64 newSize);
        void deallocate(memory_tag tag, void* address);

    private:
        tlsf_thread_t mTLSF;
        u64 mUsable = 0;
    };
}