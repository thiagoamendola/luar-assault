#include "player_missiles.h"

#include "bn_log.h"
#include "bn_string.h"
#include "bn_vector.h"

#include "base_game_scene.h"
#include "player_ship.h"
#include "controller.h"
#include "enemy_manager.h"
#include "base_enemy.h"

// - Missile

missile::missile(fr::models_3d *models)
    : _models(models), missile_full(missile_vertices, missile_faces, fr::model_3d_items::laser_colors)
{
    _missile_state = missile_state::idle;
}

void missile::launch(base_enemy *target_enemy, const fr::point_3d &starting_pos)
{
    if (!target_enemy)
    {
        // A target is necessary to launch.
        return;
    }

    _target_enemy = target_enemy;
    _missile_state = missile_state::launched;

    _starting_pos = starting_pos;
    _end_pos = _target_enemy->get_position();
    _position = _starting_pos;
    _lerp = 0;
}

void missile::deactivate()
{
    _missile_state = missile_state::idle;
    _target_enemy = nullptr;
    _lerp = 0;
}

void missile::update(bn::fixed delta_y)
{
    if (!is_launched())
    {
        return;
    }

    if (!_target_enemy || _target_enemy->is_killed())
    {
        deactivate();
        return;
    }

    // Update lerp positions.
    _starting_pos.set_y(_starting_pos.y() + delta_y);
    _end_pos = _target_enemy->get_position();

    _lerp += bn::fixed(1).division(MISSILE_PURSUE_DURATION);
    if (_lerp > 1)
    {
        _lerp = 1;
    }

    _position = calculate_lerp_position(_lerp);

    // Check distance to enemy (squared, avoids sqrt).
    fr::point_3d diff = _end_pos - _position;
    bn::fixed dist_sq = diff.x() * diff.x() + diff.y() * diff.y() + diff.z() * diff.z();

    if (_lerp >= 1 || dist_sq < 100) // 100 = 10^2
    {
        // Enemy hit.
        _target_enemy->handle_missile_hit();
        deactivate();
    }
}

fr::point_3d missile::calculate_lerp_position(bn::fixed lerp) const
{
    return _starting_pos + (_end_pos - _starting_pos) * lerp;
}

int missile::statics_render(const fr::model_3d_item **static_model_items, int static_count)
{
    if (!is_launched())
    {
        return static_count;
    }

    constexpr bn::fixed MISSILE_X_OFFSET = 6;
    constexpr bn::fixed MISSILE_LERP_OFFSET = 0.4;
    
    const bn::fixed lerp_trail = bn::max(_lerp - MISSILE_LERP_OFFSET, bn::fixed(0));
    fr::point_3d trail_pos = calculate_lerp_position(lerp_trail);

    // Update vertices.
    missile_vertices[0].reset(_position + fr::point_3d(MISSILE_X_OFFSET, 0, 0));
    missile_vertices[1].reset(_position + fr::point_3d(-MISSILE_X_OFFSET, 0, 0));
    missile_vertices[2].reset(trail_pos);
    missile_vertices[3].reset(_position + fr::point_3d(0, 0, MISSILE_X_OFFSET));
    missile_vertices[4].reset(_position + fr::point_3d(0, 0, -MISSILE_X_OFFSET));
    

    // Update faces.
    missile_faces[0].reset(missile_vertices, fr::vertex_3d(0, 1, 0), 0, 1, 2, 0,
                           7);
    missile_faces[1].reset(missile_vertices, fr::vertex_3d(0, 1, 0), 0, 2, 1, 0,
                           7);
    missile_faces[2].reset(missile_vertices, fr::vertex_3d(0, 1, 0), 2, 3, 4, 0,
                           7);
    missile_faces[3].reset(missile_vertices, fr::vertex_3d(0, 1, 0), 2, 4, 3, 0,
                           7);

    // Add nem mesh as static object
    if (static_count >= fr::constants_3d::max_static_models)
    {
        BN_LOG("Player Missile: reached static model max "
               "limit: " +
               bn::to_string<64>(fr::constants_3d::max_static_models));
        return static_count;
    }
    static_model_items[static_count] = &missile_full;
    return static_count + 1;
}

void missile::lock_target(base_enemy *target)
{
    _target_enemy = target;
    _missile_state = missile_state::pending;
}

// - Player Missiles

player_missiles::player_missiles(base_game_scene *base_scene)
    : _base_scene(base_scene),
      _controller(base_scene->get_controller()),
      _player_ship(base_scene->get_player_ship()),
      _enemy_manager(base_scene->get_enemy_manager()),
      _models(base_scene->get_models()),
      _missile_collider_detector(fr::model_3d_items::missile_collider_detector),
      _missiles{missile(_models), missile(_models), missile(_models), missile(_models)},
      _last_player_y(base_scene->get_player_ship()->get_position().y())
{
    _player_missiles_state = player_missiles_state::charging;
    _missile_collider_detector.set_initial_rotation(0, 0, 16383);
}

