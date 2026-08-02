#ifndef DIALOG_MANAGER_H
#define DIALOG_MANAGER_H

#include "bn_string.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

class dialog_manager
{
public:
    dialog_manager();

    void update();

    void add_subtitle_command(const char* subtitle, int start_time, int duration);

private:
    // General constants
    static constexpr int HIDE_DELAY_FRAMES = 45;
    static constexpr int MAX_SUBTITLE_COMMANDS = 32;

    // Subtitle text constants
    static constexpr int SUBTITLE_X = 20;
    static constexpr int SUBTITLE_Y = 52;
    static constexpr int CHAR_SPEED = 1;
    static constexpr int MAX_SUBTITLE_CHARS_PER_LINE = 26;
    static constexpr int SUBTITLE_LINE_HEIGHT = 10;

    // Dialog build constants
    static constexpr int DIALOG_START_X = -65;
    static constexpr int DIALOG_START_Y = 38;
    static constexpr int DIALOG_COLS = 12;
    static constexpr int DIALOG_ROWS = 3;
    static constexpr int DIALOG_INNER_SPRITES = (DIALOG_COLS - 2) * (DIALOG_ROWS - 2);
    static constexpr int DIALOG_MAX_SPRITES = DIALOG_INNER_SPRITES + (DIALOG_ROWS - 1) +
                                             (DIALOG_COLS - 2) + (DIALOG_COLS - 1) +
                                             (DIALOG_ROWS - 1) + 2;
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

    struct subtitle_command
    {
        const char* subtitle;
        int start_time;
        int duration;
    };

    bn::sprite_text_generator _subtitle_text_generator;
    bn::vector<subtitle_command, MAX_SUBTITLE_COMMANDS> _subtitle_commands;
    bn::vector<bn::sprite_ptr, DIALOG_MAX_SPRITES> _dialog_sprites;
    bn::vector<bn::sprite_ptr, 40> _subtitle_text_sprites;
    bn::string<64> _subtitle_text;
    bn::string<64> _wrapped_subtitle_text;
    int _elapsed_frames = 0;
    int _active_subtitle_command_index = -1;
    int _active_subtitle_end_frame = -1;
    int _hide_dialog_frame = -1;
    int _subtitle_character_index = 0;
    int _subtitle_frame_counter = 0;
    bool _dialog_shown = false;

    void _show_dialog();
    void _hide_dialog();
    void _start_subtitle(int command_index);
    void _update_subtitle_text();
    void _wrap_subtitle_text(const char* subtitle);
    void _render_subtitle_text();
};

#endif