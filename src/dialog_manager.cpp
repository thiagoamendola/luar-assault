#include "dialog_manager.h"

#include "bn_sprite_items_dialog_bg.h"
#include "bn_utility.h"

#include "common_variable_8x16_sprite_font.h"

dialog_manager::dialog_manager() :
    _subtitle_text_generator(common::variable_8x16_sprite_font)
{
    _subtitle_text_generator.set_bg_priority(0);
    _subtitle_text_generator.set_z_order(-1);
    _subtitle_text_generator.set_center_alignment();

}

void dialog_manager::update()
{
    for (int index = 0; index < _subtitle_commands.size(); ++index)
    {
        const subtitle_command& command = _subtitle_commands[index];
        if (command.start_time == _elapsed_frames)
        {
            _start_subtitle(index);
        }
    }

    _update_subtitle_text();

    if (_active_subtitle_command_index >= 0)
    {
        const subtitle_command& command = _subtitle_commands[_active_subtitle_command_index];
        if (_elapsed_frames >= command.start_time + command.duration)
        {
            _active_subtitle_command_index = -1;
            _hide_dialog_frame = _elapsed_frames + HIDE_DELAY_FRAMES;
        }
    }

    if (_hide_dialog_frame >= 0 && _elapsed_frames >= _hide_dialog_frame)
    {
        _hide_dialog();
    }

    ++_elapsed_frames;
}

void dialog_manager::_show_dialog()
{
    if (_dialog_shown)
    {
        return;
    }

    _dialog_shown = true;

    // Create lambda for tile creation
    auto add_dialog_sprite = [this](int col, int row, int graphics_index, int z_order)
    {
        const int x = DIALOG_START_X + col * DIALOG_TILE_SPACING;
        const int y = DIALOG_START_Y + row * DIALOG_TILE_SPACING;

        bn::sprite_ptr sprite = bn::sprite_items::dialog_bg.create_sprite(x, y, graphics_index);
        sprite.set_bg_priority(0);
        sprite.set_z_order(z_order);
        _dialog_sprites.push_back(bn::move(sprite));
    };

    // Add inner tiles.
    for (int row = DIALOG_ROWS - 2; row >= 1; --row)
    {
        for (int col = DIALOG_COLS - 2; col >= 1; --col)
        {
            add_dialog_sprite(col, row, UPPER_LEFT_BORDER_GRAPHICS_INDEX, INNER_BLOCKS_Z_ORDER);
        }
    }

    // Add border tiles.
    for (int row = 1; row < DIALOG_ROWS - 1; ++row)
    {
        add_dialog_sprite(DIALOG_COLS - 1, row, LOWER_RIGHT_BORDER_GRAPHICS_INDEX, RIGHT_BOTTOM_BORDER_Z_ORDER);
    }
    for (int col = 1; col < DIALOG_COLS - 1; ++col)
    {
        add_dialog_sprite(col, DIALOG_ROWS - 1, LOWER_RIGHT_BORDER_GRAPHICS_INDEX, RIGHT_BOTTOM_BORDER_Z_ORDER);
    }
    for (int col = DIALOG_COLS - 2; col >= 0; --col)
    {
        add_dialog_sprite(col, 0, UPPER_LEFT_BORDER_GRAPHICS_INDEX, UPPER_BORDER_Z_ORDER);
    }
    for (int row = DIALOG_ROWS - 2; row >= 0; --row)
    {
        add_dialog_sprite(0, row, UPPER_LEFT_BORDER_GRAPHICS_INDEX, LEFT_BORDER_Z_ORDER);
    }

    // Add remaining corner tiles.
    add_dialog_sprite(DIALOG_COLS - 1, 0, UPPER_RIGHT_BORDER_GRAPHICS_INDEX, CORNER_Z_ORDER);
    add_dialog_sprite(0, DIALOG_ROWS - 1, LOWER_LEFT_BORDER_GRAPHICS_INDEX, CORNER_Z_ORDER);
    add_dialog_sprite(DIALOG_COLS - 1, DIALOG_ROWS - 1, LOWER_RIGHT_BORDER_GRAPHICS_INDEX, CORNER_Z_ORDER);
}

void dialog_manager::_hide_dialog()
{
    _dialog_sprites.clear();
    _subtitle_text_sprites.clear();
    _subtitle_text.clear();
    _active_subtitle_command_index = -1;
    _hide_dialog_frame = -1;
    _subtitle_character_index = 0;
    _subtitle_frame_counter = 0;
    _dialog_shown = false;
}

void dialog_manager::_start_subtitle(int command_index)
{
    _show_dialog();
    _subtitle_text_sprites.clear();
    _subtitle_text.clear();
    _active_subtitle_command_index = command_index;
    _hide_dialog_frame = -1;
    _subtitle_character_index = 0;
    _subtitle_frame_counter = 0;
}

void dialog_manager::_update_subtitle_text()
{
    if (_active_subtitle_command_index < 0)
    {
        return;
    }

    const char* subtitle_text = _subtitle_commands[_active_subtitle_command_index].subtitle;
    if (subtitle_text[_subtitle_character_index] == '\0')
    {
        return;
    }

    ++_subtitle_frame_counter;
    if (_subtitle_frame_counter < CHAR_SPEED)
    {
        return;
    }

    _subtitle_frame_counter = 0;
    _subtitle_text.push_back(subtitle_text[_subtitle_character_index]);
    ++_subtitle_character_index;

    _subtitle_text_sprites.clear();
    _subtitle_text_generator.generate(SUBTITLE_X, SUBTITLE_Y, _subtitle_text, _subtitle_text_sprites);
}

void dialog_manager::add_subtitle_command(const char* subtitle, int start_time, int duration)
{
    if (_subtitle_commands.size() < MAX_SUBTITLE_COMMANDS)
    {
        _subtitle_commands.push_back({ subtitle, start_time, duration });
    }
}
