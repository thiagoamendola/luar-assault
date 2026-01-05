#include "hud_manager.h"

#include "bn_log.h"
#include "bn_math.h"
#include "bn_memory.h" // <-- Only import when debugging
#include "bn_sprite_actions.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"

#include "fr_camera_3d.h"
#include "fr_point_3d.h"

#include "controller.h"
#include "player_ship.h"
#include "base_game_scene.h"

#include "bn_sprite_items_target_ui.h"
#include "common_variable_8x16_sprite_font.h"
#include "common_variable_8x8_sprite_font.h"

hud_manager::hud_manager(base_game_scene *base_scene)
    : _base_scene(base_scene), _controller(base_scene->get_controller()), 
      _camera(base_scene->get_camera()), _player_ship(base_scene->get_player_ship()),
      _text_generator(common::variable_8x8_sprite_font), 
      _target_spr(bn::sprite_items::target_ui.create_sprite(0, 0)),
      _target_growth_action()
{
    //common::variable_8x16_sprite_font
    // Setup target sprite
    _target_spr.set_horizontal_scale(TARGET_INITIAL_SCALE);
    _target_spr.set_vertical_scale(TARGET_INITIAL_SCALE);
    _target_growth_action = bn::sprite_scale_loop_action(_target_spr, TARGET_GROWTH_STEPS, TARGET_GROWTH_MAX_SCALE);
}

void hud_manager::destroy()
{
    _target_growth_action.reset();
}

void hud_manager::update(fr::models_3d *models)
{
    // Clear texts.
    _text_sprites.clear();

    // text generators should be created only once // <-- what to do with this?

    // Display location Y debug text.
    if (_controller->is_debug_text_enabled())
    {
        _text_generator.generate(-7 * 16, -72, "Location (Y): " + bn::to_string<64>(int(_camera->position().y())),
                                 _text_sprites);
        // _text_generator.generate(-7 * 16, -60,
        //                          "Dynamic Objs: " + bn::to_string<64>(models->dynamic_models_count()) + "/" +
        //                              bn::to_string<64>(models->dynamic_models_max_count()),
        //                          _text_sprites);
        _text_generator.generate(-7 * 16, -60, "Player (Y): " + bn::to_string<64>(int(_player_ship->get_position().y())),
                                 _text_sprites);
    }
    else
    {
        _text_generator.generate(-7 * 16, -72, "HEALTH: " + bn::to_string<64>(_player_ship->get_health()),
                                 _text_sprites);
    }

    _move_target();
}



void hud_manager::statics_update(int static_count)
{
    // Display static count debug text.
    if (_controller->is_debug_text_enabled())
    {
        // _text_generator.generate(-7 * 16, -60, "Static Objs: " + bn::to_string<64>(static_count), _text_sprites);
        // _text_generator.generate(-7 * 16, -48, "IWRAM stack: " + bn::to_string<64>(bn::memory::used_stack_iwram()),
        // _text_sprites); _text_generator.generate(-7 * 16, -36, "IWRAM static: " +
        // bn::to_string<64>(bn::memory::used_static_iwram()), _text_sprites); _text_generator.generate(-7 * 16, -24,
        // "EWRAM: " + bn::to_string<64>(bn::memory::used_static_ewram()), _text_sprites);
    }
}

void hud_manager::_move_target()
{
    // Get raw input vector
    bn::fixed_point dir_input = _controller->get_smooth_directional();

    // BN_LOG("[target] dir_input: " + bn::to_string<64>(dir_input.x()) + ", " + bn::to_string<64>(dir_input.y()));

    bn::fixed_point target_pos;

    if (bn::abs(dir_input.x()) < DIRECTION_DEADZONE && bn::abs(dir_input.y()) < DIRECTION_DEADZONE)
    {
        target_pos = _compute_target_return();
    }
    else
    {
        target_pos = _compute_target_move(dir_input);
    }

    _target_spr.set_y(target_pos.y());
    _target_spr.set_x(target_pos.x());

    _player_ship->set_target_position(target_pos);

    _target_growth_action->update();
}

void hud_manager::hide_game_hud()
{
    _text_sprites.clear();
}
