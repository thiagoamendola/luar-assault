#ifndef DIALOG_MANAGER_H
#define DIALOG_MANAGER_H

#include "bn_fixed.h"
#include "bn_optional.h"
#include "bn_string.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

#include "dialog_command.h"
#include "stage_section.h"

enum class dialog_state
{
    HIDDEN,
    OPENING,
    OPEN,
    CLOSING
};

class dialog_manager
{
public:
    dialog_manager();

    void update(stage_section_list_ptr sections, size_t sections_count, bn::fixed camera_y);

    void add_subtitle_command(const char* subtitle, int start_time, int duration);
    void add_tutorial_command(const char* text, int start_time, int duration);
    void suspend_for_pause();

private:
    // General constants
    static constexpr int HIDE_DELAY_FRAMES = 45;
    static constexpr int TRANSITION_FRAMES = 5;
    static constexpr int MAX_DIALOG_COMMANDS = 32;

    // Tutorial dialog build constants
    static constexpr int TUTORIAL_START_X = -95;
    static constexpr int TUTORIAL_START_Y = 48;
    static constexpr int TUTORIAL_COLS = 14;
    static constexpr int TUTORIAL_ROWS = 2;
    static constexpr int TUTORIAL_TILE_SPACING = 15;
    static constexpr int TUTORIAL_MAX_SPRITES = TUTORIAL_COLS * TUTORIAL_ROWS;
    static constexpr int TUTORIAL_X = 0;
    static constexpr int TUTORIAL_Y = 53;
    static constexpr int MAX_TUTORIAL_CHARS_PER_LINE = 28;
    static constexpr int TUTORIAL_LINE_HEIGHT = 10;

    // Subtitle text constants
    static constexpr int SUBTITLE_X = 20;
    static constexpr int SUBTITLE_Y = 52;
    static constexpr int CHAR_SPEED = 1;
    static constexpr int MAX_SUBTITLE_CHARS_PER_LINE = 26;
    static constexpr int SUBTITLE_LINE_HEIGHT = 10;

    // Subtitle dialog build constants
    static constexpr int DIALOG_START_X = -63;
    static constexpr int DIALOG_START_Y = 38;
    static constexpr int PORTRAIT_X = -80;
    static constexpr int PORTRAIT_Y = 62;
    static constexpr int PORTRAIT_ANIMATION_FRAME_DELAY = 7;
    static constexpr int DIALOG_COLS = 12;
    static constexpr int DIALOG_ROWS = 3;
    static constexpr int DIALOG_INNER_SPRITES = (DIALOG_COLS - 2) * (DIALOG_ROWS - 2);
    static constexpr int SUBTITLE_DIALOG_MAX_SPRITES = DIALOG_INNER_SPRITES + (DIALOG_ROWS - 1) +
                                             (DIALOG_COLS - 2) + (DIALOG_COLS - 1) +
                                             (DIALOG_ROWS - 1) + 3;
    static constexpr int DIALOG_TILE_SPACING = 15;
    static constexpr int UPPER_LEFT_BORDER_GRAPHICS_INDEX = 0;
    static constexpr int LOWER_RIGHT_BORDER_GRAPHICS_INDEX = 1;
    static constexpr int UPPER_RIGHT_BORDER_GRAPHICS_INDEX = 2;
    static constexpr int LOWER_LEFT_BORDER_GRAPHICS_INDEX = 3;
    static constexpr int CORNER_Z_ORDER = 0;
    static constexpr int INNER_BLOCKS_Z_ORDER = 4;
    static constexpr int RIGHT_BOTTOM_BORDER_Z_ORDER = 3;
    static constexpr int UPPER_BORDER_Z_ORDER = 2;
    static constexpr int LEFT_BORDER_Z_ORDER = 1;

    bn::sprite_text_generator _dialog_text_generator;
    bn::vector<dialog_command, MAX_DIALOG_COMMANDS> _dialog_commands;
    bn::optional<bn::vector<bn::sprite_ptr, SUBTITLE_DIALOG_MAX_SPRITES>> _subtitle_dialog_sprites;
    bn::optional<bn::vector<bn::sprite_ptr, TUTORIAL_MAX_SPRITES>> _tutorial_dialog_sprites;
    bn::vector<bn::sprite_ptr, 40> _dialog_text_sprites;
    bn::string<64> _dialog_text;
    bn::string<64> _wrapped_dialog_text;

    dialog_state _dialog_state = dialog_state::HIDDEN;
    dialog_command_type _active_dialog_type = dialog_command_type::SUBTITLE;
    dialog_character _active_dialog_character = dialog_character::TEST;

    int _pending_dialog_command_index = -1;
    int _elapsed_frames = 0;
    int _active_dialog_end_frame = -1;
    int _hide_dialog_frame = -1;
    int _transition_frame = 0;
    bn::fixed _last_section_start_y = bn::fixed(32767);
    int _portrait_animation_frame_counter = 0;
    int _portrait_graphics_index = 0;
    int _dialog_character_index = 0;
    int _dialog_frame_counter = 0;
    bool _suspended_for_pause = false;

    void _show_dialog(dialog_command_type type, dialog_character character);
    void _hide_dialog();
    void _build_subtitle_dialog_box();
    void _build_tutorial_dialog_box();
    void _add_dialog_command(const char* text, int start_time, int duration, dialog_command_type type,
                             dialog_character character);
    void _start_dialog_command(int command_index);
    void _set_visible(bool visible);
    void _update_dialog_transition();
    void _start_closing_dialog();

    void _process_section(stage_section_list_ptr sections, size_t sections_count, bn::fixed camera_y);
    void _resume_from_pause();

    void _start_dialog_text(const dialog_command& command);
    void _update_dialog_text();
    void _wrap_dialog_text(const char* text);
    void _render_dialog_text();

    bn::sprite_ptr _create_portrait_sprite() const;
    void _set_portrait_graphics_index(int graphics_index);
    void _update_portrait_animation();

    void _set_blending_enabled(bool blending_enabled);
    void _clear_blending();
};

#endif