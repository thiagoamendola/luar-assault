#include "dialog_manager.h"

#include "bn_blending.h"
#include "bn_sprite_items_dialog_bg.h"
#include "bn_sprite_items_test_portrait.h"
#include "bn_sprite_items_tutorial_bg.h"
#include "bn_utility.h"

#include "common_variable_8x16_sprite_font.h"
#include "stage_section.h"

dialog_manager::dialog_manager() :
    _dialog_text_generator(common::variable_8x16_sprite_font)
{
    _dialog_text_generator.set_bg_priority(0);
    _dialog_text_generator.set_z_order(-1);
    _dialog_text_generator.set_center_alignment();

}

void dialog_manager::update(stage_section_list_ptr sections, size_t sections_count, bn::fixed camera_y)
{
    _resume_from_pause();

    if (_suspended_for_pause)
    {
        return;
    }

    _process_section(sections, sections_count, camera_y);

    for (int index = 0; index < _dialog_commands.size(); ++index)
    {
        const dialog_command& command = _dialog_commands[index];
        if (command.start_time == _elapsed_frames)
        {
            _start_dialog_command(index);
        }
    }

    _update_dialog_transition();
    _update_dialog_text();
    _update_portrait_animation();

    if (_active_dialog_end_frame >= 0)
    {
        if (_elapsed_frames >= _active_dialog_end_frame)
        {
            _active_dialog_end_frame = -1;
            _hide_dialog_frame = _elapsed_frames + HIDE_DELAY_FRAMES;
        }
    }

    if (_hide_dialog_frame >= 0 && _elapsed_frames >= _hide_dialog_frame)
    {
        _start_closing_dialog();
    }

    ++_elapsed_frames;
}

void dialog_manager::_process_section(stage_section_list_ptr sections, size_t sections_count, bn::fixed camera_y)
{
    for (size_t index = 0; index < sections_count; ++index)
    {
        const stage_section* section = sections[index];
        if (camera_y <= section->starting_pos() && section->starting_pos() < _last_section_start_y)
        {
            _last_section_start_y = section->starting_pos();

            for (int command_index = 0; command_index < section->dialog_commands_count(); ++command_index)
            {
                const dialog_command& command = section->dialog_commands()[command_index];
                _add_dialog_command(command.text, _elapsed_frames + command.start_time,
                                    command.duration, command.type);
            }
        }
    }
}

void dialog_manager::_show_dialog(dialog_command_type type)
{
    if (_dialog_state != dialog_state::HIDDEN && _active_dialog_type == type)
    {
        return;
    }

    // Resets unused sprites.
    _subtitle_dialog_sprites.reset();
    _tutorial_dialog_sprites.reset();
    _active_dialog_type = type;

    // Create dialog of the given type.
    if (type == dialog_command_type::TUTORIAL)
    {
        _build_tutorial_dialog_box();
    }
    else
    {
        _build_subtitle_dialog_box();
    }
}

