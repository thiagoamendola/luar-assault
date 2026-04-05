#include "scorpion.h"

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_log.h"
#include "bn_math.h"
#include "bn_random.h"
#include "bn_sound_items.h"
#include "bn_string.h"
#include "bn_sprite_animate_actions.h"
#include "bn_profiler.h"

#include "fr_point_3d.h"

#include "player_laser.h"
#include "player_ship.h"
#include "explosion_effect.h"
#include "enemy_manager.h"
#include "base_game_scene.h"
#include "utils.h"

#include "bn_sprite_items_explosion1.h"
#include "bn_sprite_items_boom.h"
#include "models/scorpion.h"

scorpion::scorpion(fr::point_3d position, fr::point_3d movement, fr::models_3d *models,
                   controller *controller, enemy_manager *enemy_manager,
                   base_game_scene *base_scene, const scorpion_properties *props)
    : _movement(movement), _models(models), _controller(controller), _enemy_manager(enemy_manager),
      _base_scene(base_scene),
      _sphere_collider_set(fr::model_3d_items::scorpion_colliders)
{
    if (props)
    {
        _player_distance = props->player_distance;
    }

    _models->get_color_mapping()->log_debug();

    _position = position;
    _current_palette = fr::model_3d_items::scorpion_alt_colors;
    _model =
        &_models->create_dynamic_model(fr::model_3d_items::scorpion_full,
                                       _current_palette);
    _model->set_position(position);
    // _model->set_palette(fr::model_3d_items::scorpion_alt_colors);
    _state = enemy_state::ACTIVE;

    // Random theta tilts the local Z axis; psi will spin around it each frame
    bn::random random;
    _initial_angle_theta = random.get_int(65536);
    _model->set_theta(_initial_angle_theta);
}

void scorpion::destroy()
{
    if (_state == enemy_state::DESTROYED)
    {
        return; // already destroyed
    }

    // Remove scorpion model.
    if (_model)
    {
        _models->destroy_dynamic_model(*_model);
        _model = nullptr;
    }

    // Remove explosion effect.
    if (_explosion)
    {
        _explosion->destroy();
        _explosion.reset();
    }

    _state = enemy_state::DESTROYED;
}

void scorpion::update(player_ship *player)
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
        else if (_explosion)
        {
            // Update the explosion effect
            _explosion->update();
        }

        break;

    default:
        break;
    }
}

void scorpion::update_active(player_ship *player)
{
    // Handle laser hit cooldown.
    if (_damage_cooldown > 0)
    {
        _damage_cooldown--;
        if (_damage_cooldown <= 0)
        {
            _model->set_palette(_current_palette);
        }
    }

    switch (_behavior_state)
    {
    case scorpion_behavior_state::APPROACHING:
    {

        // Spin around local Z (cylinder body axis), tilted by theta
        _model->set_psi(_model->psi() + 900);

        // Transition to ATTACKING state based on distance to player
        const bn::fixed distance_to_player_y = bn::abs(player->get_position().y() - _model->position().y());
        if (distance_to_player_y < _player_distance)
        {
            _behavior_state = scorpion_behavior_state::ATTACKING;
            BN_LOG("[scorpion] Proximity reached, switching to ATTACKING state.");
        }

        break;
    }

    case scorpion_behavior_state::ATTACKING:
    {
        // <-- Pursue player

        BN_PROFILER_START("scorpion_attack");

        const auto player_pos = player->get_position();
        const auto distance_to_player = player_pos - _model->position();
        const auto unit_direction = unit_vector(distance_to_player);

        auto movement_vector = unit_direction * MOVEMENT_SPEED;
        
        // Add the player movement so scorpion intercepts it.
        movement_vector.set_y(movement_vector.y() - player_ship::FORWARD_SPEED); // <-- If speed varies, we'll maybe need to update this

        // Ensure scorpion doesn't attack from behind the player.
        if (movement_vector.y() < 0)
        {
            movement_vector.set_y(0);
        }

        _model->set_position(_model->position() + movement_vector);

        BN_PROFILER_STOP();
        // OG approach: 26-27 ticks

        // <-- Rotate scorpion so it points to player

        break;
    }

    default:
        break;
    }

    // Update colliders.
    _sphere_collider_set.set_origin(get_model()->position());
}

int scorpion::statics_render(const fr::model_3d_item **static_model_items,
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

void scorpion::handle_laser_hit()
{
    if (_state != enemy_state::ACTIVE)
    {
        return;
    }

    // Apply hit feedback palette and cooldown only if still alive after hit
    if (_health > 0)
    {
        _health -= 1;
        if (_health <= 0)
        {
            // Final destruction
            kill();
            return;
        }
        _model->set_palette(fr::model_3d_items::laser_colors);
        _damage_cooldown = DAMAGE_COOLDOWN;
    }
    // bn::sound_items::scorpion_hit.play(); // <-- Get scorpion hit sound
}

void scorpion::kill()
{
    if (_state != enemy_state::ACTIVE)
    {
        return;
    }

    _state = enemy_state::DESTROYING;
    _explode_frames = TOTAL_EXPLODE_FRAMES;
    _base_scene->increment_score(20); // <-- MAGIC NUMBERS

    // Create explosion effect
    _explosion.emplace(_position, _models);

    // Remove scorpion model
    _models->destroy_dynamic_model(*_model);
    _model = nullptr;
}
