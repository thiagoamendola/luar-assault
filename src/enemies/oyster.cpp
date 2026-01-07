#include "oyster.h"

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_log.h"
#include "bn_math.h"
#include "bn_sound_items.h"
#include "bn_string.h"
#include "bn_sprite_animate_actions.h"

#include "fr_point_3d.h"

#include "player_laser.h"
#include "explosion_effect.h"
#include "enemy_manager.h"
#include "base_game_scene.h"

#include "bn_sprite_items_explosion1.h"
#include "bn_sprite_items_boom.h"
#include "models/moon_oyster.h"

// <-- use base_game_scene to get managers
oyster::oyster(fr::point_3d position, fr::point_3d movement, fr::models_3d *models,
        controller *controller, enemy_manager *enemy_manager, 
        base_game_scene *base_scene, const oyster_properties* props)
    : _movement(movement), _models(models), _controller(controller), _enemy_manager(enemy_manager),
      _base_scene(base_scene),
      _sphere_collider_set(fr::model_3d_items::oyster_colliders)
{
    if(props)
    {
        _player_distance = props->player_distance;
    }
    
    _position = position;
    _model =
        &_models->create_dynamic_model(fr::model_3d_items::moon_oyster_full);
    _model->set_position(position);
    _model->set_palette(fr::model_3d_items::moon_oyster_colors);
    _state = enemy_state::ACTIVE;
}

void oyster::destroy()
{
    if(_state == enemy_state::DESTROYED)
    {
        return; // already destroyed
    }

    // Remove oyster model.
    if(_model)
    {
        _models->destroy_dynamic_model(*_model);
        _model = nullptr;
    }
    
    // Remove explosion effect.
    if(_explosion)
    {
        _explosion->destroy();
        _explosion.reset();
    }

    _state = enemy_state::DESTROYED;
}

void oyster::update(player_ship* player)
{
    switch (_state)
    {
    case enemy_state::ACTIVE:
        update_active(player);
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

void oyster::update_active(player_ship* player)
{
    // Handle laser hit cooldown.

    if (_damage_cooldown > 0)
    {
        _damage_cooldown--;
        if (_damage_cooldown <= 0) {
            _model->set_palette(fr::model_3d_items::moon_oyster_colors);
        }
    }   

    switch (_behavior_state)
    {
    case oyster_behavior_state::APPROACHING:
    {
        // Move
        _position.set_y(_position.y() + MOVEMENT_SPEED);
        _model->set_position(_position);

        // Transition to ATTACKING state based on distance to player (and save Y location)
        const bn::fixed distance_to_player_y = bn::abs(player->get_position().y() - _model->position().y());
        
        if (distance_to_player_y < _player_distance)
        {
            _behavior_state = oyster_behavior_state::ATTACKING;
            _initial_attacking_distance = player->get_position().y();
            _bullet_cooldown = INITIAL_BULLET_COOLDOWN;
            BN_LOG("[oyster] Proximity reached, switching to ATTACKING state.");
        }

        break;
    }
    
    case oyster_behavior_state::ATTACKING:
    {
        // Maintain distance from player
        _position.set_y(player->get_position().y() - _player_distance);
        _model->set_position(_position);

        // Handle attack by shooting projectile
        if (_bullet_cooldown > 0)
        {
            _bullet_cooldown--;
        }
        else
        {
            // Fire bullet
            fr::point_3d bullet_position = _model->position();
            bullet_position.set_y(bullet_position.y() + 10); // Slightly in front
            _enemy_manager->create_bullet(bullet_position, player->get_position());

            BN_LOG("[oyster] Fired bullet at y=" + bn::to_string<64>(int(bullet_position.y())));

            _bullet_cooldown = BULLET_COOLDOWN;
        }

        // Transition to FLEEING state based on Y distance (decrease = forward)
        if (player->get_position().y() < _initial_attacking_distance - FLEEING_THRESHOLD)
        {
            _behavior_state = oyster_behavior_state::FLEEING;
            BN_LOG("[oyster] Threshold reached, switching to FLEEING state.");
        }
        break;
    }
    
    case oyster_behavior_state::FLEEING:
    {
        // <-- Move away from center
        // Continue moving forward
        _position.set_y(_position.y() + MOVEMENT_SPEED);
        _model->set_position(_position);

        // <-- Call destroy when out of bounds

        break;
    }
    default:
        break;
    }

    // Rotate.
    _model->set_phi(_model->phi() + 400); // <-- Magic number

    // Update colliders.
    _sphere_collider_set.set_origin(get_model()->position());

}

int oyster::statics_render(const fr::model_3d_item **static_model_items,
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

void oyster::handle_laser_hit()
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
    // bn::sound_items::oyster_hit.play(); // <-- Get oyster hit sound
}

void oyster::kill()
{
    if(_state != enemy_state::ACTIVE)
    {
        return;
    }

    _state = enemy_state::DESTROYING;
    _explode_frames = TOTAL_EXPLODE_FRAMES;
    _base_scene->increment_score(30); // <-- MAGIC NUMBERS

    // Create explosion effect
    _explosion.emplace(_position, _models);

    // Remove oyster model
    _models->destroy_dynamic_model(*_model);
    _model = nullptr;
}
