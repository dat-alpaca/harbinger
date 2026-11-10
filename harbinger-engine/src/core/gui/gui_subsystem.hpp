#pragma once
#include "core/defines.hpp"
#include "platform/window.hpp"
#include "renderer/renderer.hpp"
#include "rhi/common/rect.hpp"
#include <bitset>
#include <nuklear.h>

namespace har
{
    enum class gui_frame_flag : u16
    {
        none                = 0,

        border              = 1 << 0,
        movable             = 1 << 1,
        scalable            = 1 << 2,
        closable            = 1 << 3,
        minimizable         = 1 << 4,
        no_scrollbar        = 1 << 5,
        title               = 1 << 6,
        scroll_auto_hide    = 1 << 7,
        background          = 1 << 8,
        scale_left          = 1 << 9,
        no_input            = 1 << 10,

        default_flags       = border | movable | scalable | minimizable | title,
    };
    HAR_DEFINE_ENUM(gui_frame_flag);

    enum class gui_alignment : u8
    {
        left,
        center,
        right
    };

    static constexpr inline u8 GUISpecialKeyCount = 35;
    enum class gui_special_key : u8
    {
        invalid,

        del,
        tab,
        backspace,
        up,
        down,
        left,
        right,

        alt,
        scroll_up,
        scroll_down,

        f1,
        f2,
        f3,
        f4,
        f5,
        f6,
        f7,
        f8,
        f9,
        f10,
        f11,
        f12,

        copy,
        paste,
        cut,
        text_undo,
        text_redo,
        line_start,
        line_end,
        select_all,
        enter,

        text_reset_mode,
        insert_mode,
        replace_mode,
    };

    struct gui_state
    {
        tristate keyState[GUISpecialKeyCount];
        u32 text[256];

        f64 lastButtonClick = 0;

        // f32 scrollX;
        f32 scrollY;

        f32 lastDeltaTimeSeconds;
        f32 currentDeltaTimeSeconds;

        i32 windowWidth, windowHeight;
        i32 fbWidth, fbHeight;
        f32 fbScaleX, fbScaleY;

        f32 doubleClickPos[2];
        u8 textLength;
        bool isDoubleClickDown : 1 = false;
    };
    
    class gui_subsystem
    {
    public:
        void initialize(NON_OWNING window_t* window, NON_OWNING harbinger_renderer* renderer);

    public:
        void start_gui();
        bool begin_frame(const char* title, const rect_f32& geometry, gui_frame_flag flags = gui_frame_flag::default_flags);
        void end_frame();
        void finish_gui();

        void label(const char* text, gui_alignment alignment);

    private:
        void initialize_callbacks();
        void initialize_clipboard();

    private:
        NON_OWNING harbinger_renderer* rRenderer = nullptr;
        NON_OWNING window_t* rWindow = nullptr;

    private:
        nk_context mContext;
        gui_state mState;

    private:
        static inline constexpr f64 DefaultDoubleClickDelayHigh = 0.02;
        static inline constexpr f64 DefaultDoubleClickDelayLow = 0.2;
    };
}