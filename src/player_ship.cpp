#include "player_ship.h"

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_keypad.h"
#include "bn_log.h"
#include "bn_math.h"
#include "bn_sound_items.h"
#include "bn_string.h"
#include "bn_profiler.h"
#include "fr_point_3d.h"
#include "fr_constants_3d.h"
#include "fr_div_lut.h"
#include "fr_sin_cos.h"

#include "base_game_scene.h"
#include "enemy_manager.h"
#include "controller.h"
#include "utils.h"
#include "easing.h"

#include "models/shot.h"
// #include "models/player_ship_01.h"
#include "models/player_ship_02.h"

player_ship::player_ship(base_game_scene *base_scene)
    : _base_scene(base_scene), _controller(base_scene->get_controller()),
      _camera(base_scene->get_camera()), _models(base_scene->get_models()),
      _sphere_collider_set(fr::model_3d_items::ship_colliders),
      _player_laser(this, _controller)
{
    _model =
        &_models->create_dynamic_model(fr::model_3d_items::player_ship_02_full);
    // x, y (back/forward), z (down/up)
    _model->set_psi(16383); // 90 degrees // <-- Magic number

    _dodge_timeout = 0;
    _dodge_progress = 0;
    _is_dodging = false;
}

void player_ship::destroy()
{
    _models->destroy_dynamic_model(*_model);
}

