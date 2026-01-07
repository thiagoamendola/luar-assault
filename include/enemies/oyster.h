#ifndef OYSTER_H
#define OYSTER_H

#include "base_enemy.h"

#include "bn_fixed.h"
#include "bn_optional.h"
#include "bn_sprite_animate_actions.h"

#include "fr_models_3d.h"

#include "colliders.h"
#include "controller.h"
#include "explosion_effect.h"
#include "enemy_def.h"
#include "base_game_scene.h"

// - Forward declaration
class enemy_manager;
class base_game_scene;

// - Enums

enum class oyster_behavior_state {
    APPROACHING,
    ATTACKING,
    FLEEING
};

// - Constants

namespace fr::model_3d_items
{

constexpr inline bn::color oyster_hit_colors[] = {
    bn::color(18, 0, 0),
    bn::color(18, 0, 0),
};

constexpr const sphere_collider oyster_colliders[] = {
    sphere_collider(fr::point_3d(0, 0, 0), 12),
};

constexpr size_t oyster_colliders_count = sizeof(oyster_colliders) / sizeof(oyster_colliders[0]);

} // namespace fr::model_3d_items

// - Main class

class oyster : public base_enemy
{
  public:
    oyster(fr::point_3d position, fr::point_3d movement, fr::models_3d *models,
        controller *controller, enemy_manager *enemy_manager, 
        base_game_scene *base_scene, const oyster_properties* props = nullptr);

    void destroy() override;

    void update(player_ship* player) override;
    void update_active(player_ship* player);

    int statics_render(const fr::model_3d_item **static_model_items, int static_count) override;

    void kill() override;

    fr::model_3d *get_model()
    {
        return _model;
    };

    void handle_laser_hit();

    sphere_collider_set *get_collider() override
    {
        return &_sphere_collider_set;
    }

    const bn::fixed MOVEMENT_SPEED = 2.5;
    const bn::fixed ROTATION_SPEED = 1.5;
    const int DAMAGE_COOLDOWN = 3;
    const int MAX_HEALTH = 6;
    const int TOTAL_EXPLODE_FRAMES = 10;
    const bn::fixed FLEEING_THRESHOLD = 700;

    const int INITIAL_BULLET_COOLDOWN = 30;
    const int BULLET_COOLDOWN = 120;

  private:
    fr::point_3d _movement;

    base_game_scene *_base_scene;
    fr::models_3d *_models;
    fr::model_3d *_model;
    controller *_controller;
    enemy_manager *_enemy_manager;

    oyster_behavior_state _behavior_state = oyster_behavior_state::APPROACHING;

    int _health = MAX_HEALTH;
    int _damage_cooldown = 0;
    int _explode_frames = 0;
    bn::optional<explosion_effect> _explosion;

    int _bullet_cooldown = 0;
    
    sphere_collider_set _sphere_collider_set;

    
    // Properties
    bn::fixed _player_distance = 0;

    bn::fixed _initial_attacking_distance = 0;

};

#endif
