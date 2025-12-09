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

// - Constants

namespace fr::model_3d_items
{

constexpr inline bn::color oyster_hit_colors[] = {
    bn::color(18, 0, 0),
};

constexpr const sphere_collider oyster_colliders[] = {
    sphere_collider(fr::point_3d(0, 0, 0), 12)
};

constexpr size_t oyster_colliders_count = sizeof(oyster_colliders) / sizeof(oyster_colliders[0]);

} // namespace fr::model_3d_items

// - Main class

class oyster : public base_enemy
{
  public:
    oyster(fr::point_3d position, fr::point_3d movement, fr::models_3d *models, controller *controller);

    void destroy() override;

    void update() override;

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
    const int MAX_HEALTH = 2;
    const int TOTAL_CRASH_FRAMES = 10;

  private:
    fr::point_3d _position;
    fr::point_3d _movement;

    fr::models_3d *_models;
    fr::model_3d *_model;
    controller *_controller;

    int _damage_cooldown = 0;
    int _health = MAX_HEALTH;
    int _crash_frames = 0;

    bn::optional<explosion_effect> _explosion;

    sphere_collider_set _sphere_collider_set;
};

#endif