void player_ship::update()
{
    bn::fixed_point dir_input = _controller->get_smooth_directional();

    {
        // - Player ship movement

        fr::point_3d ship_pos = _model->position();

        // Directional movement

        const bn::fixed dodge_boost_x = _is_dodging ? (
            DODGE_MOVEMENT_BOOST_X // Dodge will work for any side
            // _dodge_speed < 0 ? DODGE_MOVEMENT_BOOST_X : -DODGE_MOVEMENT_BOOST_X // if button choice matters
        ) : 0; 
        const bn::fixed dodge_boost_y = _is_dodging ?
             DODGE_MOVEMENT_BOOST_Y : 0; // Does not care about dodge button choice. 
        
        ship_pos.set_x(ship_pos.x()
             + dir_input.x() * MANEUVER_SPEED
             + dir_input.x() * dodge_boost_x);
        ship_pos.set_z(ship_pos.z()
             + dir_input.y() * MANEUVER_SPEED
             + dir_input.y() * dodge_boost_y);

        if (ship_pos.x() < -65) // <-- Magic number
        {
            ship_pos.set_x(-65);
        }
        else if (ship_pos.x() > 65)
        {
            ship_pos.set_x(65);
        }
        if (ship_pos.z() < -45) // <-- Magic number
        {
            ship_pos.set_z(-45);
        }
        else if (ship_pos.z() > 45)
        {
            ship_pos.set_z(45);
        }

        // Forward movement
        ship_pos.set_y(ship_pos.y() - FORWARD_SPEED);

        _model->set_position(ship_pos);
    }

    bn::fixed dodge_rotation = 0; // <-- MOVE

    {
        // - Dodge
        if (!_is_dodging && _dodge_timeout <= 0)
        {
            // Player is dodge ready. Check for player input.
            if (_controller->is_dodge_right_button_pressed())
            {
                _dodge_timeout = DODGE_DURATION;
                _is_dodging = true;
                _dodge_rotation_direction = -1;
                _dodge_progress = 0;
            }
            else if (_controller->is_dodge_left_button_pressed())
            {
                _dodge_timeout = DODGE_DURATION;
                _is_dodging = true;
                _dodge_rotation_direction = 1;
                _dodge_progress = 0;
            }
        }
        else if (_is_dodging)
        {
            // Player is currently dodging. Continue dodge movement.
            _dodge_progress += DODGE_STEPS;
            dodge_rotation = _dodge_rotation_direction * 65532
                 * apply_easing(_dodge_progress, easing::EASE_CUSTOM_DODGE);
            // _model->set_theta(dodge_rotation);
            _dodge_timeout--;

            if (_dodge_timeout <= 0)
            {
                // Dodge ended
                _is_dodging = false;
                // _model->set_theta(0); // Reset to default orientation
                dodge_rotation = 0;
                _dodge_timeout = DODGE_COOLDOWN; // Start cooldown
            }
        }
        else if (_dodge_timeout > 0)
        {
            // Player is in dodge cooldown. Decrease cooldown timer.
            _dodge_timeout--;
        }
        // <-- Add more visual feedback for dodge???
        // <-- Add sound for dodge
        // <-- Add invincibility frames to dodge??? Or maybe remove some hitboxes temporarily?
    }

    {
        // - Player ship Yaw/Pitch

        // <-- Move this to its own method
        // > Point ship to target position
        constexpr int focal_length_shift = fr::constants_3d::focal_length_shift;
        bn::fixed depth_position = _model->position().y() - FOCUS_DISTANCE; // Setup distance in front of ship
        bn::fixed depth_to_camera = _camera->position().y() - depth_position;

        fr::point_3d cam_pos = _camera->position();
        fr::point_3d cam_u = _camera->u();
        fr::point_3d cam_v = _camera->v();
        // cam_w is identity. How this affects things? (0,0,1)

        // Build direction vector in camera space
        bn::fixed dir_x = cam_u.x() * target_position.x() + cam_v.x() * target_position.y();
        bn::fixed dir_z = cam_u.z() * target_position.x() + cam_v.z() * target_position.y();

        // Compute target world position. Dividing using focal shift for optimization.
        fr::point_3d target_world_pos(
            int((dir_x - cam_pos.x()) * depth_to_camera) >> focal_length_shift,
            depth_position,
            -int((dir_z - cam_pos.z()) * depth_to_camera) >> focal_length_shift);

        // BN_LOG("[player_ship] target_world_pos position: " + bn::to_string<64>(target_world_pos.x()) + ", " + bn::to_string<64>(target_world_pos.y()) + ", " + bn::to_string<64>(target_world_pos.z()));

        // Calculate direction vector from ship to target
        bn::fixed dx = target_world_pos.x() - _model->position().x();
        bn::fixed dy = target_world_pos.y() - _model->position().y();
        bn::fixed dz = target_world_pos.z() - _model->position().z();

        // BN_LOG("[player_ship] angles dx: " + bn::to_string<64>(dx) + ", dy: " + bn::to_string<64>(dy) + ", dz: " + bn::to_string<64>(dz));

        // Yaw (phi): rotate around Z axis, atan2(dx, -dy) to account for inverted Y
        bn::fixed angle_phi_degrees = bn::degrees_atan2(dx.integer(), -dy.integer());
        // Pitch (psi): tilt up/down, atan2(-dz, sqrt(dx^2 + dy^2))
        bn::fixed horizontal_dist = bn::sqrt(dx * dx + dy * dy); // <-- Can this be optimized?
        bn::fixed angle_psi_degrees = bn::degrees_atan2(-dz.integer(), horizontal_dist.integer());

        // BN_LOG("[player_ship] angles_degrees psi: " + bn::to_string<64>(angle_psi_degrees) + ", phi " + bn::to_string<64>(angle_phi_degrees));

        bn::rule_of_three_approximation rotation_units(360, 65536); // <-- Reutilize this for entire project
        bn::fixed angle_psi = rotation_units.calculate(angle_psi_degrees);
        bn::fixed angle_phi = rotation_units.calculate(angle_phi_degrees);

        // Store true aiming angles for laser to use (unaffected by dodge)
        _aim_phi = angle_phi;
        _aim_psi = 16383 + angle_psi; // Include the 90-degree base offset

        const bn::fixed rotation_cos = fr::cos(dodge_rotation.right_shift_integer());
        angle_psi *= rotation_cos;
        _model->set_theta(0); // Reset pitch before applying dodge rotation
        _model->set_psi(0);                 // Avoid gimbal lock
        _model->set_phi(angle_phi);         // Yaw (around Z)
        _model->set_psi(16383 + angle_psi); // Pitch (centered) // <-- Magic number
        _model->set_theta(dodge_rotation);
        // BN_LOG("[player_ship] angles psi: " + bn::to_string<64>(angle_psi) + ", phi " + bn::to_string<64>(angle_phi));
    }

    {
        // - Camera movement

        fr::point_3d camera_pos = _camera->position();
        camera_pos.set_y(camera_pos.y() - FORWARD_SPEED);
        _camera->set_position(camera_pos);
    }

    {
        // - Sound test // <-- Remove later
        // if (bn::keypad::b_pressed())
        // {
        //     bn::sound_items::player_damage.play();
        // }
        // else if (bn::keypad::r_pressed())
        // {
        //     bn::sound_items::player_death.play();
        // }
        // else if (bn::keypad::l_pressed())
        // {
        //     bn::sound_items::enemy_death.play();
        // }
    }

    {
        // Update general controller input.
        _controller->update();

        // Update colliders.
        _sphere_collider_set.set_origin(get_model()->position());
    }
}

