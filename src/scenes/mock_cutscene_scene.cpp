#include "scenes/mock_cutscene_scene.h"

#include "bn_bg_palettes.h"
#include "bn_bg_palettes_actions.h"
#include "bn_keypad.h"
#include "bn_log.h"
#include "bn_sprite_palettes_actions.h"
#include "bn_sound_items.h"

#include "bn_affine_bg_items_earth.h"
#include "bn_regular_bg_items_floor.h"
#include "bn_sprite_items_explosion_cutscene.h"
#include "bn_sprite_items_luar_small.h"
#include "common_variable_8x16_sprite_font.h"
#include "editundo_sprite_font.h"

#include "fr_model_colors.h"
#include "fr_point_3d.h"

#include "models/player_ship_02.h"

mock_cutscene_scene::mock_cutscene_scene() :
    _text_generator(common::variable_8x16_sprite_font),
    _text_generator_2(editundo_sprite_font)
{
    BN_LOG("mock_cutscene_scene: init");

    _text_generator.set_bg_priority(1); // Draw on top of letterbox (bg_priority 2)
    _text_generator.set_center_alignment();

    _text_generator_2.set_bg_priority(1);
    _text_generator_2.set_right_alignment();

    bn::bg_palettes::set_transparent_color(bn::color(2, 2, 6));

    // Camera
    _camera.set_position(fr::point_3d(0, 0, -50));
    // _camera.set_position(fr::point_3d(0, 0, 0));
    // _camera.set_phi(6000);
    // _camera.set_psi(-3000);
    _camera.set_theta(3000);

    // Model
    _models.load_colors(fr::model_3d_items::player_ship_02_colors);
    _model = &_models.create_dynamic_model(fr::model_3d_items::player_ship_02_full);
    _model->set_psi(-16383); // 90 degrees
    _model->set_phi(-8000);

    // Timeline commands
    int take_start_time;
    move_model_cmd *_cmd_move = nullptr;
    rotate_model_combined_cmd *_cmd_rotate = nullptr;
    move_camera_cmd *_cmd_move_camera = nullptr;
    rotate_camera_cmd *_cmd_rotate_camera = nullptr;

    // _cmd_rotate_camera = new rotate_camera_cmd(
    //     _camera,
    //     model_rotation{.phi = 0, .theta = -6000, .psi = 0}, // start
    //     model_rotation{.phi = 0, .theta = 3000, .psi = 0}, // end
    //     0, 90, easing::EASE_OUT);
    // _timeline.add(_cmd_rotate_camera);

    // ----- Take 1 -----
    {
        take_start_time = 0;

        // <-- Move this to its own take lambda start
        _floor_bg.emplace(bn::regular_bg_items::floor.create_bg(0, 0)); // <-- REPLACE
        _floor_bg->set_priority(3);

        _earth_bg.emplace(bn::affine_bg_items::earth.create_bg(-85, 20));
        _earth_bg->set_scale(0.7);
        _earth_bg->set_priority(2);
        _earth_bg->set_wrapping_enabled(false);

        _luar_sprite.emplace(bn::sprite_items::luar_small.create_sprite(75, -20));
        _luar_sprite->set_scale(0.7);
        _timeline.add(new move_sprite_cmd(
            *_luar_sprite, bn::fixed_point(73, -20), take_start_time, 300, easing::LINEAR));

        _explosion_sprite.emplace(bn::sprite_items::explosion_cutscene.create_sprite(5, 0));
        _explosion_sprite->set_blending_enabled(true);
        _explosion_sprite->set_visible(false);
        _explosion_sprite->set_scale(0.4);

        _timeline.add(new sprite_fade_cmd(
            *_explosion_sprite, 0, 1, take_start_time + 0, 15));
        _timeline.add(new sprite_fade_cmd(
            *_explosion_sprite, 1, 0, take_start_time + 15, 50));
        _timeline.add(new move_sprite_cmd(
            *_explosion_sprite, bn::fixed_point(15, -5), take_start_time + 65, 0, easing::LINEAR));
        _timeline.add(new sprite_fade_cmd(
            *_explosion_sprite, 0, 1, take_start_time + 65, 15));
        _timeline.add(new sprite_fade_cmd(
            *_explosion_sprite, 1, 0, take_start_time + 80, 50));
        _timeline.add(new move_sprite_cmd(
            *_explosion_sprite, bn::fixed_point(10, 5), take_start_time + 130, 0, easing::LINEAR));
        _timeline.add(new sprite_fade_cmd(
            *_explosion_sprite, 0, 1, take_start_time + 130, 15));
        _timeline.add(new sprite_fade_cmd(
            *_explosion_sprite, 1, 0, take_start_time + 145, 50));
        _timeline.add(new move_sprite_cmd(
            *_explosion_sprite, bn::fixed_point(20, 0), take_start_time + 195, 0, easing::LINEAR));
        _timeline.add(new sprite_fade_cmd(
            *_explosion_sprite, 0, 1, take_start_time + 195, 15));
        _timeline.add(new sprite_fade_cmd(
            *_explosion_sprite, 1, 0, take_start_time + 210, 50));
        _timeline.add(new move_sprite_cmd(
            *_explosion_sprite, bn::fixed_point(15, 5), take_start_time + 260, 0, easing::LINEAR));
        _timeline.add(new sprite_fade_cmd(
            *_explosion_sprite, 0, 1, take_start_time + 260, 15));
        _timeline.add(new sprite_fade_cmd(
            *_explosion_sprite, 1, 0, take_start_time + 275, 50));

        // <-- Make ship fly by.
    }

    // <-- Maybe do a fade in between takes

    // ----- Take 2 -----
    {
        take_start_time = TAKE_2_START_TIME;

        _cmd_move = new move_model_cmd(
            *_model,
            fr::point_3d(-180, -330, 0), // start
            fr::point_3d(0, -180, 0),     // end
            take_start_time + 50, 70, easing::EASE_OUT);
        _timeline.add(_cmd_move);

        _cmd_rotate = new rotate_model_combined_cmd(
            *_model,
            model_rotation{.phi = -8000, .theta = 0, .psi = -16383},     // start
            model_rotation{.phi = -8000, .theta = 67000, .psi = -16383}, // end
            take_start_time + 130, 30, easing::EASE_IN_OUT);
        _timeline.add(_cmd_rotate);

        _cmd_move = new move_model_cmd(
            *_model,
            fr::point_3d(0, -180, 0),   // start
            fr::point_3d(150, -30, -0), // end
            take_start_time + 170, 50, easing::EASE_IN_OUT_BACK_QUAD);
        _timeline.add(_cmd_move);

        _cmd_rotate_camera = new rotate_camera_cmd(
            _camera,
            model_rotation{.phi = 0, .theta = 3000, .psi = 0}, // start
            model_rotation{.phi = 0, .theta = 2500, .psi = -6000}, // end
            take_start_time + 170, 30, easing::EASE_IN_OUT);
        _timeline.add(_cmd_rotate_camera);

        _cmd_move_camera = new move_camera_cmd(
            _camera,
            _camera.position(),
            fr::point_3d(-50, 0, -35),
            take_start_time + 170, 30, easing::EASE_IN_OUT);
        _timeline.add(_cmd_move_camera);

        _timeline.add(new play_sound_cmd(
            bn::sound_items::mc_test_04, 1, take_start_time + 0));
        _timeline.add(new subtitle_cmd(
            _subtitle_text_sprites, _text_generator,
            "This mission's codename will be", take_start_time + 0, 100));

        _timeline.add(new play_sound_cmd(
            bn::sound_items::mc_test_05, 1, take_start_time + 100));
        _timeline.add(new subtitle_cmd(
            _subtitle_text_sprites, _text_generator,
            "Luar Assault!", take_start_time + 100, 75));

        _timeline.add(new play_sound_cmd(
            bn::sound_items::player_death, 1, take_start_time + 160));
    }

    // Letterbox
    _letterbox.show();

    // Start timeline playback
    _timeline.start();
}

