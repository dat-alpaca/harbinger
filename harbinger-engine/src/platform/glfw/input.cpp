#include "platform/input.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "core/input/keys.hpp"
#include "core/logging/logger.hpp"

namespace har::platform
{
    input_action get_input_action(i32 action)
    {
      if (action == GLFW_PRESS)
			return input_action::press;

      if (action == GLFW_RELEASE)
		   return input_action::release;
    
      HAR_LOG_ERROR("Invalid key action.");
	   return input_action::none;
    }

    i32 get_native_input_action(input_action action)
    {
         if (action == input_action::press)
			   return GLFW_PRESS;

         if (action == input_action::release)
			   return GLFW_RELEASE;

         HAR_LOG_ERROR("Invalid key action.");
		   return GLFW_FALSE;
    }

    key_t get_key(i32 keyCode)
    {
      switch(keyCode)
      {
         case GLFW_KEY_SPACE: 
			return key_t::space;

         case GLFW_KEY_APOSTROPHE: 
			return key_t::apostrophe;

         case GLFW_KEY_COMMA: 
				return key_t::comma;

         case GLFW_KEY_MINUS: 
				return key_t::minus;

         case GLFW_KEY_PERIOD: 
				return key_t::period;

         case GLFW_KEY_SLASH: 
				return key_t::slash;

         case GLFW_KEY_0: 
				return key_t::key_0;

         case GLFW_KEY_1: 
				return key_t::key_1;

         case GLFW_KEY_2: 
				return key_t::key_2;

         case GLFW_KEY_3: 
				return key_t::key_3;

         case GLFW_KEY_4: 
				return key_t::key_4;

         case GLFW_KEY_5: 
				return key_t::key_5;

         case GLFW_KEY_6: 
				return key_t::key_6;

         case GLFW_KEY_7: 
				return key_t::key_7;

         case GLFW_KEY_8: 
				return key_t::key_8;

         case GLFW_KEY_9: 
				return key_t::key_9;

         case GLFW_KEY_SEMICOLON: 
				return key_t::semicolon;

         case GLFW_KEY_EQUAL: 
				return key_t::equal;

         case GLFW_KEY_A: 
				return key_t::a;

         case GLFW_KEY_B: 
				return key_t::b;

         case GLFW_KEY_C: 
				return key_t::c;

         case GLFW_KEY_D: 
				return key_t::d;

         case GLFW_KEY_E: 
				return key_t::e;

         case GLFW_KEY_F: 
				return key_t::f;

         case GLFW_KEY_G: 
				return key_t::g;

         case GLFW_KEY_H: 
				return key_t::h;

         case GLFW_KEY_I: 
				return key_t::i;

         case GLFW_KEY_J: 
				return key_t::j;

         case GLFW_KEY_K: 
				return key_t::k;

         case GLFW_KEY_L: 
				return key_t::l;

         case GLFW_KEY_M: 
				return key_t::m;

         case GLFW_KEY_N: 
				return key_t::n;

         case GLFW_KEY_O: 
				return key_t::o;

         case GLFW_KEY_P: 
				return key_t::p;

         case GLFW_KEY_Q: 
				return key_t::q;

         case GLFW_KEY_R: 
				return key_t::r;

         case GLFW_KEY_S: 
				return key_t::s;

         case GLFW_KEY_T: 
				return key_t::t;

         case GLFW_KEY_U: 
				return key_t::u;

         case GLFW_KEY_V: 
				return key_t::v;

         case GLFW_KEY_W: 
				return key_t::w;

         case GLFW_KEY_X: 
				return key_t::x;

         case GLFW_KEY_Y: 
				return key_t::y;

         case GLFW_KEY_Z: 
				return key_t::z;

         case GLFW_KEY_LEFT_BRACKET: 
				return key_t::left_bracket;

         case GLFW_KEY_BACKSLASH: 
				return key_t::backslash;
 
         case GLFW_KEY_RIGHT_BRACKET: 
				return key_t::right_bracket;

         case GLFW_KEY_GRAVE_ACCENT: 
				return key_t::grave_accent;

         case GLFW_KEY_WORLD_1: 
				return key_t::world_1;

         case GLFW_KEY_WORLD_2: 
				return key_t::world_2;

         case GLFW_KEY_ESCAPE: 
				return key_t::escape;

         case GLFW_KEY_ENTER: 
				return key_t::enter;

         case GLFW_KEY_TAB: 
				return key_t::tab;

         case GLFW_KEY_BACKSPACE: 
				return key_t::backspace;

         case GLFW_KEY_INSERT: 
				return key_t::insert;

         case GLFW_KEY_DELETE: 
				return key_t::del;

         case GLFW_KEY_RIGHT: 
				return key_t::right;

         case GLFW_KEY_LEFT: 
				return key_t::left;

         case GLFW_KEY_DOWN: 
				return key_t::down;

         case GLFW_KEY_UP: 
				return key_t::up;

         case GLFW_KEY_PAGE_UP: 
				return key_t::page_up;

         case GLFW_KEY_PAGE_DOWN: 
				return key_t::page_down;

         case GLFW_KEY_HOME: 
				return key_t::home;

         case GLFW_KEY_END: 
				return key_t::end;

         case GLFW_KEY_CAPS_LOCK: 
				return key_t::caps_lock;

         case GLFW_KEY_SCROLL_LOCK: 
				return key_t::scroll_lock;

         case GLFW_KEY_NUM_LOCK: 
				return key_t::num_lock;

         case GLFW_KEY_PRINT_SCREEN: 
				return key_t::print_screen;

         case GLFW_KEY_PAUSE: 
				return key_t::pause;

         case GLFW_KEY_F1: 
				return key_t::f1;

         case GLFW_KEY_F2: 
				return key_t::f2;

         case GLFW_KEY_F3: 
				return key_t::f3;

         case GLFW_KEY_F4: 
				return key_t::f4;

         case GLFW_KEY_F5: 
				return key_t::f5;

         case GLFW_KEY_F6: 
				return key_t::f6;

         case GLFW_KEY_F7: 
				return key_t::f7;

         case GLFW_KEY_F8: 
				return key_t::f8;

         case GLFW_KEY_F9: 
				return key_t::f9;

         case GLFW_KEY_F10: 
				return key_t::f10;

         case GLFW_KEY_F11: 
				return key_t::f11;

         case GLFW_KEY_F12: 
				return key_t::f12;

         case GLFW_KEY_F13: 
				return key_t::f13;

         case GLFW_KEY_F14: 
				return key_t::f14;

         case GLFW_KEY_F15: 
				return key_t::f15;

         case GLFW_KEY_F16: 
				return key_t::f16;

         case GLFW_KEY_F17: 
				return key_t::f17;

         case GLFW_KEY_F18: 
				return key_t::f18;

         case GLFW_KEY_F19: 
				return key_t::f19;

         case GLFW_KEY_F20: 
				return key_t::f20;

         case GLFW_KEY_F21: 
				return key_t::f21;

         case GLFW_KEY_F22: 
				return key_t::f22;

         case GLFW_KEY_F23: 
				return key_t::f23;

         case GLFW_KEY_F24: 
				return key_t::f24;

         case GLFW_KEY_F25: 
				return key_t::f25;

         case GLFW_KEY_KP_0: 
				return key_t::kp_0;

         case GLFW_KEY_KP_1: 
				return key_t::kp_1;

         case GLFW_KEY_KP_2: 
				return key_t::kp_2;

         case GLFW_KEY_KP_3: 
				return key_t::kp_3;

         case GLFW_KEY_KP_4: 
				return key_t::kp_4;

         case GLFW_KEY_KP_5: 
				return key_t::kp_5;

         case GLFW_KEY_KP_6: 
				return key_t::kp_6;

         case GLFW_KEY_KP_7: 
				return key_t::kp_7;

         case GLFW_KEY_KP_8: 
				return key_t::kp_8;

         case GLFW_KEY_KP_9: 
				return key_t::kp_9;

         case GLFW_KEY_KP_DECIMAL: 
				return key_t::kp_decimal;

         case GLFW_KEY_KP_DIVIDE: 
				return key_t::kp_divide;

         case GLFW_KEY_KP_MULTIPLY: 
				return key_t::kp_multiply;

         case GLFW_KEY_KP_SUBTRACT: 
				return key_t::kp_subtract;

         case GLFW_KEY_KP_ADD: 
				return key_t::kp_add;

         case GLFW_KEY_KP_ENTER: 
				return key_t::kp_enter;

         case GLFW_KEY_KP_EQUAL: 
				return key_t::kp_equal;

         case GLFW_KEY_LEFT_SHIFT: 
				return key_t::left_shift;

         case GLFW_KEY_LEFT_CONTROL: 
				return key_t::left_control;

         case GLFW_KEY_LEFT_ALT: 
				return key_t::left_alt;

         case GLFW_KEY_LEFT_SUPER: 
				return key_t::left_super;

         case GLFW_KEY_RIGHT_SHIFT: 
				return key_t::right_shift;

         case GLFW_KEY_RIGHT_CONTROL: 
				return key_t::right_control;

         case GLFW_KEY_RIGHT_ALT: 
				return key_t::right_alt;

         case GLFW_KEY_RIGHT_SUPER:
				return key_t::right_super;

         case GLFW_KEY_MENU: 
				return key_t::menu;

         default:
         {
               HAR_LOG_ERROR("Invalid key.");
			      return key_t::invalid;
         }
      }
    }