void player_ship::collision_update(const fr::model_3d_item **static_model_items,
                                   size_t static_items_count, enemy_manager &enemies)
{
    {
        // - Player Laser
        _player_laser.update(enemies);
    }

    {
        // Player ship collision

        if (_damage_cooldown > 0)
        {
            _damage_cooldown--;
            // Blink ship
            if (_damage_cooldown % 6 < 3)
            { // <-- MAGIC NUMBER
                _model->set_palette(fr::model_3d_items::hurt_colors);
            }
            else
            {
                _model->set_palette(fr::model_3d_items::player_ship_02_colors);
            }
            return;
        }

        // - Collision with statics
        if (_sphere_collider_set.colliding_with_statics(static_model_items, static_items_count))
        {
            take_damage();
            // return;
        }
        // - Collision with dynamic enemies
        else if (check_collision_with_enemies(enemies))
        {
            take_damage();
        }
        else
        {
            _model->set_palette(fr::model_3d_items::player_ship_02_colors);
        }
    }
}

void player_ship::take_damage()
{
    bn::sound_items::player_damage.play();
    _damage_cooldown = DAMAGE_COOLDOWN;
    health--;
    _base_scene->set_hit_stop(HIT_STOP_COOLDOWN);
    _model->set_palette(fr::model_3d_items::hurt_colors);
    if (health <= 0)
    {
        _base_scene->get_game_over_manager()->show();
    }
}

int player_ship::statics_render(const fr::model_3d_item **static_model_items,
                                int static_count)
{
#if LOG_STATIC_RENDER_STEPS
    BN_LOG("[statics_render][player_ship] Initial value: " +
           bn::to_string<64>(static_count));
#endif

    // Render laser model
    int current_static_count = _player_laser.render_player_laser(
        static_model_items, static_count);

#if LOG_STATIC_RENDER_STEPS
    BN_LOG("[statics_render][player_ship] Updated laser: " +
           bn::to_string<64>(current_static_count));
#endif

    // Render debug collider models
    if (_controller->is_collider_display_enabled())
    {
        current_static_count = _sphere_collider_set.debug_collider(
            static_model_items, current_static_count);
    }

#if LOG_STATIC_RENDER_STEPS
    BN_LOG("[statics_render][player_ship] Updated debug collider: " +
           bn::to_string<64>(current_static_count));
#endif

    return current_static_count;
}

bool player_ship::check_collision_with_enemies(enemy_manager &enemies)
{
    auto &player_collider = collider_set();
    enemy_slot *enemy_slots = enemies.get_enemies();
    for (int i = 0; i < enemy_manager::MAX_ENEMIES; ++i)
    {
        if (enemy_slots[i].used && enemy_slots[i].ptr)
        {
            if (!enemy_slots[i].ptr->is_killed() && player_collider.colliding_with_dynamic(enemy_slots[i].ptr->get_collider()))
            {
                return true;
            }
        }
    }
    return false;
}
