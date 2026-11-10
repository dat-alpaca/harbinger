#pragma once
#include "core/input/keys.hpp"

namespace har::platform
{
    input_action get_input_action(i32 action);
    i32 get_native_input_action(input_action action);

    key_t get_key(i32 keyCode);
    i32 get_native_key(key_t key);

    mouse_button get_mouse_button(i32 mouseButtonCode);
    i32 get_native_mouse_button(mouse_button mouseButton);
}