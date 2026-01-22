#include "audio_viewer_scene.h"
#include "audio_viewer_scene_defs.h"

#include "bn_bg_palettes.h"
#include "bn_colors.h"
#include "bn_keypad.h"
#include "bn_music.h"
#include "bn_sound_items.h"
#include "bn_string.h"
#include "common_variable_8x16_sprite_font.h"

audio_viewer_scene::audio_viewer_scene() : _text_generator(common::variable_8x16_sprite_font)
{
    bn::bg_palettes::set_transparent_color(bn::color(2, 2, 8));
    _update_display();
}

audio_viewer_scene::~audio_viewer_scene()
{
    bn::music::stop();
}

void audio_viewer_scene::_update_display()
{
    _text_sprites.clear();
    
    if(audio_viewer_defs::music_count == 0)
    {
        _text_generator.set_center_alignment();
        _text_generator.generate(0, -20, "NO MUSIC FILES FOUND", _text_sprites);
        _text_generator.generate(0, 75, "SELECT: Model Viewer  START: Title", _text_sprites);
        _text_generator.set_left_alignment();
        return;
    }
    
    // Title
    _text_generator.set_center_alignment();
    bn::string<48> title("AUDIO VIEWER");
    _text_generator.generate(0, -70, title, _text_sprites);
    
    // Music counter
    bn::string<48> counter;
    counter.append(bn::to_string<4>(_current_index + 1));
    counter.push_back('/');
    counter.append(bn::to_string<4>(audio_viewer_defs::music_count));
    _text_generator.generate(0, -55, counter, _text_sprites);
    
    // Display music list (show 5 items: 2 above, current, 2 below)
    const int ITEMS_TO_SHOW = 5;
    const int CENTER_OFFSET = ITEMS_TO_SHOW / 2;
    const int LINE_HEIGHT = 12;
    const int START_Y = -10;
    
    for(int i = 0; i < ITEMS_TO_SHOW; ++i)
    {
        int index = _current_index - CENTER_OFFSET + i;
        
        // Wrap around
        while(index < 0) index += audio_viewer_defs::music_count;
        while(index >= audio_viewer_defs::music_count) index -= audio_viewer_defs::music_count;
        
        const char* name = audio_viewer_defs::entries[index].name;
        int y_pos = START_Y + (i - CENTER_OFFSET) * LINE_HEIGHT;
        
        if(i == CENTER_OFFSET)
        {
            // Highlight current selection
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
    
    // Status
    bn::string<32> status;
    if(_is_paused)
    {
        status = "PAUSED";
    }
    else if(_is_playing)
    {
        status = "PLAYING";
    }
    else
    {
        status = "STOPPED";
    }
    _text_generator.generate(0, 40, status, _text_sprites);
    
    // Controls
    _text_generator.generate(0, 60, "A: Play/Pause  B: Stop", _text_sprites);
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
    else if(audio_viewer_defs::music_count > 0)
    {
        // Only allow music control if one or more available.
        if(bn::keypad::down_pressed())
        {
            // Stop music if moving away from currently playing track
            if(_is_playing && _current_index == _playing_index)
            {
                bn::music::stop();
                _is_playing = false;
                _is_paused = false;
                _playing_index = -1;
            }
            _current_index = (_current_index + 1) % audio_viewer_defs::music_count;
            bn::sound_items::menu_focus.play();
            _update_display();
        }
        else if(bn::keypad::up_pressed())
        {
            // Stop music if moving away from currently playing track
            if(_is_playing && _current_index == _playing_index)
            {
                bn::music::stop();
                _is_playing = false;
                _is_paused = false;
                _playing_index = -1;
            }
            _current_index = (_current_index + audio_viewer_defs::music_count - 1) % audio_viewer_defs::music_count;
            bn::sound_items::menu_focus.play();
            _update_display();
        }
        else if(bn::keypad::a_pressed())
        {
            if(_is_playing && _current_index == _playing_index)
            {
                // Toggle pause/resume
                if(_is_paused)
                {
                    bn::music::resume();
                    _is_paused = false;
                }
                else
                {
                    bn::music::pause();
                    _is_paused = true;
                }
            }
            else
            {
                // Play selected music
                const auto& entry = audio_viewer_defs::entries[_current_index];
                entry.item.play(0.3); // Volume at 0.3
                _is_playing = true;
                _is_paused = false;
                _playing_index = _current_index;
            }
            bn::sound_items::menu_confirm.play();
            _update_display();
        }
        else if(bn::keypad::b_pressed())
        {
            // Stop music
            bn::music::stop();
            _is_playing = false;
            _is_paused = false;
            _playing_index = -1;
            bn::sound_items::menu_confirm.play();
            _update_display();
        }
    }

    return result;
}
