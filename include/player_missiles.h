#ifndef PLAYER_MISSILES_H
#define PLAYER_MISSILES_H

#include "fr_model_3d_item.h"

// - Forward declaration
class base_game_scene;
class player_ship;
class controller;
class enemy_manager;

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

  private:
    base_game_scene *_base_scene;
    controller *_controller;
    player_ship *_player_ship;
    enemy_manager *_enemy_manager;

    bool _enabled = true;
};

#endif
