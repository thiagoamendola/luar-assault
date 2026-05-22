#ifndef HUD_MANAGER_H
#define HUD_MANAGER_H

#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_actions.h"
#include "bn_blending_actions.h"
#include "bn_point.h"
#include "bn_fixed.h"
#include "bn_optional.h"
#include "bn_vector.h"

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

    void hide();
    void show();

    void fade_in();
    void fade_out();

    // Neutral stick threshold
    const bn::fixed DIRECTION_DEADZONE = 0.15;

    // Target location constraints
    const int TARGET_SPEED = 10;
    const int TARGET_MIN_X = -110;           
    const int TARGET_MAX_X = 110;
    const int TARGET_MIN_Y = -75;            
    const int TARGET_MAX_Y = 75;

    // Target animation constants
    const bn::fixed TARGET_INITIAL_SCALE = 1.3;
    const int TARGET_GROWTH_STEPS = 15;
    const bn::fixed TARGET_GROWTH_MAX_SCALE = 1.5;

    // Fade constants
    const int FADE_FRAMES = 30;

private:
    base_game_scene *_base_scene;
    controller *_controller; // <-- move to common stuff
    fr::camera_3d *_camera;
    player_ship *_player_ship;
    bool _is_hidden = false;

    // HUD
    bn::sprite_text_generator _text_generator; // <-- move to common stuff?
    bn::vector<bn::sprite_ptr, 32> _text_sprites;
    bn::sprite_ptr _lifebar_frame;
    bn::vector<bn::sprite_ptr, 20> _lifebar_tiles;
    int _displayed_health = -1;

    static constexpr int LIFEBAR_MAX_TILES = 20;
    static constexpr int LIFEBAR_START_X = 5;
    static constexpr int LIFEBAR_START_Y = 5;
    static constexpr int LIFEBAR_TILE_SPACING = 3;
    static constexpr int LIFEBAR_TILE_WIDTH = 8;
    static constexpr int LIFEBAR_RED_GRAPHICS_INDEX = 1;
    static constexpr int LIFEBAR_DAMAGE_HOLD_FRAMES = 30;
    static constexpr int LIFEBAR_DAMAGE_SHRINK_FRAMES = 15;

    struct lifebar_damage_tile
    {
        bn::sprite_ptr spr;
        bn::fixed center_x;
    };
    bn::vector<lifebar_damage_tile, LIFEBAR_MAX_TILES> _lifebar_damage_tiles;
    int _damage_hold_frames = 0;
    int _damage_shrink_frames = 0;
    int _damage_shrink_per_tile = 0;

    // Target animated sprite
    bn::sprite_ptr _target_spr;
    bn::optional<bn::sprite_scale_loop_action> _target_growth_action;

    // Fade actions
    bn::optional<bn::blending_transparency_alpha_to_action> _fade_in_action;
    bn::optional<bn::blending_transparency_alpha_to_action> _fade_out_action;
    bool _is_blending_active = false;

    void _update_lifebar();
    void _update_lifebar_damage_tiles();

    // Target calculation
    void _move_target();
    bn::fixed_point _compute_target_return();
    bn::fixed_point _compute_target_move(const bn::fixed_point& dir_input);

};

#endif
