#include "gui_subsystem.hpp"
#include "core/assert/assert.hpp"
#include "core/defines.hpp"
#include "core/input/keys.hpp"
#include "core/logging/logger.hpp"
#include "core/memory/allocators.hpp"
#include "core/memory/memory_tag.hpp"

#include "nuklear.h"
#include "platform/window.hpp"

using namespace har;

static inline nk_flags get_nuklear_flags(gui_frame_flag flags)
{
    using namespace har;
    
    nk_flags result = 0;
    
    if ((flags & gui_frame_flag::border) == 1)
        result |= NK_WINDOW_BORDER;

    if ((flags & gui_frame_flag::movable) == 1)
        result |= NK_WINDOW_MOVABLE;

    if ((flags & gui_frame_flag::scalable) == 1)
        result |= NK_WINDOW_SCALABLE;

    if ((flags & gui_frame_flag::closable) == 1)
        result |= NK_WINDOW_CLOSABLE;

    if ((flags & gui_frame_flag::minimizable) == 1)
        result |= NK_WINDOW_MINIMIZABLE;

    if ((flags & gui_frame_flag::no_scrollbar) == 1)
        result |= NK_WINDOW_NO_SCROLLBAR;

    if ((flags & gui_frame_flag::title) == 1)
        result |= NK_WINDOW_TITLE;

    if ((flags & gui_frame_flag::scroll_auto_hide) == 1)
        result |= NK_WINDOW_SCROLL_AUTO_HIDE;

    if ((flags & gui_frame_flag::background) == 1)
        result |= NK_WINDOW_BACKGROUND;

    if ((flags & gui_frame_flag::scale_left) == 1)
        result |= NK_WINDOW_SCALE_LEFT;

    if ((flags & gui_frame_flag::no_input) == 1)
        result |= NK_WINDOW_NO_INPUT;

    
    return result;
}

static inline nk_flags get_nuklear_alignment(gui_alignment alignment)
{
    using namespace har;
    
    if (alignment == gui_alignment::left)
        return NK_TEXT_LEFT;

    else if (alignment == gui_alignment::center)
        return NK_TEXT_ALIGN_MIDDLE;

    else if (alignment == gui_alignment::right)
        return NK_TEXT_RIGHT;

    HAR_LOG_ERROR("Invalid GUI alignment option.");
    return 0;
}

static inline gui_special_key get_gui_special_key(har::key_t key)
{
    switch (key)
    {
        case har::key_t::del:
            return gui_special_key::del;

        case har::key_t::tab:
            return gui_special_key::tab;

        case har::key_t::backspace:
            return gui_special_key::backspace;

        case har::key_t::up:
            return gui_special_key::up;

        case har::key_t::down:
            return gui_special_key::down;

        case har::key_t::left:
            return gui_special_key::left;

        case har::key_t::right:
            return gui_special_key::right;

        case har::key_t::left_alt:
        case har::key_t::right_alt:
            return gui_special_key::alt;

        case har::key_t::page_up:
            return gui_special_key::scroll_up;

        case har::key_t::page_down:
            return gui_special_key::scroll_down;

        case har::key_t::f1:
            return gui_special_key::f1;

        case har::key_t::f2:
            return gui_special_key::f2;

        case har::key_t::f3:
            return gui_special_key::f3;

        case har::key_t::f4:
            return gui_special_key::f4;

        case har::key_t::f5:
            return gui_special_key::f5;

        case har::key_t::f6:
            return gui_special_key::f6;

        case har::key_t::f7:
            return gui_special_key::f7;

        case har::key_t::f8:
            return gui_special_key::f8;

        case har::key_t::f9:
            return gui_special_key::f9;

        case har::key_t::f10:
            return gui_special_key::f10;

        case har::key_t::f11:
            return gui_special_key::f11;

        case har::key_t::f12:
            return gui_special_key::f12;

        case har::key_t::c:
            return gui_special_key::copy;

        case har::key_t::v:
            return gui_special_key::paste;

        case har::key_t::x:
            return gui_special_key::cut;

        case har::key_t::z:
            return gui_special_key::text_undo;

        case har::key_t::y:
            return gui_special_key::text_redo;

        case har::key_t::b:
            return gui_special_key::line_start;

        case har::key_t::e:
            return gui_special_key::line_end;

        case har::key_t::a:
            return gui_special_key::select_all;

        case har::key_t::enter:
        case har::key_t::kp_enter:
            return gui_special_key::enter;

        case har::key_t::escape:
            return gui_special_key::text_reset_mode;

        default:
            return gui_special_key::invalid;
    }

    return gui_special_key::invalid;
}

