#ifndef ASTEROID_H
#define ASTEROID_H

#include "bn_fixed.h"

#include "fr_models_3d.h"
#include "fr_sprite_3d_item.h"

#include "colliders.h"
#include "controller.h"

// - Constants

namespace fr::model_3d_items
{

constexpr inline bn::color asteroid_hit_colors[] = {
    bn::color(18, 0, 0),
};

constexpr const sphere_collider asteroid_colliders[] = {
    sphere_collider(fr::point_3d(3, 0, 5), 15)
    
};

constexpr size_t asteroid_colliders_count = sizeof(asteroid_colliders) / sizeof(asteroid_colliders[0]);

} // namespace fr::model_3d_items

// - Main class

class asteroid
{
  public:
    asteroid(fr::point_3d position, fr::point_3d movement, fr::models_3d *models, controller *controller);
    
    void destroy();

    void update(); // <-- receive player?

    int statics_render(const fr::model_3d_item **static_model_items,
      int static_count);

    void handle_laser_hit();

    bool is_destroyed() const { return _destroyed; }

    fr::model_3d *get_model()
    {
      return _model;
    };

    sphere_collider_set<fr::model_3d_items::asteroid_colliders_count> *get_collider()
    {
      return &_sphere_collider_set;
    }

    const bn::fixed MOVEMENT_SPEED = 3;
    const bn::fixed ROTATION_SPEED = 2.5;
    const int DAMAGE_COOLDOWN = 3;
    const int MAX_HEALTH = 3;

  private:
    fr::point_3d _position;
    fr::point_3d _movement;

    fr::models_3d *_models;
    fr::model_3d *_model;
    controller *_controller;

    int _damage_cooldown = 0;
    int _health = MAX_HEALTH;
    bool _destroyed = false;
    int _crash_frames = 0;
    fr::sprite_3d_item _explosion_sprite_3d_item;
    fr::sprite_3d* _explosion_sprite = nullptr;


    sphere_collider_set<fr::model_3d_items::asteroid_colliders_count>
        _sphere_collider_set;

};


#endif
