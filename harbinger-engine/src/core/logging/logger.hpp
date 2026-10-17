#pragma once
#include <memory>
#include <source_location>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "core/defines.hpp"
#include "core/memory/dynamic_allocator.hpp"
#include "core/memory/memory_tag.hpp"

#include "platform/breakpoint.hpp"

namespace
{
    constexpr inline void log_file_information(std::source_location location = std::source_location::current())
    {
        spdlog::info("- File: {} (L: {} | C: {})", location.file_name(), location.line(), location.column());
        spdlog::info("- Function: {}", location.function_name());
    }
}

namespace har
{
    using logger_t = har_shared<spdlog::logger>; 

#ifndef HAR_DEBUG
    inline void initialize_logger(NON_OWNING dynamic_allocator*) { /* Blank */ }

    constexpr inline u32 required_logger_dynamic_memory() { return 0 ; }

    #define HAR_LOG_INFO(...)     { }
    #define HAR_LOG_WARN(...)     { }
    #define HAR_LOG_ERROR(...)    { }
    #define HAR_LOG_CRITICAL(...) { }
    #define HAR_LOG_TRACE(...)    { }
    #define HAR_LOG_DEBUG(...)    { }

#else
    inline void initialize_logger(NON_OWNING dynamic_allocator* allocator)
    {
        using namespace spdlog;
        using sink_t = sinks::stdout_color_sink_st;

        // Sink:
        sink_t* sinkBuffer = static_cast<sink_t*>(allocator->allocate(memory_tag::logging, sizeof(sink_t)));
        std::construct_at(sinkBuffer);

        har_shared<sink_t> sink = std::shared_ptr<sink_t>(sinkBuffer,
        [allocator](sink_t* sink)
        {
            std::destroy_at(sink);
            allocator->deallocate(memory_tag::logging, sink);
        });

        sink->set_color_mode(color_mode::automatic);

        // Logger:
        logger* loggerBuffer = static_cast<logger*>(allocator->allocate(memory_tag::logging, sizeof(logger)));
        std::construct_at(loggerBuffer, logger("engine", sink));

        har_shared<logger> mainLogger = std::shared_ptr<logger>(loggerBuffer, 
        [allocator](logger* logger) {
            std::destroy_at(logger);
            allocator->deallocate(memory_tag::logging, logger);
        });

        spdlog::initialize_logger(mainLogger);
        spdlog::set_default_logger(mainLogger);

        mainLogger->set_pattern("%^[%P | %t] [%n] [%l]%$:\t%v");
    }

    constexpr inline u32 required_logger_dynamic_memory()
    {
        u32 loggerMemory = sizeof(logger_t);
        u32 sinkMemory = sizeof(spdlog::sinks::stdout_color_sink_st);
        return loggerMemory + sinkMemory;
    }

    #define HAR_LOG_INFO(...)     { spdlog::info(__VA_ARGS__); }
    #define HAR_LOG_WARN(...)     { spdlog::warn(__VA_ARGS__); }
    #define HAR_LOG_ERROR(...)    { spdlog::error(__VA_ARGS__); log_file_information(); }
    #define HAR_LOG_CRITICAL(...) { spdlog::critical(__VA_ARGS__); log_file_information(); har::platform::breakpoint(); }
    #define HAR_LOG_TRACE(...)    { spdlog::trace(__VA_ARGS__); log_file_information(); }
    #define HAR_LOG_DEBUG(...)    { spdlog::debug(__VA_ARGS__); log_file_information(); }
#endif
}