namespace har
{
    void gui_subsystem::initialize(NON_OWNING window_t* window, NON_OWNING harbinger_renderer* renderer)
    {
        HAR_ASS(window);
        HAR_ASS(renderer);
        rWindow = window;
        rRenderer = renderer;

        // TODO: ponder about memory usage
        static constexpr u64 MaxMemory = 1;
        nk_init_fixed(&mContext, internal::MainDynamicAllocator.allocate(memory_tag::gui, MaxMemory, 1), MaxMemory, nullptr);

        initialize_callbacks();
        initialize_clipboard();

        mState.lastDeltaTimeSeconds = static_cast<f32>(platform::window_get_time_seconds(*rWindow));
    }

    void gui_subsystem::start_gui()
    {
        // Update timer:
        f32 currentDeltaTime = static_cast<f32>(platform::window_get_time_seconds(*rWindow));
        mState.currentDeltaTimeSeconds = currentDeltaTime - mState.lastDeltaTimeSeconds;
        mState.lastDeltaTimeSeconds = currentDeltaTime;

        // Update window size:
        platform::window_get_dimensions(*rWindow, mState.windowWidth, mState.windowHeight);
        platform::window_get_framebuffer_dimensions(*rWindow, mState.fbWidth, mState.fbHeight);
        mState.fbScaleX = static_cast<f32>(mState.fbWidth) / static_cast<f32>(mState.windowWidth);
        mState.fbScaleY = static_cast<f32>(mState.fbHeight) / static_cast<f32>(mState.windowHeight);

        // Char:
        nk_input_begin(&mContext);
        for (u8 i = 0; i < mState.textLength; ++i)
            nk_input_unicode(&mContext, mState.text[i]);

        // Mouse grabbing:
        if (!mContext.input.mouse.grab)
            platform::window_show_cursor(*rWindow);
        else
            platform::window_hide_cursor(*rWindow);

        // Special key state:
        auto get_key_state = [&](gui_special_key specialKey) -> i8
        {
            return static_cast<i8>(mState.keyState[static_cast<u8>(specialKey)]);
        };

        if (get_key_state(gui_special_key::del) >= 0)
            nk_input_key(&mContext, NK_KEY_DEL, get_key_state(gui_special_key::del));

        if (get_key_state(gui_special_key::enter) >= 0)
            nk_input_key(&mContext, NK_KEY_ENTER, get_key_state(gui_special_key::enter));

        if (get_key_state(gui_special_key::text_reset_mode) >= 0)
            nk_input_key(&mContext, NK_KEY_TEXT_RESET_MODE, get_key_state(gui_special_key::text_reset_mode));
        
        if (get_key_state(gui_special_key::tab) >= 0)
            nk_input_key(&mContext, NK_KEY_TAB, get_key_state(gui_special_key::tab));

        if (get_key_state(gui_special_key::alt) >= 0) 
            nk_input_key(&mContext, NK_KEY_ALT, get_key_state(gui_special_key::alt));

        if (get_key_state(gui_special_key::backspace) >= 0) 
            nk_input_key(&mContext, NK_KEY_BACKSPACE, get_key_state(gui_special_key::backspace));

        if (get_key_state(gui_special_key::up) >= 0) 
            nk_input_key(&mContext, NK_KEY_UP, get_key_state(gui_special_key::up));

        if (get_key_state(gui_special_key::down) >= 0) 
            nk_input_key(&mContext, NK_KEY_DOWN, get_key_state(gui_special_key::down));

        if (get_key_state(gui_special_key::scroll_up) >= 0) 
            nk_input_key(&mContext, NK_KEY_SCROLL_UP, get_key_state(gui_special_key::scroll_up));

        if (get_key_state(gui_special_key::scroll_down) >= 0) 
            nk_input_key(&mContext, NK_KEY_SCROLL_DOWN, get_key_state(gui_special_key::scroll_down));

        if (get_key_state(gui_special_key::text_reset_mode) >= 0) 
            nk_input_key(&mContext, NK_KEY_TEXT_RESET_MODE, get_key_state(gui_special_key::text_reset_mode));

        if (get_key_state(gui_special_key::text_reset_mode) >= 0) 
            nk_input_key(&mContext, NK_KEY_TEXT_RESET_MODE, get_key_state(gui_special_key::text_reset_mode));

        if (get_key_state(gui_special_key::text_reset_mode) >= 0) 
            nk_input_key(&mContext, NK_KEY_TEXT_RESET_MODE, get_key_state(gui_special_key::text_reset_mode));

        if (get_key_state(gui_special_key::text_reset_mode) >= 0) 
            nk_input_key(&mContext, NK_KEY_TEXT_RESET_MODE, get_key_state(gui_special_key::text_reset_mode));

        if (get_key_state(gui_special_key::f1) >= 0)  
            nk_input_key(&mContext, NK_KEY_F1,  get_key_state(gui_special_key::f1));

        if (get_key_state(gui_special_key::f2) >= 0)  
            nk_input_key(&mContext, NK_KEY_F2,  get_key_state(gui_special_key::f2));

        if (get_key_state(gui_special_key::f3) >= 0)  
            nk_input_key(&mContext, NK_KEY_F3,  get_key_state(gui_special_key::f3));

        if (get_key_state(gui_special_key::f4) >= 0)  
            nk_input_key(&mContext, NK_KEY_F4,  get_key_state(gui_special_key::f4));

        if (get_key_state(gui_special_key::f5) >= 0)  
            nk_input_key(&mContext, NK_KEY_F5,  get_key_state(gui_special_key::f5));

        if (get_key_state(gui_special_key::f6) >= 0)  
            nk_input_key(&mContext, NK_KEY_F6,  get_key_state(gui_special_key::f6));

        if (get_key_state(gui_special_key::f7) >= 0)  
            nk_input_key(&mContext, NK_KEY_F7,  get_key_state(gui_special_key::f7));

        if (get_key_state(gui_special_key::f8) >= 0)  
            nk_input_key(&mContext, NK_KEY_F8,  get_key_state(gui_special_key::f8));

        if (get_key_state(gui_special_key::f9) >= 0)  
            nk_input_key(&mContext, NK_KEY_F9,  get_key_state(gui_special_key::f9));

        if (get_key_state(gui_special_key::f10) >= 0) 
            nk_input_key(&mContext, NK_KEY_F10, get_key_state(gui_special_key::f10));

        if (get_key_state(gui_special_key::f11) >= 0) 
            nk_input_key(&mContext, NK_KEY_F11, get_key_state(gui_special_key::f11));

        if (get_key_state(gui_special_key::f12) >= 0) 
            nk_input_key(&mContext, NK_KEY_F12, get_key_state(gui_special_key::f12));

        if (get_key_state(gui_special_key::insert_mode) >= 0)
            nk_input_key(&mContext, NK_KEY_TEXT_INSERT_MODE, get_key_state(gui_special_key::insert_mode));

        if (get_key_state(gui_special_key::replace_mode) >= 0)
            nk_input_key(&mContext, NK_KEY_TEXT_REPLACE_MODE, get_key_state(gui_special_key::replace_mode));

        nk_input_key(&mContext, NK_KEY_TEXT_START, platform::window_get_key(*rWindow, key_t::home) == input_action::press);
        nk_input_key(&mContext, NK_KEY_TEXT_END, platform::window_get_key(*rWindow, key_t::end) == input_action::press);
        nk_input_key(&mContext, NK_KEY_SCROLL_START, platform::window_get_key(*rWindow, key_t::home) == input_action::press);
        nk_input_key(&mContext, NK_KEY_SCROLL_END, platform::window_get_key(*rWindow, key_t::end) == input_action::press);
        
        nk_input_key
        (   
            &mContext, NK_KEY_SHIFT, 
            platform::window_get_key(*rWindow, key_t::left_shift)  == input_action::press ||
            platform::window_get_key(*rWindow, key_t::right_shift) == input_action::press
        );

        bool eitherControlPressed = platform::window_get_key(*rWindow, key_t::left_control)  == input_action::press ||
                                    platform::window_get_key(*rWindow, key_t::right_control) == input_action::press;

        if (eitherControlPressed) 
        {
            if (get_key_state(gui_special_key::copy) >= 0)
                nk_input_key(&mContext, NK_KEY_COPY, get_key_state(gui_special_key::copy));

            if (get_key_state(gui_special_key::paste) >= 0)
                nk_input_key(&mContext, NK_KEY_PASTE, get_key_state(gui_special_key::paste));

            if (get_key_state(gui_special_key::cut) >= 0)
                nk_input_key(&mContext, NK_KEY_CUT, get_key_state(gui_special_key::cut));

            if (get_key_state(gui_special_key::text_undo) >= 0)
                nk_input_key(&mContext, NK_KEY_TEXT_UNDO, get_key_state(gui_special_key::text_undo));

            if (get_key_state(gui_special_key::text_redo) >= 0)
                nk_input_key(&mContext, NK_KEY_TEXT_REDO, get_key_state(gui_special_key::text_redo));

            if (get_key_state(gui_special_key::line_start) >= 0)
                nk_input_key(&mContext, NK_KEY_TEXT_LINE_START, get_key_state(gui_special_key::line_start));

            if (get_key_state(gui_special_key::line_end) >= 0)
                nk_input_key(&mContext, NK_KEY_TEXT_LINE_END, get_key_state(gui_special_key::line_end));

            if (get_key_state(gui_special_key::select_all) >= 0)
                nk_input_key(&mContext, NK_KEY_TEXT_SELECT_ALL, get_key_state(gui_special_key::select_all));

            if (get_key_state(gui_special_key::left) >= 0)
                nk_input_key(&mContext, NK_KEY_TEXT_WORD_LEFT, get_key_state(gui_special_key::left));

            if (get_key_state(gui_special_key::right) >= 0)
                nk_input_key(&mContext, NK_KEY_TEXT_WORD_RIGHT, get_key_state(gui_special_key::right));
        } else
        {
            if (get_key_state(gui_special_key::left) >= 0)
                nk_input_key(&mContext, NK_KEY_LEFT, get_key_state(gui_special_key::left));

            if (get_key_state(gui_special_key::right) >= 0)
                nk_input_key(&mContext, NK_KEY_RIGHT, get_key_state(gui_special_key::right));

            nk_input_key(&mContext, NK_KEY_COPY, 0);
            nk_input_key(&mContext, NK_KEY_PASTE, 0);
            nk_input_key(&mContext, NK_KEY_CUT, 0);
        }

        // Cursor position:
        f64 x, y;
        platform::window_get_cursor_position(*rWindow, &x, &y);
        nk_input_motion(&mContext, static_cast<i32>(x), static_cast<i32>(y));

        // Mouse grabing position:
        if (mContext.input.mouse.grabbed)
        {
            platform::window_set_cursor_position(*rWindow, mContext.input.mouse.prev.x, mContext.input.mouse.prev.y);
            mContext.input.mouse.pos.x = mContext.input.mouse.prev.x;
            mContext.input.mouse.pos.y = mContext.input.mouse.prev.y;
        }

        // Mouse buttons:
        nk_input_button(&mContext, NK_BUTTON_LEFT, static_cast<i32>(x), static_cast<i32>(y), platform::window_get_mouse_button(*rWindow, mouse_button::left) == input_action::press);
        nk_input_button(&mContext, NK_BUTTON_MIDDLE, static_cast<i32>(x), static_cast<i32>(y), platform::window_get_mouse_button(*rWindow, mouse_button::middle) == input_action::press);
        nk_input_button(&mContext, NK_BUTTON_RIGHT, static_cast<i32>(x), static_cast<i32>(y), platform::window_get_mouse_button(*rWindow, mouse_button::right) == input_action::press);

        nk_input_button(&mContext, NK_BUTTON_DOUBLE, static_cast<i32>(mState.doubleClickPos[0]), static_cast<i32>(mState.doubleClickPos[1]), mState.isDoubleClickDown);
        nk_input_button(&mContext, NK_BUTTON_X1, static_cast<i32>(x), static_cast<i32>(y), platform::window_get_mouse_button(*rWindow, mouse_button::button_4) == input_action::press);
        nk_input_button(&mContext, NK_BUTTON_X2, static_cast<i32>(x), static_cast<i32>(y), platform::window_get_mouse_button(*rWindow, mouse_button::button_5) == input_action::press);
        
        nk_input_scroll(&mContext, { .x = 0, .y = mState.scrollY});
        nk_input_end(&mContext);

        // Reset:
        std::memset(mState.keyState, static_cast<u8>(tristate::neither), sizeof(mState.keyState));
        mState.textLength = 0;
        mState.scrollY = 0;
    }

