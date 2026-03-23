#include "scenes/mock_cutscene_scene.h"

#include "bn_bg_palettes.h"
#include "bn_bg_palettes_actions.h"
#include "bn_log.h"
#include "bn_sprite_palettes_actions.h"
#include "bn_sound_items.h"

#include "fr_model_colors.h"
#include "fr_point_3d.h"

#include "models/player_ship_02.h"


mock_cutscene_scene::mock_cutscene_scene()
{
    BN_LOG("mock_cutscene_scene: init");

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
    move_model_cmd *_cmd_move = nullptr;
    rotate_model_combined_cmd *_cmd_rotate = nullptr;

    _cmd_move = new move_model_cmd(
        *_model,
        fr::point_3d(-30, -210, -60), // start
        fr::point_3d(0, -180, 0),  // end
        0, 50, easing::EASE_OUT);
    _timeline.add(_cmd_move);

    _cmd_rotate = new rotate_model_combined_cmd(
        *_model,
        model_rotation{.phi = -8000, .theta = 0, .psi = -16383}, // start
        model_rotation{.phi = -8000, .theta = 67000, .psi = -16383}, // end
        60, 30, easing::EASE_IN_OUT);
    _timeline.add(_cmd_rotate);

    _cmd_move = new move_model_cmd(
        *_model,
        fr::point_3d(0, -180, 0), // start
        fr::point_3d(150, -30, -0),  // end
        100, 30, easing::EASE_IN);
    _timeline.add(_cmd_move);

    _timeline.add(new play_sound_cmd(
        bn::sound_items::player_death, 1, 95));

    // Letterbox
    _letterbox.show();

    // Start timeline playback
    _timeline.start();
}

mock_cutscene_scene::~mock_cutscene_scene()
{
    _timeline.clear();
}

bn::optional<scene_type> mock_cutscene_scene::update()
{
    // Tick the letterbox animation
    _letterbox.update();

    if (_timeline.is_running())
    {
        _timeline.update();
    }
    else if (!_bgs_fade_out_action)
    {
        // Timeline done — begin fade to black
        _bgs_fade_out_action.emplace(45, 1);
        _sprites_fade_out_action.emplace(45, 1);
    }
    else if (!_bgs_fade_out_action->done())
    {
        _bgs_fade_out_action->update();
        _sprites_fade_out_action->update();
    }
    else
    {
        BN_LOG("mock_cutscene_scene: done, returning to TITLE");
        return scene_type::ALPHA_STAGE_V1;
    }

    // Render 3D scene
    _models.update(_camera);

    return bn::nullopt;
}
