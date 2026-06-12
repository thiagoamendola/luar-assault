#ifndef PLAYER_MISSILES_H
#define PLAYER_MISSILES_H

#include "fr_model_3d_item.h"
#include "colliders.h"

// - Forward declaration
class base_game_scene;
class player_ship;
class controller;
class enemy_manager;


namespace fr::model_3d_items
{

    constexpr const sphere_collider missile_collider_detector[] = {
        sphere_collider(fr::point_3d(0, -20, 0), 60),
        sphere_collider(fr::point_3d(0, -90, 0), 50),
        sphere_collider(fr::point_3d(0, -160, 0), 50),
        sphere_collider(fr::point_3d(0, -230, 0), 50),
        sphere_collider(fr::point_3d(0, -300, 0), 50),

    };

    constexpr size_t missile_collider_detector_count = sizeof(missile_collider_detector) / sizeof(missile_collider_detector[0]);

} // namespace fr::model_3d_items

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

  private:
    base_game_scene *_base_scene;
    controller *_controller;
    player_ship *_player_ship;
    enemy_manager *_enemy_manager;

    bool _enabled = true;
    bool _is_active = false;

    sphere_collider_set _missile_collider_detector;

};

#endif