    bool gui_subsystem::begin_frame(const char* title, const rect_f32& geometry, gui_frame_flag flags)
    {
        nk_flags nkFlags = get_nuklear_flags(flags);

        return nk_begin
        (
            &mContext, 
            title, 
            {
                .x = geometry.x,
                .y = geometry.y,
                .w = geometry.width,
                .h = geometry.height,
            }, 
            nkFlags
        );
    }

    void gui_subsystem::end_frame()
    {
        nk_end(&mContext);
    }

    void gui_subsystem::finish_gui()
    {

    }

    void gui_subsystem::label(const char* text, gui_alignment alignment)
    {
        nk_flags flags = get_nuklear_alignment(alignment);
        nk_label(&mContext, text, flags);
    }
}

namespace har
{
    void gui_subsystem::initialize_callbacks()
    {
        platform::window_set_mouse_button_callback(*rWindow, [&](i32 button, i32 action, i32 mods) -> void
        {
            UNUSED mods;

            if (button != GLFW_MOUSE_BUTTON_LEFT)
                return;

            double x, y;
            platform::window_get_cursor_position(*rWindow, &x, &y);

            if (action != GLFW_PRESS)
            {
                mState.isDoubleClickDown = false;
                return;
            }

            double dt = platform::window_get_time_seconds(*rWindow) - mState.lastButtonClick;
            if (dt > DefaultDoubleClickDelayLow && dt < DefaultDoubleClickDelayHigh)
            {
                mState.isDoubleClickDown = true;
                mState.doubleClickPos[0] = static_cast<f32>(x);
                mState.doubleClickPos[1] = static_cast<f32>(y);
            }

            mState.lastButtonClick = platform::window_get_time_seconds(*rWindow);
        });

        platform::window_set_scroll_callback(*rWindow, [&](f64 x, f64 y) -> void
        {
            UNUSED x;
            mState.scrollY += y;
        });

        platform::window_set_character_callback(*rWindow, [&](u32 codepoint) -> void
        {
            if (mState.textLength >= 255)
                return;
            
            mState.text[mState.textLength++] = codepoint;
        });

        platform::window_set_key_callback(*rWindow, [&](key_t key, i32 scancode, input_action action, i32 mods) -> void
        {
            static bool sInsertToggle = false;

            if (key == key_t::insert && action == input_action::release)
            {
                sInsertToggle = !sInsertToggle;
                if (sInsertToggle)
                    mState.keyState[static_cast<u8>(gui_special_key::insert_mode)] = tristate::no;
                else
                    mState.keyState[static_cast<u8>(gui_special_key::replace_mode)] = tristate::no;
                
                return;
            }

            auto specialKey = get_gui_special_key(key);
            if (specialKey == gui_special_key::invalid)
                return;
            
            mState.keyState[static_cast<u8>(specialKey)] = tristate { action == input_action::press };
        });
    }

    void gui_subsystem::initialize_clipboard()
    {
        mContext.clip.userdata = nk_handle{ .ptr = this };

        mContext.clip.copy = [](nk_handle handle, const char* text, i32 length)
        {
            auto* gui = reinterpret_cast<gui_subsystem*>(handle.ptr);

            if (!text || !length)
                return;

            platform::window_set_clipboard_string(*gui->rWindow, text);
        };

        mContext.clip.paste = [](nk_handle usr, struct nk_text_edit* edit)
        {
            auto* gui = reinterpret_cast<gui_subsystem*>(usr.ptr);
            const char* text = platform::window_get_clipboard_string(*gui->rWindow);
            if (text)
                nk_textedit_paste(edit, text, std::strlen(text));
        };
    }
}