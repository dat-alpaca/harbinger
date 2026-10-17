#pragma once
#include <chrono>
#include "core/defines.hpp"

namespace har
{
    using s_clock_t = std::chrono::steady_clock;

    struct stopwatch
    {
    public:
        void restart()
        {
            using namespace std::chrono;
            mBegin = steady_clock::now();
        }

        u64 elapsed() const
        {
            using namespace std::chrono;

            auto end = steady_clock::now();
            const auto duration = duration_cast<microseconds>(end - mBegin);

            return duration.count();
        }

    private:
        s_clock_t::time_point mBegin = s_clock_t::now();
    };
}