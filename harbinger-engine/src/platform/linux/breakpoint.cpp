#include <csignal>
#include "platform/breakpoint.hpp"

namespace har::platform
{
    void breakpoint()
    {
        std::raise(SIGINT);
    }
}