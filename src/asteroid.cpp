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

#include "bn_sprite_items_explosion1.h"
#include "models/asteroid1.h"

// <-- Implement definition from header


asteroid::asteroid(fr::point_3d position, fr::point_3d movement, fr::models_3d *models, controller *controller)
    : _position(position), _movement(movement), _models(models), _controller(controller),
      _sphere_collider_set(fr::model_3d_items::asteroid_colliders), 
      _explosion_sprite_3d_item(bn::sprite_items::explosion1, 0)
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
    // Remove explosion sprite.
    if(_explosion_sprite)
    {
        _models->destroy_sprite(*_explosion_sprite);
        _explosion_sprite = nullptr;
    }
    // Remove action.
    if (_explosion_sprite_action)
    {
        _explosion_sprite_action.reset();
    }
    _state = enemy_state::DESTROYED;
}

void asteroid::update()
{
    switch (_state)
    {
    case enemy_state::ACTIVE:
        // Handle cooldown.

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

        _crash_frames -= 1;

        if (_crash_frames <= 0)
        {
            destroy();
        }
        else
        {
            if (_explosion_sprite_action)
            {
                _explosion_sprite_action->update();
            }

            // Update explosion sprite scale and fade
            // bn::fixed fade_intensity = bn::fixed(_crash_frames).division(bn::fixed(TOTAL_CRASH_FRAMES));
            // _explosion_sprite->set_scale(bn::fixed(1) + ((bn::fixed(1) - fade_intensity).unsafe_multiplication(bn::fixed(0.6))));
            // _explosion_sprite->set_alpha(fade_intensity);

            // const bn::sprite_tiles_item& tiles_item = bn::sprite_items::explosion1.tiles_item();
            // bn::sprite_tiles_ptr& explosion_tiles = _explosion_sprite_3d_item.tiles();
            // int explosion_graphics_index = (_crash_frames) % 8; // <-- DO SOMETHING SMARTER HERE //((fade_crash_frames - crash_frames) * 9) / fade_crash_frames;
            // BN_LOG("Asteroid crash frames: " + bn::to_string<64>(_crash_frames) + ", explosion graphics index: " + bn::to_string<64>(explosion_graphics_index));
            // explosion_tiles.set_tiles_ref(tiles_item, explosion_graphics_index);
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

    // Final destruction

    _crash_frames = TOTAL_CRASH_FRAMES;

    // Set explosion vfx
    _explosion_sprite = &_models->create_sprite(_explosion_sprite_3d_item);
    _explosion_sprite->set_position(_position); // + fr::model_3d_items::asteroid_colliders[0].position);
    _explosion_sprite->set_theta(40000);
    _explosion_sprite->set_scale(1.5); // + ((1 - fade_intensity).unsafe_multiplication(bn::fixed(0.6))));
    // <-- DOESN'T ACCEPT sprite_3d_item AS PARAMETER!!! I need to find a way around it
    // <-- CENTRALIZE ASTEROID MODEL IN BLENDER 
    // _explosion_sprite_action = bn::create_sprite_animate_action_forever(
    //     _explosion_sprite, 64, bn::sprite_items::explosion1.tiles_item(), 0, 1, 2, 3, 4, 5, 6, 7);

    // Remove asteroid model
    _models->destroy_dynamic_model(*_model);
    _model = nullptr;
}
