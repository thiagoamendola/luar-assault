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

enum class missile_state
{
  idle,
  pending,
  launched
};

enum class player_missiles_state
{
  disabled,
  charging,
  launching,
  launched
};

// - Missile

class missile
{
  public:
    explicit missile(fr::models_3d *models);

    void update(bn::fixed delta_y);
    int statics_render(const fr::model_3d_item **static_model_items,
                       int static_count);

    bool is_launched() const { return _missile_state == missile_state::launched; }
    bool is_pending() const { return _missile_state == missile_state::pending; }

    const fr::point_3d &starting_pos() const { return _starting_pos; }
    const fr::point_3d &end_pos() const { return _end_pos; }
    const bn::fixed lerp() const { return _lerp; }
    const fr::point_3d &position() const { return _position; }

    void lock_target(base_enemy *target);

    base_enemy *get_target() const { return _target_enemy; }

    void launch(base_enemy *target_enemy, const fr::point_3d &starting_pos);
    void deactivate(); // <-- RENAME?

  private:
    static constexpr int MISSILE_PURSUE_DURATION = 15;

    fr::point_3d calculate_lerp_position(bn::fixed lerp) const;

    missile_state _missile_state = missile_state::idle;
    fr::point_3d _starting_pos;
    fr::point_3d _end_pos;
    fr::point_3d _position;
    bn::fixed _lerp = 0;

    fr::model_3d_item missile_full;
    base_enemy *_target_enemy = nullptr;
    fr::models_3d *_models = nullptr;

    // Static render variables

    fr::vertex_3d missile_vertices[5] = {
        fr::vertex_3d(1, 0, 0), fr::vertex_3d(0, 1, 0), fr::vertex_3d(0, 0, 1),
        fr::vertex_3d(1, 1, 0), fr::vertex_3d(0, 1, 1),
    };
    fr::face_3d missile_faces[4] = {
        fr::face_3d(missile_vertices, fr::vertex_3d(0, 1, 0), 0, 1, 2, 0, 7),
        fr::face_3d(missile_vertices, fr::vertex_3d(0, 1, 0), 0, 2, 1, 0, 7),
        fr::face_3d(missile_vertices, fr::vertex_3d(0, 1, 0), 2, 3, 4, 0, 7),
        fr::face_3d(missile_vertices, fr::vertex_3d(0, 1, 0), 2, 4, 3, 0, 7),
    };
};

// - Player Missiles

class player_missiles
{
  public:
    player_missiles(base_game_scene *base_scene);

    // Calculates whether missiles are being used, collision and so on.
    void update();

    // Controls missile mesh render as static models (at the end of update)
    int statics_render(const fr::model_3d_item **static_model_items,
                       int static_count);

    bool is_active() const { return _player_missiles_state != player_missiles_state::disabled; }
    bool is_charging() const { return _player_missiles_state == player_missiles_state::charging; }
    bool is_launching() const { return _player_missiles_state == player_missiles_state::launching; }
    bool is_launched() const { return _player_missiles_state == player_missiles_state::launched; }

    void fire_missiles();

  private:
    base_game_scene *_base_scene;
    controller *_controller;
    player_ship *_player_ship;
    enemy_manager *_enemy_manager;
    fr::models_3d *_models;

    bool _enabled = true;
    player_missiles_state _player_missiles_state = player_missiles_state::disabled;

    sphere_collider_set _missile_collider_detector;
    static constexpr int MAX_MISSILES = 4;
    static constexpr int LAUNCH_INTERVAL = 3; // Frames between successive missile launches
    missile _missiles[MAX_MISSILES];

    // Staggered launch state
    int _launch_timer = 0;

    bn::fixed _last_player_y;

};

#endif
