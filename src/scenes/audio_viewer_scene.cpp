#include "audio_viewer_scene.h"
#include "audio_viewer_scene_defs.h"

#include "bn_bg_palettes.h"
#include "bn_colors.h"
#include "bn_keypad.h"
#include "bn_sound_items.h"
#include "bn_string.h"
#include "common_variable_8x16_sprite_font.h"

audio_viewer_scene::audio_viewer_scene() : _text_generator(common::variable_8x16_sprite_font)
{
    bn::bg_palettes::set_transparent_color(bn::color(2, 2, 8));
    _update_display();
}

void audio_viewer_scene::_update_display()
{
    _text_sprites.clear();

    if(audio_viewer_defs::sound_count == 0)
    {
        _text_generator.set_center_alignment();
        _text_generator.generate(0, -20, "NO WAV FILES FOUND", _text_sprites);
        _text_generator.generate(0, 75, "SELECT: Model Viewer  START: Title", _text_sprites);
        _text_generator.set_left_alignment();
        return;
    }

    _text_generator.set_center_alignment();
    bn::string<48> title("AUDIO VIEWER");
    _text_generator.generate(0, -70, title, _text_sprites);

    bn::string<48> counter;
    counter.append(bn::to_string<4>(_current_index + 1));
    counter.push_back('/');
    counter.append(bn::to_string<4>(audio_viewer_defs::sound_count));
    _text_generator.generate(0, -55, counter, _text_sprites);

    const int ITEMS_TO_SHOW = 5;
    const int CENTER_OFFSET = ITEMS_TO_SHOW / 2;
    const int LINE_HEIGHT = 12;
    const int START_Y = -10;

    for(int item_offset = 0; item_offset < ITEMS_TO_SHOW; ++item_offset)
    {
        int sound_index = _current_index - CENTER_OFFSET + item_offset;

        while(sound_index < 0) sound_index += audio_viewer_defs::sound_count;
        while(sound_index >= audio_viewer_defs::sound_count) sound_index -= audio_viewer_defs::sound_count;

        const char* name = audio_viewer_defs::entries[sound_index].name;
        int y_pos = START_Y + (item_offset - CENTER_OFFSET) * LINE_HEIGHT;

        if(item_offset == CENTER_OFFSET)
        {
            bn::string<48> selected("> ");
            selected.append(name);
            selected.append(" <");
            _text_generator.generate(0, y_pos, selected, _text_sprites);
        }
        else
        {
            _text_generator.generate(0, y_pos, name, _text_sprites);
        }
    }

    _text_generator.generate(0, 45, "A: Play Sound", _text_sprites);
    _text_generator.generate(0, 75, "SELECT: Model Viewer  START: Title", _text_sprites);

    _text_generator.set_left_alignment();
}

bn::optional<scene_type> audio_viewer_scene::update()
{
    bn::optional<scene_type> result;

    if(bn::keypad::select_pressed())
    {
        result = scene_type::MODEL_VIEWER;
        bn::sound_items::menu_confirm.play();
    }
    else if(bn::keypad::start_pressed())
    {
        result = scene_type::TITLE;
        bn::sound_items::menu_confirm.play();
    }
    else if(audio_viewer_defs::sound_count > 0)
    {
        if(bn::keypad::down_pressed())
        {
            _current_index = (_current_index + 1) % audio_viewer_defs::sound_count;
            bn::sound_items::menu_focus.play();
            _update_display();
        }
        else if(bn::keypad::up_pressed())
        {
            _current_index = (_current_index + audio_viewer_defs::sound_count - 1) % audio_viewer_defs::sound_count;
            bn::sound_items::menu_focus.play();
            _update_display();
        }
        else if(bn::keypad::a_pressed())
        {
            audio_viewer_defs::entries[_current_index].item.play();
            _update_display();
        }
    }

    return result;
}