void player_missiles::update()
{
    if (!is_active())
    {
        return;
    }

    if (is_charging())
    {
        // On button press: detect targets and queue them for staggered launch
        if (_controller->is_missiles_pressed())
        {
            fire_missiles(); // assigns pending targets to missile slots
        }
    }
    else
    {
        // Update launched missiles.
        bn::fixed current_player_y = _player_ship->get_position().y();
        bn::fixed delta_y = current_player_y - _last_player_y;
        _last_player_y = current_player_y;
        for (int i = 0; i < MAX_MISSILES; i++)
        {
            _missiles[i].update(delta_y);
        }

        if (is_launching())
        {
            // Check for next launch.
            if (_launch_timer <= 0)
            {
                bool has_launched = false;
                bool has_pending = false;
                for (int i = 0; i < MAX_MISSILES; i++)
                {
                    if (_missiles[i].is_pending() && !_missiles[i].is_launched())
                    {
                        if (!has_launched)
                        {
                            // Launch first pending we find.
                            _missiles[i].launch(_missiles[i].get_target(), _player_ship->get_position());
                            _launch_timer = LAUNCH_INTERVAL;
                            has_launched = true;
                        }
                        else
                        {
                            // If more pending, do not change state.
                            has_pending = true;
                        }
                    }
                }

                if (!has_pending)
                {
                    // No more pending missiles to launch.
                    _player_missiles_state = player_missiles_state::launched;
                }
            }
            else
            {
                _launch_timer--;
            }
        }
        else
        {
            // Check if all missiles have finished launching and hitting targets.
            bool all_finished = true;
            for (int i = 0; i < MAX_MISSILES; i++)
            {
                if (_missiles[i].is_launched() || _missiles[i].is_pending())
                {
                    all_finished = false;
                    break;
                }
            }

            if (all_finished)
            {
                // All missiles have finished! Move to initial state.
                _player_missiles_state = player_missiles_state::charging;
            }
        }
    }

    // Update missile collider detector.
    BN_LOG("[update][player_missiles] Update origin");
    _missile_collider_detector.set_origin(_player_ship->get_position()); // <-- MOVE
    fr::model_3d *ship_model = _player_ship->get_model();
    _missile_collider_detector.set_rotation(ship_model->phi(), ship_model->theta(), ship_model->psi()); // <-- MOVE
}

int player_missiles::statics_render(const fr::model_3d_item **static_model_items,
                                    int static_count)
{
    if (!is_active())
    {
        return static_count;
    }

    int current_static_count = static_count;

    // Render missile models into static_model_items
    if (is_launching() || is_launched())
    {
        for (int i = 0; i < MAX_MISSILES; i++)
        {
            if (_missiles[i].is_launched())
            {
                current_static_count = _missiles[i].statics_render(static_model_items, current_static_count);
            }
        }
    }

    // Render detection colliders
    if (_controller->is_collider_display_enabled() && is_charging())
    {
        // BN_LOG("[statics_render][player_missiles] Initial value: " +
        //    bn::to_string<64>(static_count));
        // current_static_count = _missile_collider_detector.debug_collider(
        //     static_model_items, current_static_count);
        // BN_LOG("[statics_render][player_missiles] Final value: " +
        //    bn::to_string<64>(current_static_count));
    }

    return current_static_count;
}

void player_missiles::fire_missiles()
{
    _player_missiles_state = player_missiles_state::launching;

    // Sync detection colliders position and rotation.
    fr::model_3d *ship_model = _player_ship->get_model();
    _missile_collider_detector.set_origin(_player_ship->get_position());
    _missile_collider_detector.set_rotation(ship_model->phi(), ship_model->theta(), ship_model->psi());

    bn::vector<base_enemy *, enemy_manager::MAX_ENEMIES> hit_enemies;
    enemy_slot *enemy_slots = _enemy_manager->get_enemies();
    size_t detector_count = _missile_collider_detector.get_sphere_collider_count();

    // Iterate missile colliders in order and check with all enemies.
    for (int i = 0; i < detector_count; i++)
    {
        for (int e = 0; e < enemy_manager::MAX_ENEMIES; e++)
        {
            enemy_slot &slot = enemy_slots[e];
            if (!slot.used || !slot.ptr || slot.ptr->is_killed())
            {
                continue;
            }

            if (!slot.ptr->get_collider()->colliding_with_single_dynamic(&_missile_collider_detector, i))
            {
                continue;
            }

            // Skip if a previous detector collider already claimed this enemy.
            bool already_hit = false;
            for (base_enemy *seen : hit_enemies)
            {
                if (seen == slot.ptr)
                {
                    already_hit = true;
                    break;
                }
            }
            if (!already_hit && !hit_enemies.full())
            {
                hit_enemies.push_back(slot.ptr);
            }
        }
    }

    // Assign detected targets to missile slots; activation is staggered in update()
    int assigned = 0;
    _launch_timer = 0; // fire first missile immediately on next update tick
    for (int i = 0; i < MAX_MISSILES && assigned < int(hit_enemies.size()); ++i)
    {
        if (!_missiles[i].is_launched() && !_missiles[i].is_pending())
        {
            _missiles[i].lock_target(hit_enemies[assigned]);
            assigned++;
        }
    }

    BN_LOG("[fire_missiles] hit " + bn::to_string<64>(hit_enemies.size()) + " enemies");
    for (base_enemy *enemy : hit_enemies)
    {
        fr::point_3d pos = enemy->get_position();
        BN_LOG("[fire_missiles] " + bn::string<64>(enemy->type_name()) +
               " @ (" + bn::to_string<64>(pos.x()) +
               ", " + bn::to_string<64>(pos.y()) +
               ", " + bn::to_string<64>(pos.z()) + ")");
        // enemy->handle_missile_hit(); // <-- REPLACE BY ACTUAL MISSILES
    }
}
