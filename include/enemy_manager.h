#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H

#include "fr_models_3d.h"
#include "fr_constants_3d.h"

#include "controller.h"
#include "asteroid.h"
#include "base_enemy.h"
#include "stage_section.h"
#include "colliders.h"
#include "player_ship.h"


// <-- Change to generic enemy
struct enemy_slot {
  bool used = false;
  base_enemy *ptr = nullptr; // dynamically created via models
  const enemy_def *source = nullptr; // descriptor origin
  // <-- I might need optional fields for more complex enemies
};

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

  enemy_slot* get_enemies()
  {
    return _enemies;
  }

  // <-- Definitely needs to revamp this shortly!
  static constexpr int MAX_ENEMIES = fr::constants_3d::max_dynamic_models - 1;

private:
  // <-- Change to generic enemy
  enemy_slot _enemies[MAX_ENEMIES];

  fr::models_3d *_models;
  controller *_controller;
  player_ship* _player;

  bn::fixed _last_section_start_y = bn::fixed(32767);
  bn::fixed _last_section_end_y = bn::fixed(32767);
    
};

#endif
