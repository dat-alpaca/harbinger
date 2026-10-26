#pragma once
#include "core/engine/context.hpp"

namespace har
{
    void harbinger_engine_initialize(context_t& context, const char* configFilepath = "config.ini");
    void harbinger_engine_shutdown(context_t& context);

    void harbinger_engine_start_frame(context_t& context);
    void harbinger_engine_on_update(context_t& context);
    
    bool harbinger_engine_is_running(context_t& context);
}
