#include "dynamic_allocator.hpp"
#include "core/assert/assert.hpp"
#include "core/memory/memory_stats.hpp"
#include "core/memory/memory_tag.hpp"

namespace har
{    
    void dynamic_allocator::initialize(void* externalPool, u64 size)
    {
        mUsable = tlsf_pool_init(&mTLSF, externalPool, size);
        HAR_ASS(mUsable);
    }

    void* dynamic_allocator::allocate(memory_tag tag, u64 size, u8 alignment)
    {
        HAR_ASS(size);

        void* address = tlsf_aalloc(&mTLSF, alignment, size);
        HAR_ASS(address);

        gMemoryStats.add(tlsf_usable_size(address), tag);
        return address;
    }

    void* dynamic_allocator::reallocate(memory_tag tag, void* address, u64 newSize)
    {
        if (newSize == 0)
            return nullptr;

        gMemoryStats.remove(tlsf_usable_size(address), tag);
        
        void* newAddress = tlsf_realloc(&mTLSF, address, newSize);
        
        gMemoryStats.add(tlsf_usable_size(newAddress), tag);
        return newAddress;
    }

    void dynamic_allocator::deallocate(memory_tag tag, void* address)
    {
        if (!address)
            return;

        gMemoryStats.remove(tlsf_usable_size(address), tag);
        tlsf_free(&mTLSF, address);
    }
}

namespace har::safe
{
    dynamic_allocator::dynamic_allocator(void* externalPool, u64 size)
    {
        initialize(externalPool, size);
    }
    
    void dynamic_allocator::initialize(void* externalPool, u64 size)
    {
        mUsable = tlsf_thread_init(&mTLSF, externalPool, size);
        HAR_ASS(mUsable);
    }

    void* dynamic_allocator::allocate(memory_tag tag, u64 size, u8 alignment)
    {
        // TODO: Handle out of memory issues?
        HAR_ASS(size);

        void* address = tlsf_thread_aalloc(&mTLSF, alignment, size);
        HAR_ASS(address);

        gMemoryStats.add(tlsf_usable_size(address), tag);
        return address;
    }

    void* dynamic_allocator::reallocate(memory_tag tag, void* address, u64 newSize)
    {
        if (newSize == 0)
            return nullptr;

        gMemoryStats.remove(tlsf_usable_size(address), tag);
        
        void* newAddress = tlsf_thread_realloc(&mTLSF, address, newSize);
        
        gMemoryStats.add(tlsf_usable_size(newAddress), tag);
        return newAddress;
    }

    void dynamic_allocator::deallocate(memory_tag tag, void* address)
    {
        if (!address)
            return;

        gMemoryStats.remove(tlsf_usable_size(address), tag);
        tlsf_thread_free(&mTLSF, address);
    }
}
