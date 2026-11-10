#pragma once
#include <functional>
#include "core/defines.hpp"
#include "core/input/keys.hpp"

namespace har::platform
{
    using mouse_button_clbk = std::function<void(i32 button, i32 action, i32 mods)>;
    using scroll_clbk       = std::function<void(f64 x, f64 y)>;
    using character_clbk    = std::function<void(u32 codepoint)>;
    using key_clbk          = std::function<void(key_t key, i32 scancode, input_action action, i32 mods)>;
}