    i32 get_native_key(key_t key)
    {
        switch(key)
        {
            case key_t::space: 
				return GLFW_KEY_SPACE;

            case key_t::apostrophe: 
				return GLFW_KEY_APOSTROPHE;

            case key_t::comma: 
				return GLFW_KEY_COMMA;

            case key_t::minus: 
				return GLFW_KEY_MINUS;

            case key_t::period: 
				return GLFW_KEY_PERIOD;

            case key_t::slash: 
				return GLFW_KEY_SLASH;

            case key_t::key_0: 
				return GLFW_KEY_0;

            case key_t::key_1: 
				return GLFW_KEY_1;

            case key_t::key_2: 
				return GLFW_KEY_2;

            case key_t::key_3: 
				return GLFW_KEY_3;

            case key_t::key_4: 
				return GLFW_KEY_4;

            case key_t::key_5: 
				return GLFW_KEY_5;

            case key_t::key_6: 
				return GLFW_KEY_6;

            case key_t::key_7: 
				return GLFW_KEY_7;

            case key_t::key_8: 
				return GLFW_KEY_8;

            case key_t::key_9: 
				return GLFW_KEY_9;

            case key_t::semicolon: 
				return GLFW_KEY_SEMICOLON;

            case key_t::equal: 
				return GLFW_KEY_EQUAL;

            case key_t::a: 
				return GLFW_KEY_A;

            case key_t::b: 
				return GLFW_KEY_B;

            case key_t::c: 
				return GLFW_KEY_C;

            case key_t::d: 
				return GLFW_KEY_D;

            case key_t::e: 
				return GLFW_KEY_E;

            case key_t::f: 
				return GLFW_KEY_F;

            case key_t::g: 
				return GLFW_KEY_G;

            case key_t::h: 
				return GLFW_KEY_H;

            case key_t::i: 
				return GLFW_KEY_I;

            case key_t::j: 
				return GLFW_KEY_J;

            case key_t::k: 
				return GLFW_KEY_K;

            case key_t::l: 
				return GLFW_KEY_L;

            case key_t::m: 
				return GLFW_KEY_M;

            case key_t::n: 
				return GLFW_KEY_N;

            case key_t::o: 
				return GLFW_KEY_O;

            case key_t::p: 
				return GLFW_KEY_P;

            case key_t::q: 
				return GLFW_KEY_Q;

            case key_t::r: 
				return GLFW_KEY_R;

            case key_t::s: 
				return GLFW_KEY_S;

            case key_t::t: 
				return GLFW_KEY_T;

            case key_t::u: 
				return GLFW_KEY_U;

            case key_t::v: 
				return GLFW_KEY_V;

            case key_t::w: 
				return GLFW_KEY_W;

            case key_t::x: 
				return GLFW_KEY_X;

            case key_t::y: 
				return GLFW_KEY_Y;

            case key_t::z: 
				return GLFW_KEY_Z;

            case key_t::left_bracket: 
				return GLFW_KEY_LEFT_BRACKET;

            case key_t::backslash: 
				return GLFW_KEY_BACKSLASH;
 
            case key_t::right_bracket: 
				return GLFW_KEY_RIGHT_BRACKET;

            case key_t::grave_accent: 
				return GLFW_KEY_GRAVE_ACCENT;

            case key_t::world_1: 
				return GLFW_KEY_WORLD_1;

            case key_t::world_2: 
				return GLFW_KEY_WORLD_2;

            case key_t::escape: 
				return GLFW_KEY_ESCAPE;

            case key_t::enter: 
				return GLFW_KEY_ENTER;

            case key_t::tab: 
				return GLFW_KEY_TAB;

            case key_t::backspace: 
				return GLFW_KEY_BACKSPACE;

            case key_t::insert: 
				return GLFW_KEY_INSERT;

            case key_t::del: 
				return GLFW_KEY_DELETE;

            case key_t::right: 
				return GLFW_KEY_RIGHT;

            case key_t::left: 
				return GLFW_KEY_LEFT;

            case key_t::down: 
				return GLFW_KEY_DOWN;

            case key_t::up: 
				return GLFW_KEY_UP;

            case key_t::page_up: 
				return GLFW_KEY_PAGE_UP;

            case key_t::page_down: 
				return GLFW_KEY_PAGE_DOWN;

            case key_t::home: 
				return GLFW_KEY_HOME;

            case key_t::end: 
				return GLFW_KEY_END;

            case key_t::caps_lock: 
				return GLFW_KEY_CAPS_LOCK;

            case key_t::scroll_lock: 
				return GLFW_KEY_SCROLL_LOCK;

            case key_t::num_lock: 
				return GLFW_KEY_NUM_LOCK;

            case key_t::print_screen: 
				return GLFW_KEY_PRINT_SCREEN;

            case key_t::pause: 
				return GLFW_KEY_PAUSE;

            case key_t::f1: 
				return GLFW_KEY_F1;

            case key_t::f2: 
				return GLFW_KEY_F2;

            case key_t::f3: 
				return GLFW_KEY_F3;

            case key_t::f4: 
				return GLFW_KEY_F4;

            case key_t::f5: 
				return GLFW_KEY_F5;

            case key_t::f6: 
				return GLFW_KEY_F6;

            case key_t::f7: 
				return GLFW_KEY_F7;

            case key_t::f8: 
				return GLFW_KEY_F8;

            case key_t::f9: 
				return GLFW_KEY_F9;

            case key_t::f10: 
				return GLFW_KEY_F10;

            case key_t::f11: 
				return GLFW_KEY_F11;

            case key_t::f12: 
				return GLFW_KEY_F12;

            case key_t::f13: 
				return GLFW_KEY_F13;

            case key_t::f14: 
				return GLFW_KEY_F14;

            case key_t::f15: 
				return GLFW_KEY_F15;

            case key_t::f16: 
				return GLFW_KEY_F16;

            case key_t::f17: 
				return GLFW_KEY_F17;

            case key_t::f18: 
				return GLFW_KEY_F18;

            case key_t::f19: 
				return GLFW_KEY_F19;

            case key_t::f20: 
				return GLFW_KEY_F20;

            case key_t::f21: 
				return GLFW_KEY_F21;

            case key_t::f22: 
				return GLFW_KEY_F22;

            case key_t::f23: 
				return GLFW_KEY_F23;

            case key_t::f24: 
				return GLFW_KEY_F24;

            case key_t::f25: 
				return GLFW_KEY_F25;

            case key_t::kp_0: 
				return GLFW_KEY_KP_0;

            case key_t::kp_1: 
				return GLFW_KEY_KP_1;

            case key_t::kp_2: 
				return GLFW_KEY_KP_2;

            case key_t::kp_3: 
				return GLFW_KEY_KP_3;

            case key_t::kp_4: 
				return GLFW_KEY_KP_4;

            case key_t::kp_5: 
				return GLFW_KEY_KP_5;

            case key_t::kp_6: 
				return GLFW_KEY_KP_6;

            case key_t::kp_7: 
				return GLFW_KEY_KP_7;

            case key_t::kp_8: 
				return GLFW_KEY_KP_8;

            case key_t::kp_9: 
				return GLFW_KEY_KP_9;

            case key_t::kp_decimal: 
				return GLFW_KEY_KP_DECIMAL;

            case key_t::kp_divide: 
				return GLFW_KEY_KP_DIVIDE;

            case key_t::kp_multiply: 
				return GLFW_KEY_KP_MULTIPLY;

            case key_t::kp_subtract: 
				return GLFW_KEY_KP_SUBTRACT;

            case key_t::kp_add: 
				return GLFW_KEY_KP_ADD;

            case key_t::kp_enter: 
				return GLFW_KEY_KP_ENTER;

            case key_t::kp_equal: 
				return GLFW_KEY_KP_EQUAL;

            case key_t::left_shift: 
				return GLFW_KEY_LEFT_SHIFT;

            case key_t::left_control: 
				return GLFW_KEY_LEFT_CONTROL;

            case key_t::left_alt: 
				return GLFW_KEY_LEFT_ALT;

            case key_t::left_super: 
				return GLFW_KEY_LEFT_SUPER;

            case key_t::right_shift: 
				return GLFW_KEY_RIGHT_SHIFT;

            case key_t::right_control: 
				return GLFW_KEY_RIGHT_CONTROL;

            case key_t::right_alt: 
				return GLFW_KEY_RIGHT_ALT;

            case key_t::right_super: 
				return GLFW_KEY_RIGHT_SUPER;

            case key_t::menu: 
				return GLFW_KEY_MENU;

            default:
            {
                HAR_LOG_ERROR("Invalid key.");
				return GLFW_FALSE;
            }
        }
    }

