#include "enemy_bullet.h"

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_log.h"
#include "bn_math.h"
#include "bn_sound_items.h"
#include "bn_string.h"
#include "bn_sprite_animate_actions.h"

#include "fr_point_3d.h"

#include "player_laser.h"
#include "player_ship.h"
#include "explosion_effect.h"
#include "base_enemy.h"

#include "bn_sprite_items_explosion1.h"
#include "bn_sprite_items_boom.h"
#include "models/shot.h"


enemy_bullet::enemy_bullet(fr::point_3d position, fr::point_3d target, fr::models_3d *models, controller *controller)
    : _models(models), _controller(controller),
      _sphere_collider_set(fr::model_3d_items::enemy_bullet_colliders)
{
    _position = position;
    
    // Calculate direction vector from origin to target
    fr::point_3d distance_target = fr::point_3d(
        target.x() - position.x(),
        target.y() - position.y(),
        target.z() - position.z()
    );

    bn::fixed distance_mag = bn::sqrt(distance_target.x() * distance_target.x() + distance_target.y() * distance_target.y() + distance_target.z() * distance_target.z()); // <-- Can be optimized to avoid sqrt?
    
    // Normalize to unit vector and multiply by movement speed
    if (distance_mag > 0)
    {
        // <-- Optimize to avoid divisions? It's only in the constructor though.
        const bn::fixed test_x = distance_target.x().division(distance_mag);
        _movement = fr::point_3d(
            (distance_target.x().division(distance_mag)) * MOVEMENT_SPEED,
            (distance_target.y().division(distance_mag)) * MOVEMENT_SPEED,
            (distance_target.z().division(distance_mag)) * MOVEMENT_SPEED
        );
    }
    else
    {
        _movement = fr::point_3d(MOVEMENT_SPEED, 0, 0);
    }
    
    // Add the ship movement to ensure bullet intercepts it.
    _movement.set_y(_movement.y() - player_ship::FORWARD_SPEED); // <-- If speed varies, we'll maybe need to update this.

    // Calculate shot rotation towards target
    bn::fixed angle_phi_degrees = bn::degrees_atan2(distance_target.x().integer(), -distance_target.y().integer());

    bn::rule_of_three_approximation rotation_units(360, 65536); // <-- Reutilize this for entire project
    angle_phi = rotation_units.calculate(angle_phi_degrees);

    // Update model
    _model = &_models->create_dynamic_model(fr::model_3d_items::shot_full);
    _model->set_position(position);
    // _model->set_theta(16383); // 90 degrees // <-- Magic number
    _model->set_palette(fr::model_3d_items::shot_colors);

    _state = enemy_state::ACTIVE;
}

void enemy_bullet::destroy()
{
    if(_state == enemy_state::DESTROYED)
    {
        return; // already destroyed
    }

    // Remove bullet model.
    if(_model)
    {
        _models->destroy_dynamic_model(*_model);
        _model = nullptr;
    }
    
    // Remove explosion effect.
    if(_explosion)
    {
        _explosion.reset();
    }

    _state = enemy_state::DESTROYED;
}

void enemy_bullet::update(player_ship* player)
{
    switch (_state)
    {
    case enemy_state::ACTIVE:
        // Handle laser hit cooldown.
        if (_damage_cooldown > 0)
        {
           _damage_cooldown--;
            if (_damage_cooldown <= 0) {
                _model->set_palette(fr::model_3d_items::shot_colors);
            }
        }
        
        // Update position using movement vector
        _position = fr::point_3d(
            _position.x() + _movement.x(),
            _position.y() + _movement.y(),
            _position.z() + _movement.z()
        );
        _model->set_position(_position);
    
        // Rotate.
        _model->set_phi(0); // Avoid gimbal lock
        _model->set_psi(_model->psi() + 600); // Bullet twisting// <-- Magic number
        _model->set_phi(-16383 + angle_phi);  // Model front + towards target // <-- Magic number

        // Update colliders.
        _sphere_collider_set.set_origin(get_model()->position());

        break;

    case enemy_state::DESTROYING:
        // Handle destruction animation.
        _explode_frames -= 1;

        if (_explode_frames <= 0)
        {
            destroy();
        }
        else if(_explosion)
        {
            // Update the explosion effect
            _explosion->update();
        }

        break;

    default:
        break;
    }

}

int enemy_bullet::statics_render(const fr::model_3d_item **static_model_items,
                       int static_count)
{
    int current_static_count = static_count;

    // Render debug collider models
    if (_controller->is_collider_display_enabled())
    {
        current_static_count = _sphere_collider_set.debug_collider(
            static_model_items, current_static_count);
    }

    return current_static_count;
}

void enemy_bullet::handle_laser_hit()
{
    if(_state != enemy_state::ACTIVE)
    {
        return;
    }

    // Apply hit feedback palette and cooldown only if still alive after hit
    if(_health > 0)
    {
        // <-- MUST NOT HELATH HEALTH
        _health -= 1;
        if(_health <= 0)
        {
            // Final destruction
            // kill();
            return;
        }
        // _model->set_palette(fr::model_3d_items::laser_colors);
        _damage_cooldown = DAMAGE_COOLDOWN;
    }
    // bn::sound_items::bullet_hit.play(); // <-- Get bullet hit sound
}

void enemy_bullet::kill()
{
    if(_state != enemy_state::ACTIVE)
    {
        return;
    }

    // <-- MUST NOT BE KILLED WITH EXPLOSION

    _state = enemy_state::DESTROYING;
    _explode_frames = TOTAL_EXPLODE_FRAMES;

    // Create explosion effect
    _explosion.emplace(_position, _models);

    // Remove bullet model
    _models->destroy_dynamic_model(*_model);
    _model = nullptr;
}
