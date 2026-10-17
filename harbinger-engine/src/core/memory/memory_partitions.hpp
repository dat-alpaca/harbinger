#pragma once
#include "core/defines.hpp"

namespace har
{
    struct memory_partitions
    {
        OWNING byte* staticMemory;
        OWNING byte* dynamicMemory;
        OWNING byte* transientMemory;
        OWNING byte* safeMemory;

        u32 staticMemorySize = 0;
        u32 dynamicMemorySize = 0;
        u32 transientMemorySize = 0;
        u32 safeMemorySize = 0;
    };

    struct memory_partition_args
    {
        u32 staticMemorySizeBytes;
        u32 dynamicMemorySizeBytes;
        u32 transientMemorySizeBytes;
        u32 safeMemorySizeBytes;
    };

    memory_partitions memory_partitions_initialize(const memory_partition_args& args);
    void memory_partitions_destroy(memory_partitions& partitions);
}
