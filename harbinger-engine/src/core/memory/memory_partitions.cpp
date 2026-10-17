#include "memory_partitions.hpp"
#include "platform/memory.hpp"
#include "core/defines.hpp"

namespace har
{
    memory_partitions memory_partitions_initialize(const memory_partition_args& args)
    {
        memory_partitions partitions = { };
        partitions.staticMemory    = static_cast<byte*>(platform::allocate(args.staticMemorySizeBytes));
        partitions.dynamicMemory   = static_cast<byte*>(platform::allocate(args.dynamicMemorySizeBytes));
        partitions.transientMemory = static_cast<byte*>(platform::allocate(args.transientMemorySizeBytes));
        partitions.safeMemory      = static_cast<byte*>(platform::allocate(args.safeMemorySizeBytes));

        partitions.staticMemorySize = args.staticMemorySizeBytes;
        partitions.dynamicMemorySize = args.dynamicMemorySizeBytes;
        partitions.transientMemorySize = args.transientMemorySizeBytes;
        partitions.safeMemorySize = args.safeMemorySizeBytes;

        return partitions;
    }

    void memory_partitions_destroy(memory_partitions& partitions)
    {
        platform::deallocate(partitions.staticMemory);
        platform::deallocate(partitions.dynamicMemory);
        platform::deallocate(partitions.transientMemory);
        platform::deallocate(partitions.safeMemory);
    }
}