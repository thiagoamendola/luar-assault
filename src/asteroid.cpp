#include "asteroid.h"

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_log.h"
#include "bn_math.h"
#include "bn_sound_items.h"
#include "bn_string.h"

#include "fr_point_3d.h"

#include "player_laser.h"

#include "models/asteroid1.h"

// <-- Implement definition from header

asteroid::asteroid(fr::point_3d position, fr::point_3d movement, fr::models_3d *models, controller *controller)
    : _position(position), _movement(movement), _models(models), _controller(controller),
      _sphere_collider_set(fr::model_3d_items::asteroid_colliders)
{
    _model =
        &_models->create_dynamic_model(fr::model_3d_items::asteroid1_full);
    _model->set_position(position);
    _model->set_palette(fr::model_3d_items::asteroid1_colors);

}

void asteroid::destroy()
{
    _models->destroy_dynamic_model(*_model);
}

void asteroid::update()
{
    // Handle cooldown.
    if (damage_cooldown > 0)
    {
       damage_cooldown--;
        if (damage_cooldown <= 0) {
            _model->set_palette(fr::model_3d_items::asteroid1_colors);
        }
    }   

    // Rotate.
    _model->set_phi(_model->phi() + 600); // <-- Magic number

    // Update colliders.
    _sphere_collider_set.set_origin(get_model()->position());
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

// damage_cooldown = DAMAGE_COOLDOWN;
void asteroid::handle_laser_hit()
{
    _model->set_palette(fr::model_3d_items::laser_colors);
    damage_cooldown = DAMAGE_COOLDOWN;
    // bn::sound_items::asteroid_hit.play(); // <-- Get asteroid hit sound
    // <-- Lower health or destroy asteroid
}
