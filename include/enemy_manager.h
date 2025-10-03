#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H

#include "fr_models_3d.h"

#include "controller.h"
#include "asteroid.h"
#include "stage_section.h"
#include "colliders.h"
#include "player_ship.h"

class enemy_manager
{
public:
  enemy_manager(fr::models_3d *models, controller *controller, player_ship* player);

  void destroy();

  void update(); 

  int statics_render(const fr::model_3d_item **static_model_items,
    int static_count);

  // Basic section enemy processing: given current camera Y and all sections, print enemies in active sections.
  void process_section_enemies(stage_section_list_ptr sections, size_t sections_count, bn::fixed camera_y);

  static constexpr int max_asteroids = 12;

  struct asteroid_slot {
    bool used = false;
    asteroid *ptr = nullptr; // dynamically created via models
    const enemy_descriptor *source = nullptr; // descriptor origin
  };

  // Returns true if player collides with any active enemy collider
  bool check_collision();

private:
  asteroid_slot _asteroids[max_asteroids];

  fr::models_3d *_models;
  controller *_controller;
  player_ship* _player;

  bn::fixed _last_section_start_y = bn::fixed(32767);
    
};

#endif
