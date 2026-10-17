#pragma once
#include "core/defines.hpp"
#include "core/memory/memory_tag.hpp"

namespace har
{
    struct memory_stats
    {
    public:
        void initialize()
        {
            for (u64& stat : stats)
                stat = 0;
        }

    public:
        constexpr inline void add(u64 size, memory_tag tag)
        {
            stats[static_cast<u64>(tag)] += size;
            rawBytesUsed += size;
            peakBytesUsed = std::max(peakBytesUsed, rawBytesUsed);
        }

        constexpr inline void remove(u64 size, memory_tag tag)
        {
            stats[static_cast<u64>(tag)] -= size;
            rawBytesUsed -= size;
            peakBytesUsed = std::max(peakBytesUsed, rawBytesUsed);
        }

        constexpr inline u64 get(memory_tag tag) const { return stats[static_cast<u64>(tag)]; }
        constexpr inline u64 get_raw_bytes() const { return rawBytesUsed; }
        constexpr inline u64 get_peak_bytes() const { return peakBytesUsed; }

    private:
        u64 stats[static_cast<u64>(memory_tag::amount)];
        u64 rawBytesUsed = 0;
        u64 peakBytesUsed = 0;
    };

    inline memory_stats gMemoryStats = {};
}