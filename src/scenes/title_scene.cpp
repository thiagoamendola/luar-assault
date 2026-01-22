#include "title_scene.h"

#include "bn_bg_palettes_actions.h"
#include "bn_colors.h"
#include "bn_config_audio.h"
#include "bn_core.h"
#include "bn_fixed.h"
#include "bn_keypad.h"
#include "bn_log.h"
#include "bn_music.h"
#include "bn_music_items.h"
#include "bn_sound_items.h"
#include "bn_sprite_text_generator.h"

#include "fr_model_3d_item.h"
#include "fr_model_colors.h"

#include "scene_type.h"
#include "test_3d_scene_defs.h"

#include "source_han_sans_jp_sprite_font.h"
#include "common_variable_8x16_sprite_font.h"
#include "common_variable_8x8_sprite_font.h"
#include "common_fixed_8x16_sprite_font.h"
#include "models/player_ship_02.h"
#include "models/shot.h"

#define BN_CFG_AUDIO_MIXING_RATE BN_AUDIO_MIXING_RATE_8_KHZ

// <-- Likely need to move this to somewhere else
int mixing_rate()
{
    switch (BN_CFG_AUDIO_MIXING_RATE)
    {

    case BN_AUDIO_MIXING_RATE_8_KHZ:
        return 8;

    case BN_AUDIO_MIXING_RATE_10_KHZ:
        return 10;

    case BN_AUDIO_MIXING_RATE_13_KHZ:
        return 13;

    case BN_AUDIO_MIXING_RATE_16_KHZ:
        return 16;

    case BN_AUDIO_MIXING_RATE_18_KHZ:
        return 18;

    case BN_AUDIO_MIXING_RATE_21_KHZ:
        return 21;

    case BN_AUDIO_MIXING_RATE_27_KHZ:
        return 27;

    case BN_AUDIO_MIXING_RATE_31_KHZ:
        return 31;

    default:
        BN_ERROR("Invalid maxing rate: ", BN_CFG_AUDIO_MIXING_RATE);
    }
}

title_scene::title_scene() : _prepare_to_leave(false)
{
    _camera.set_position(fr::point_3d(0, 0, 0));
    _camera.set_phi(6000);

    _models.load_colors(fr::model_3d_items::player_ship_02_colors);

    _model =
        &_models.create_dynamic_model(fr::model_3d_items::player_ship_02_full);
    _model->set_position(
        fr::point_3d(15, -110, 10)); // x, y (back/forward), z (down/up)

    // 16000 => 90 degrees
    // _model->set_psi(58000);
    _model->set_phi(16000);

    bn::bg_palettes::set_transparent_color(bn::color(2, 2, 6));

    // Set text
    bn::sprite_text_generator text_generator(source_han_sans_jp_sprite_font);
    bn::sprite_text_generator text_generator2(common::fixed_8x16_sprite_font);
    bn::sprite_text_generator text_generator3(common::variable_8x8_sprite_font);
    text_generator.generate(-100, 58, "Press Start", _text_sprites);
    text_generator2.generate(-100, -52, "LUAR", _text_sprites);
    text_generator2.generate(-100, -41, "ASSAULT", _text_sprites);
    text_generator3.generate(-100, -24, "Demo", _text_sprites);

    // Set music
    // bn::music_items::amayadori2.play(1); // Works flawless (.xm)
    // bn::music_items::title.play(1); // Freezes the game (.xm)
    // bn::music_items::gameplay.play(1); // SLOOOOOW (.xm)
    // bn::music_items::gameplay2.play(1); // Slow, lost some channels (.it)
    // bn::music_items::gameplay3.play(1); // Starts alright and breaks like
    // crazy after. Prob a instrument! (.it, BB)
    // bn::music_items::title2.play(0.3); // WORKS!!! (.it, BB)
    
    bn::music_items::title_v2.play(0.2); // WORKS!!! With lower HH (.it, BB)
    // bn::music_items::gameplay2.play(0.3); 
    // bn::music_items::gameplay2c1.play(0.3); // WORKS!!!

    BN_LOG(mixing_rate());
}

bn::optional<scene_type> title_scene::update()
{
    bn::optional<scene_type> result;

    if (_prepare_to_leave)
    {
        // <-- Can be moved to pre-destroy???
        _bgs_fade_out_action->update();
        _sprites_fade_out_action->update();
        result = _target_scene;
    }
    else if (_bgs_fade_out_action)
    {
        if (!_bgs_fade_out_action->done())
        {
            _bgs_fade_out_action->update();
            _sprites_fade_out_action->update();
            _leave_scene_frame_counter++;
            if (_leave_scene_frame_counter > 20)
            {
                bn::sound_items::mc_test_05.play(1.0); // <-- Test voice playback. Move to cutscene later.
                _leave_scene_frame_counter = -2000; // <-- Very hacky. Remove later.
            }
        }
        else
        {
            _prepare_to_leave = true;
            // Pre-destroy
            _bgs_fade_out_action.emplace(1, 0);
            _sprites_fade_out_action.emplace(1, 0);
            bn::music::stop();
            _text_sprites.clear();
        }
    }
    else if (bn::keypad::start_pressed())
    {
        // Transition to gameplay (TEST_3D)
        _target_scene = scene_type::TEST_3D;
        bn::sound_items::menu_confirm.play(0.4);
        _leave_scene_frame_counter = 0;
        _bgs_fade_out_action.emplace(90, 1);
        _sprites_fade_out_action.emplace(90, 1);
    }
    else if (bn::keypad::select_pressed())
    {
        // Transition to audio viewer
        _target_scene = scene_type::AUDIO_VIEWER;
        bn::sound_items::menu_confirm.play();
        _bgs_fade_out_action.emplace(60, 1);
        _sprites_fade_out_action.emplace(60, 1);
    }
    else if (bn::keypad::any_pressed())
    {
        // <-- Remove when menu is done
        bn::sound_items::menu_focus.play();
    }

    _model->set_phi(_model->phi() + 300); // <-- Magic number

    _models.update(_camera);

    return result;
}

title_scene::~title_scene()
{
    if (_model)
    {
        _models.destroy_dynamic_model(*_model);
        _model = nullptr;
    }
}

