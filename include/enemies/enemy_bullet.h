#ifndef ENEMY_BULLET_H
#define ENEMY_BULLET_H

#include "base_enemy.h"

#include "bn_fixed.h"
#include "bn_optional.h"
#include "bn_sprite_animate_actions.h"

#include "fr_models_3d.h"

#include "colliders.h"
#include "controller.h"
#include "explosion_effect.h"
#include "player_ship.h"

// - Constants

namespace fr::model_3d_items
{

constexpr inline bn::color enemy_bullet_hit_colors[] = {
    bn::color(18, 0, 0),
};

constexpr const sphere_collider enemy_bullet_colliders[] = {
    sphere_collider(fr::point_3d(0, 0, 0), 15)
};

constexpr size_t enemy_bullet_colliders_count = sizeof(enemy_bullet_colliders) / sizeof(enemy_bullet_colliders[0]);

} // namespace fr::model_3d_items

// - Main class

class enemy_bullet : public base_enemy
{
  public:
    enemy_bullet(fr::point_3d position, fr::point_3d target, fr::models_3d *models, controller *controller);

    void destroy() override;

    void update(player_ship* player) override;

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

    const bn::fixed MOVEMENT_SPEED = 5;
    const bn::fixed ROTATION_ANIM_SPEED = 600;

  private:
    fr::point_3d _movement;

    fr::models_3d *_models;
    fr::model_3d *_model;
    controller *_controller;

    sphere_collider_set _sphere_collider_set;

    bn::fixed angle_phi;
};

#endif
