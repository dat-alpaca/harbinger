#pragma once
#include "core/logging/logger.hpp"
#include <source_location>

#ifdef HAR_DEBUG
namespace har
{
	inline void __har_assert(const char* condition, const char* message, std::source_location location = std::source_location::current())
	{
 		HAR_LOG_CRITICAL
        (
            "[Assertion failed]: {} ({}) | At: {} [{}] ({}:{})",
            message, 
            condition, 
            location.file_name(), 
            location.function_name(), 
            location.line(), 
            location.column()
        );
	}

    inline void __har_ass(const char* condition, std::source_location location = std::source_location::current())
	{
        HAR_LOG_CRITICAL
        (
            "[Assertion failed]: ({}) | At: {} [{}] ({}:{})",
            condition, 
            location.file_name(), 
            location.function_name(), 
            location.line(), 
            location.column()
        );
	}

    #define HAR_ASSERT(condition, message)									                \
	{																		                \
		if(condition) { }													                \
		else																                \
		{																	                \
			har::__har_assert(#condition, message, std::source_location::current());		\
		}																	                \
	}

    #define HAR_ASS(condition)															    \
	{																						\
		if(condition) { }																	\
		else																				\
		{																					\
			har::__har_ass(#condition,  std::source_location::current());		            \
		}																					\
	}
}
#else
    #define HAR_ASSERT(condition, ...)  ((void)0)
    #define HAR_ASS(condition)          ((void)0)
#endif