    mouse_button get_mouse_button(i32 mouseButtonCode)
    {
        switch (mouseButtonCode)
        {
            case GLFW_MOUSE_BUTTON_LEFT:
                return mouse_button::left;

            case GLFW_MOUSE_BUTTON_RIGHT:
                return mouse_button::right;

            case GLFW_MOUSE_BUTTON_MIDDLE:
                return mouse_button::middle;

            case GLFW_MOUSE_BUTTON_4:
                return mouse_button::button_4;

            case GLFW_MOUSE_BUTTON_5:
                return mouse_button::button_5;

            case GLFW_MOUSE_BUTTON_6:
                return mouse_button::button_6;

            case GLFW_MOUSE_BUTTON_7:
                return mouse_button::button_7;

            case GLFW_MOUSE_BUTTON_8:
                return mouse_button::button_8;

            default:
            {
                HAR_LOG_ERROR("Invalid mouse button");
                return mouse_button::invalid;
            }
        }
    }

    i32 get_native_mouse_button(mouse_button mouseButton)
    {
        switch (mouseButton)
        {
            case mouse_button::left:
                return GLFW_MOUSE_BUTTON_LEFT;

            case mouse_button::right:
                return GLFW_MOUSE_BUTTON_RIGHT;

            case mouse_button::middle:
                return GLFW_MOUSE_BUTTON_MIDDLE;

            case mouse_button::button_4:
                return GLFW_MOUSE_BUTTON_4;

            case mouse_button::button_5:
                return GLFW_MOUSE_BUTTON_5;

            case mouse_button::button_6:
                return GLFW_MOUSE_BUTTON_6;

            case mouse_button::button_7:
                return GLFW_MOUSE_BUTTON_7;

            case mouse_button::button_8:
                return GLFW_MOUSE_BUTTON_8;

            default:
            {
                HAR_LOG_ERROR("Invalid mouse button");
                return GLFW_MOUSE_BUTTON_8 + 10;
            }
        }
    }
}