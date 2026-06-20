#ifndef PLAYER_MISSILES_H
#define PLAYER_MISSILES_H

#include "fr_model_3d_item.h"
#include "fr_sprite_3d_item.h"
#include "fr_models_3d.h"
#include "colliders.h"

// - Forward declaration
class base_game_scene;
class player_ship;
class controller;
class enemy_manager;
class base_enemy;


namespace fr::model_3d_items
{

    constexpr const sphere_collider missile_collider_detector[] = {
        sphere_collider(fr::point_3d(0, -20, 0), 60),
        sphere_collider(fr::point_3d(0, -120, 0), 100),
        sphere_collider(fr::point_3d(0, -220, 0), 200),
        sphere_collider(fr::point_3d(0, -320, 0), 200),
        sphere_collider(fr::point_3d(0, -420, 0), 400),

    };

    constexpr size_t missile_collider_detector_count = sizeof(missile_collider_detector) / sizeof(missile_collider_detector[0]);

} // namespace fr::model_3d_items

class missile
{
  public:
    explicit missile(fr::models_3d *models);

    void update(bn::fixed delta_y);
    // int statics_render(const fr::model_3d_item **static_model_items,
    //                    int static_count);

    const bool is_active() const { return _is_active; }
    const fr::point_3d &starting_pos() const { return _starting_pos; }
    const fr::point_3d &end_pos() const { return _end_pos; }
    const bn::fixed lerp() const { return _lerp; }
    const fr::point_3d &position() const { return _position; }

    void activate(base_enemy *target_enemy, const fr::point_3d &starting_pos);
    void deactivate();

  private:
    static constexpr int MISSILE_PURSUE_DURATION = 15;

    bool _is_active = false;
    fr::point_3d _starting_pos;
    fr::point_3d _end_pos;
    fr::point_3d _position;
    bn::fixed _lerp = 0;

    base_enemy *_target_enemy = nullptr;
    fr::models_3d *_models = nullptr;
    fr::sprite_3d_item _sprite_item;
    fr::sprite_3d *_sprite = nullptr;
};

class player_missiles
{
  public:
    player_missiles(base_game_scene *base_scene);

    // Calculates whether missiles are being used, collision and so on.
    void update();

    // Controls missile mesh render as static models (at the end of update)
    int statics_render(const fr::model_3d_item **static_model_items,
                       int static_count);

    bool is_enabled() const { return _enabled; }
    void set_enabled(bool enabled) { _enabled = enabled; }

    bool is_active() const { return _is_active; }
    void set_active(bool active) { _is_active = active; }

    void fire_missiles();

  private:
    base_game_scene *_base_scene;
    controller *_controller;
    player_ship *_player_ship;
    enemy_manager *_enemy_manager;
    fr::models_3d *_models;

    bool _enabled = true;
    bool _is_active = false;

    sphere_collider_set _missile_collider_detector;
    static constexpr int MAX_MISSILES = 4;
    missile _missiles[MAX_MISSILES];

    bn::fixed _last_player_y;

};

#endif
