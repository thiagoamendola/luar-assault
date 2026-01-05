#ifndef HUD_MANAGER_H
#define HUD_MANAGER_H

#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_actions.h"
#include "bn_point.h"
#include "bn_fixed.h"

#include "fr_camera_3d.h"
#include "fr_models_3d.h"

#include "controller.h"
#include "player_ship.h"

// - Forward declaration
class base_game_scene;

class hud_manager
{
public:
    hud_manager(base_game_scene *base_scene);

    void destroy();

    void update(fr::models_3d *models); 
    void statics_update(int static_count);

    void hide_game_hud();

    // Neutral stick threshold
    const bn::fixed DIRECTION_DEADZONE = 0.15;

    // Target location constraints
    const int TARGET_SPEED = 10;
    const int TARGET_MIN_X = -110;           
    const int TARGET_MAX_X = 110;
    const int TARGET_MIN_Y = -75;            
    const int TARGET_MAX_Y = 75;

    // Target animation constants
    const bn::fixed TARGET_INITIAL_SCALE = 1.2;
    const int TARGET_GROWTH_STEPS = 15;
    const bn::fixed TARGET_GROWTH_MAX_SCALE = 2.0;

private:
    base_game_scene *_base_scene;
    controller *_controller; // <-- move to common stuff
    fr::camera_3d *_camera;
    player_ship *_player_ship;

    // UI
    bn::sprite_text_generator _text_generator; // <-- move to common stuff?
    bn::vector<bn::sprite_ptr, 32> _text_sprites;

    // Target animated sprite
    // bn::sprite_animate_action<4> _target_action;
    bn::sprite_ptr _target_spr;
    bn::optional<bn::sprite_scale_loop_action> _target_growth_action;

    void _move_target();
    bn::fixed_point _compute_target_return();
    bn::fixed_point _compute_target_move(const bn::fixed_point& dir_input);

};

#endif
