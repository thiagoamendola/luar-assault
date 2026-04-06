#ifndef SCORPION_H
#define SCORPION_H

#include "base_enemy.h"

#include "bn_fixed.h"
#include "bn_optional.h"
#include "bn_sprite_animate_actions.h"

#include "fr_models_3d.h"
#include "fr_point_3d.h"

#include "colliders.h"
#include "controller.h"
#include "explosion_effect.h"
#include "enemy_def.h"
#include "base_game_scene.h"

// - Forward declaration
class enemy_manager;
class base_game_scene;

// - Enums

enum class scorpion_behavior_state {
    APPROACHING,
    ATTACKING
};

// - Constants

namespace fr::model_3d_items
{

constexpr inline bn::color scorpion_alt_colors[] = {
            bn::color(24,0,2),
            bn::color(24,24,24),
};

constexpr const sphere_collider scorpion_colliders[] = {
    sphere_collider(fr::point_3d(0, 0, 0), 10),
};

constexpr size_t scorpion_colliders_count = sizeof(scorpion_colliders) / sizeof(scorpion_colliders[0]);

} // namespace fr::model_3d_items

// - Main class

class scorpion : public base_enemy
{
  public:
    scorpion(fr::point_3d position, fr::models_3d *models,
        controller *controller, enemy_manager *enemy_manager, 
        base_game_scene *base_scene, const scorpion_properties* props = nullptr);

    void destroy() override;

    void update(player_ship* player) override;
    void update_active(player_ship* player);

    int statics_render(const fr::model_3d_item **static_model_items, int static_count) override;

    void kill() override;

    fr::model_3d *get_model()
    {
        return _model;
    };

    void handle_laser_hit() override;

    sphere_collider_set *get_collider() override
    {
        return &_sphere_collider_set;
    }

    const bn::fixed MOVEMENT_SPEED = 2.5;
    const bn::fixed ROTATION_SPEED_IDLE = 900;
    const bn::fixed DIRECTION_CORRECTION_SPEED = 0.035;
    const int DAMAGE_COOLDOWN = 3;
    const int MAX_HEALTH = 3;
    const int TOTAL_EXPLODE_FRAMES = 10;

  private:
    base_game_scene *_base_scene;
    fr::models_3d *_models;
    fr::model_3d *_model;
    controller *_controller;
    enemy_manager *_enemy_manager;

    scorpion_behavior_state _behavior_state = scorpion_behavior_state::APPROACHING;

    const bn::color *_current_palette;

    int _health = MAX_HEALTH;
    int _damage_cooldown = 0;
    int _explode_frames = 0;

    bn::fixed _player_distance = 300; // <-- MAGIC NUMBER
    bn::fixed _initial_angle_theta;
    fr::point_3d _current_movement_vector;

    bn::optional<explosion_effect> _explosion;
    sphere_collider_set _sphere_collider_set;

    const fr::point_3d calculate_target_vector(player_ship* player);
};

#endif // SCORPION_H