void dialog_manager::_build_subtitle_dialog_box()
{
    const bool blending_enabled = _dialog_state != dialog_state::OPEN;
    _subtitle_dialog_sprites.emplace();
    _portrait_animation_frame_counter = 0;
    _portrait_graphics_index = 0;

    bn::sprite_ptr portrait_sprite = bn::sprite_items::test_portrait.create_sprite(PORTRAIT_X, PORTRAIT_Y, 0);
    portrait_sprite.set_bg_priority(0);
    portrait_sprite.set_z_order(-1);
    portrait_sprite.set_blending_enabled(blending_enabled);
    _subtitle_dialog_sprites.value().push_back(bn::move(portrait_sprite));

    // Create lambda for tile creation
    auto add_dialog_sprite = [this, blending_enabled](int col, int row, int graphics_index, int z_order)
    {
        const int x = DIALOG_START_X + col * DIALOG_TILE_SPACING;
        const int y = DIALOG_START_Y + row * DIALOG_TILE_SPACING;

        bn::sprite_ptr sprite = bn::sprite_items::dialog_bg.create_sprite(x, y, graphics_index);
        sprite.set_bg_priority(0);
        sprite.set_z_order(z_order);
        sprite.set_blending_enabled(blending_enabled);
        _subtitle_dialog_sprites.value().push_back(bn::move(sprite));
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

void dialog_manager::_build_tutorial_dialog_box()
{
    const bool blending_enabled = _dialog_state != dialog_state::OPEN;
    _tutorial_dialog_sprites.emplace();

    for (int row = 0; row < TUTORIAL_ROWS; ++row)
    {
        for (int col = 0; col < TUTORIAL_COLS; ++col)
        {
            const int x = TUTORIAL_START_X + col * TUTORIAL_TILE_SPACING;
            const int y = TUTORIAL_START_Y + row * TUTORIAL_TILE_SPACING;

            bn::sprite_ptr sprite = bn::sprite_items::tutorial_bg.create_sprite(x, y);
            sprite.set_bg_priority(0);
            sprite.set_z_order(INNER_BLOCKS_Z_ORDER);
            sprite.set_blending_enabled(blending_enabled);
            _tutorial_dialog_sprites.value().push_back(bn::move(sprite));
        }
    }
}

void dialog_manager::_hide_dialog()
{
    _clear_blending();
    // Clear existing dialog box sprites.
    _subtitle_dialog_sprites.reset();
    _tutorial_dialog_sprites.reset();
    // Clear text-related variables.
    _dialog_text_sprites.clear();
    _dialog_text.clear();
    _wrapped_dialog_text.clear();

    _pending_dialog_command_index = -1;
    _active_dialog_end_frame = -1;
    _hide_dialog_frame = -1;
    _transition_frame = 0;
    _portrait_animation_frame_counter = 0;
    _portrait_graphics_index = 0;
    _dialog_character_index = 0;
    _dialog_frame_counter = 0;
    _dialog_state = dialog_state::HIDDEN;
}

void dialog_manager::suspend_for_pause()
{
    if (_suspended_for_pause)
    {
        return;
    }

    _suspended_for_pause = true;
    _set_visible(false);
}

void dialog_manager::_resume_from_pause()
{
    if (!_suspended_for_pause)
    {
        return;
    }

    _suspended_for_pause = false;

    if (_dialog_state == dialog_state::HIDDEN)
    {
        return;
    }

    _set_visible(true);

    if (_dialog_state != dialog_state::OPENING && _dialog_state != dialog_state::CLOSING)
    {
        _clear_blending();
    }
}

void dialog_manager::_set_visible(bool visible)
{
    if (_subtitle_dialog_sprites.has_value())
    {
        for (bn::sprite_ptr& sprite : _subtitle_dialog_sprites.value())
        {
            sprite.set_visible(visible);
        }
    }

    if (_tutorial_dialog_sprites.has_value())
    {
        for (bn::sprite_ptr& sprite : _tutorial_dialog_sprites.value())
        {
            sprite.set_visible(visible);
        }
    }

    for (bn::sprite_ptr& sprite : _dialog_text_sprites)
    {
        sprite.set_visible(visible);
    }
}

void dialog_manager::_add_dialog_command(const char* text, int start_time, int duration, dialog_command_type type)
{
    if (_dialog_commands.size() < MAX_DIALOG_COMMANDS)
    {
        _dialog_commands.push_back({ text, start_time, duration, type });
    }
}

void dialog_manager::_start_dialog_command(int command_index)
{
    const dialog_command& command = _dialog_commands[command_index];

    if (_dialog_state == dialog_state::HIDDEN)
    {
        _show_dialog(command.type);
        _pending_dialog_command_index = command_index;
        _transition_frame = 0;
        _hide_dialog_frame = -1;
        _dialog_state = dialog_state::OPENING;
        return;
    }

    if (_dialog_state == dialog_state::CLOSING)
    {
        _show_dialog(command.type);
        _pending_dialog_command_index = command_index;
        _transition_frame = TRANSITION_FRAMES - _transition_frame;
        _hide_dialog_frame = -1;
        _dialog_state = dialog_state::OPENING;
        return;
    }

    if (_dialog_state == dialog_state::OPENING)
    {
        _show_dialog(command.type);
        _pending_dialog_command_index = command_index;
        _hide_dialog_frame = -1;
        return;
    }

    _start_dialog_text(command);
    _pending_dialog_command_index = -1;
}

void dialog_manager::_start_dialog_text(const dialog_command& command)
{
    // Show the dialog box if it's not already shown.
    _show_dialog(command.type);
    // Clear existing text.
    _dialog_text_sprites.clear();
    _dialog_text.clear();
    // Wrap provided text to fit within the dialog box.
    _wrap_dialog_text(command.text);
    _active_dialog_end_frame = _elapsed_frames + command.duration;
    _hide_dialog_frame = -1;
    _dialog_character_index = 0;
    _dialog_frame_counter = 0;

    if (command.type == dialog_command_type::TUTORIAL)
    {
        _dialog_text = _wrapped_dialog_text;
        _dialog_character_index = _wrapped_dialog_text.size();
        _render_dialog_text();
    }
}

void dialog_manager::_update_dialog_transition()
{
    if (_dialog_state == dialog_state::OPENING)
    {
        ++_transition_frame;
        bn::fixed alpha = (1.0 * _transition_frame) / TRANSITION_FRAMES;
        if (alpha > 1.0)
        {
            alpha = 1.0;
        }
        bn::blending::set_transparency_alpha(alpha);

        if (_transition_frame >= TRANSITION_FRAMES)
        {
            _clear_blending();
            _transition_frame = 0;
            _dialog_state = dialog_state::OPEN;

            if (_pending_dialog_command_index >= 0)
            {
                int command_index = _pending_dialog_command_index;
                _start_dialog_text(_dialog_commands[command_index]);
                _pending_dialog_command_index = -1;
            }
        }
    }
    else if (_dialog_state == dialog_state::CLOSING)
    {
        ++_transition_frame;
        bn::fixed alpha = 1.0 - (1.0 * _transition_frame) / TRANSITION_FRAMES;
        if (alpha < 0.0)
        {
            alpha = 0.0;
        }
        bn::blending::set_transparency_alpha(alpha);

        if (_transition_frame >= TRANSITION_FRAMES)
        {
            _hide_dialog();
        }
    }
}

void dialog_manager::_set_blending_enabled(bool blending_enabled)
{
    if (_subtitle_dialog_sprites.has_value())
    {
        for (bn::sprite_ptr& sprite : _subtitle_dialog_sprites.value())
        {
            sprite.set_blending_enabled(blending_enabled);
        }
    }

    if (_tutorial_dialog_sprites.has_value())
    {
        for (bn::sprite_ptr& sprite : _tutorial_dialog_sprites.value())
        {
            sprite.set_blending_enabled(blending_enabled);
        }
    }

    for (bn::sprite_ptr& sprite : _dialog_text_sprites)
    {
        sprite.set_blending_enabled(blending_enabled);
    }
}

void dialog_manager::_clear_blending()
{
    _set_blending_enabled(false);
    bn::blending::set_transparency_alpha(1.0);
}

void dialog_manager::_start_closing_dialog()
{
    if (_dialog_state != dialog_state::OPEN)
    {
        return;
    }

    _dialog_text_sprites.clear();
    _dialog_text.clear();
    _wrapped_dialog_text.clear();
    _hide_dialog_frame = -1;
    _transition_frame = 0;
    _set_blending_enabled(true);
    bn::blending::set_transparency_alpha(1.0);
    _dialog_state = dialog_state::CLOSING;
}

void dialog_manager::_update_dialog_text()
{
    if (_active_dialog_end_frame < 0)
    {
        return;
    }

    if (_dialog_character_index >= _wrapped_dialog_text.size())
    {
        return;
    }

    _dialog_frame_counter++;
    if (_dialog_frame_counter < CHAR_SPEED)
    {
        return;
    }

    _dialog_frame_counter = 0;
    _dialog_text.push_back(_wrapped_dialog_text[_dialog_character_index]);
    _dialog_character_index++;

    _render_dialog_text();
}

void dialog_manager::_update_portrait_animation()
{
    if (!_subtitle_dialog_sprites.has_value() || _active_dialog_end_frame < 0 ||
        _dialog_character_index >= _wrapped_dialog_text.size())
    {
        return;
    }

    ++_portrait_animation_frame_counter;
    if (_portrait_animation_frame_counter < PORTRAIT_ANIMATION_FRAME_DELAY)
    {
        return;
    }

    _portrait_animation_frame_counter = 0;
    _portrait_graphics_index = 1 - _portrait_graphics_index;
    _subtitle_dialog_sprites.value()[0].set_tiles(bn::sprite_items::test_portrait.tiles_item(), _portrait_graphics_index);
}

void dialog_manager::_wrap_dialog_text(const char* text)
{
    _wrapped_dialog_text.clear();

    int text_length = 0;
    for (; text[text_length] != '\0'; ++text_length)
    {
        _wrapped_dialog_text.push_back(text[text_length]);
    }

    const int max_chars_per_line = _active_dialog_type == dialog_command_type::TUTORIAL ?
                                   MAX_TUTORIAL_CHARS_PER_LINE : MAX_SUBTITLE_CHARS_PER_LINE;

    if (text_length <= max_chars_per_line)
    {
        return;
    }

    _wrapped_dialog_text.clear();

    int line_length = 0;
    int word_start_index = -1;
    int word_length = 0;

    for (int input_index = 0; text[input_index] != '\0'; input_index++)
    {
        char character = text[input_index];

        if (character == ' ')
        {
            word_start_index = -1;
            word_length = 0;
            _wrapped_dialog_text.push_back(character);
            line_length++;
            continue;
        }

        if (word_start_index < 0)
        {
            word_start_index = _wrapped_dialog_text.size();
            word_length = 0;
        }

        _wrapped_dialog_text.push_back(character);
        line_length++;
        word_length++;

        if (line_length > max_chars_per_line && word_start_index > 0)
        {
            _wrapped_dialog_text[word_start_index - 1] = '\n';
            line_length = word_length;
        }
    }
}

void dialog_manager::_render_dialog_text()
{
    bn::string<64> first_line;
    bn::string<64> second_line;
    bool second_line_active = false;

    for (char character : _dialog_text)
    {
        if (character == '\n')
        {
            second_line_active = true;
            continue;
        }

        if (second_line_active)
        {
            second_line.push_back(character);
        }
        else
        {
            first_line.push_back(character);
        }
    }

    _dialog_text_sprites.clear();
    if (!second_line.empty())
    {
        int text_x = SUBTITLE_X;
        int text_y = SUBTITLE_Y;
        int line_height = SUBTITLE_LINE_HEIGHT;
        if (_active_dialog_type == dialog_command_type::TUTORIAL)
        {
            text_x = TUTORIAL_X;
            text_y = TUTORIAL_Y;
            line_height = TUTORIAL_LINE_HEIGHT;
        }

        _dialog_text_generator.generate(text_x, text_y - line_height / 2, first_line,
                        _dialog_text_sprites);
        _dialog_text_generator.generate(text_x, text_y + line_height / 2, second_line,
                        _dialog_text_sprites);
    }
    else
    {
        int text_x = SUBTITLE_X;
        int text_y = SUBTITLE_Y;
        if (_active_dialog_type == dialog_command_type::TUTORIAL)
        {
            text_x = TUTORIAL_X;
            text_y = TUTORIAL_Y;
        }

        _dialog_text_generator.generate(text_x, text_y, first_line, _dialog_text_sprites);
    }

    if (_dialog_state == dialog_state::OPENING || _dialog_state == dialog_state::CLOSING)
    {
        for (bn::sprite_ptr& sprite : _dialog_text_sprites)
        {
            sprite.set_blending_enabled(true);
        }
    }
}

void dialog_manager::add_subtitle_command(const char* subtitle, int start_time, int duration)
{
    _add_dialog_command(subtitle, start_time, duration, dialog_command_type::SUBTITLE);
}

void dialog_manager::add_tutorial_command(const char* text, int start_time, int duration)
{
    _add_dialog_command(text, start_time, duration, dialog_command_type::TUTORIAL);
}