mock_cutscene_scene::~mock_cutscene_scene()
{
    _timeline.clear();
    _subtitle_text_sprites.clear();
    _skip_text_sprites.clear();
}

bn::optional<scene_type> mock_cutscene_scene::update()
{
    // Handle Start press for skip prompt
    if (bn::keypad::start_pressed() && !_bgs_fade_out_action)
    {
        if (_skip_prompt_timer <= 0)
        {
            // First press — show prompt
            _skip_text_sprites.clear();
            _text_generator_2.generate(110, 65, "Press Start to Skip", _skip_text_sprites);
            _skip_prompt_timer = SKIP_PROMPT_DURATION;
        }
        else
        {
            // Second press — trigger skip
            _skip_prompt_timer = 0;
            _skip_triggered = true;
            _skip_text_sprites.clear();
            _bgs_fade_out_action.emplace(20, 1);
            _sprites_fade_out_action.emplace(20, 1);
        }
    }

    // Update skip prompt timer
    if (_skip_prompt_timer > 0)
    {
        _skip_prompt_timer--;
        if (_skip_prompt_timer == 0)
        {
            _skip_text_sprites.clear();
        }
    }

    // Tick the letterbox animation
    _letterbox.update();

    // <-- Make lambdas out of those
    if (_earth_bg.has_value() && _timeline.current_frame() >= TAKE_2_START_TIME)
    {
        _floor_bg.reset();
        _earth_bg.reset();
        _luar_sprite.reset();
        _explosion_sprite.reset();
    }
    else if (!_hyperlight_bg.has_value() && _timeline.current_frame() > TAKE_2_START_TIME)
    {
        _hyperlight_bg.emplace(bn::fixed_point(-4, -.5), 6);
    }

    if (_timeline.is_running())
    {
        _timeline.update();
    }

    // Update fade out if active
    if (!_timeline.is_running() && !_bgs_fade_out_action)
    {
        // Timeline done — begin fade to black
        _bgs_fade_out_action.emplace(30, 1);
        _sprites_fade_out_action.emplace(30, 1);
    }
    else if (_bgs_fade_out_action && _sprites_fade_out_action && !_bgs_fade_out_action->done())
    {
        _bgs_fade_out_action->update();
        _sprites_fade_out_action->update();
    }
    else if ((!_timeline.is_running() || _skip_triggered) && _bgs_fade_out_action && _bgs_fade_out_action->done())
    {
        return scene_type::ALPHA_STAGE_V1;
    }

    // Render 3D scene
    _models.update(_camera);
    if (_hyperlight_bg.has_value())
    {
        _hyperlight_bg->update();
    }

    return bn::nullopt;
}
