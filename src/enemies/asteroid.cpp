#include "asteroid.h"

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

#include "bn_sprite_items_explosion1.h"
#include "bn_sprite_items_boom.h"
#include "models/asteroid1.h"


asteroid::asteroid(fr::point_3d position, fr::point_3d movement, fr::models_3d *models, controller *controller)
    : _position(position), _movement(movement), _models(models), _controller(controller),
      _sphere_collider_set(fr::model_3d_items::asteroid_colliders)
{
    _model =
        &_models->create_dynamic_model(fr::model_3d_items::asteroid1_full);
    _model->set_position(position);
    _model->set_palette(fr::model_3d_items::asteroid1_colors);
    _state = enemy_state::ACTIVE;
}

void asteroid::destroy()
{
    if(_state == enemy_state::DESTROYED)
    {
        return; // already destroyed
    }

    // Remove asteroid model.
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

void asteroid::update(player_ship* player)
{
    switch (_state)
    {
    case enemy_state::ACTIVE:
        // Handle laser hit cooldown.
        if (_damage_cooldown > 0)
        {
           _damage_cooldown--;
            if (_damage_cooldown <= 0) {
                _model->set_palette(fr::model_3d_items::asteroid1_colors);
            }
        }   
    
        // Rotate.
        _model->set_phi(_model->phi() + 600); // <-- Magic number
    
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

int asteroid::statics_render(const fr::model_3d_item **static_model_items,
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

void asteroid::handle_laser_hit()
{
    if(_state != enemy_state::ACTIVE)
    {
        return;
    }

    // Apply hit feedback palette and cooldown only if still alive after hit
    if(_health > 0)
    {
        _health -= 1;
        if(_health <= 0)
        {
            // Final destruction
            kill();
            return;
        }
        _model->set_palette(fr::model_3d_items::laser_colors);
        _damage_cooldown = DAMAGE_COOLDOWN;
    }
    // bn::sound_items::asteroid_hit.play(); // <-- Get asteroid hit sound
}

void asteroid::kill()
{
    if(_state != enemy_state::ACTIVE)
    {
        return;
    }

    _state = enemy_state::DESTROYING;
    _explode_frames = TOTAL_EXPLODE_FRAMES;

    // Create explosion effect
    _explosion.emplace(_position, _models);

    // Remove asteroid model
    _models->destroy_dynamic_model(*_model);
    _model = nullptr;
}
