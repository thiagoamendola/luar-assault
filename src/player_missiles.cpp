#include "player_missiles.h"

#include "bn_log.h"
#include "bn_string.h"
#include "bn_vector.h"
#include "bn_sprite_items_boom.h"

#include "base_game_scene.h"
#include "player_ship.h"
#include "controller.h"
#include "enemy_manager.h"
#include "base_enemy.h"

// - Missile

missile::missile(fr::models_3d *models)
    : _models(models),
      _sprite_item(bn::sprite_items::boom, 0) // <-- REPLACE OR REMOVE
{
}

void missile::activate(base_enemy *target_enemy, const fr::point_3d &starting_pos)
{
    if (!target_enemy)
    {
        deactivate();
        return;
    }

    _target_enemy = target_enemy;
    _starting_pos = starting_pos;
    _end_pos = _target_enemy->get_position();
    _position = _starting_pos;
    _lerp = 0;
    _is_active = true;

    if (!_sprite)
    {
        _sprite = &_models->create_sprite(_sprite_item);
    }

    _sprite->set_position(_position);
}

void missile::deactivate()
{
    _is_active = false;
    _target_enemy = nullptr;
    _lerp = 0;

    if (_sprite)
    {
        _models->destroy_sprite(*_sprite);
        _sprite = nullptr;
    }
}

void missile::update(bn::fixed delta_y)
{
    if (!_is_active)
    {
        return;
    }

    if (!_target_enemy)
    {
        deactivate();
        return;
    }

    if (_target_enemy->is_killed())
    {
        deactivate();
        return;
    }

    _starting_pos.set_y(_starting_pos.y() + delta_y);
    _end_pos = _target_enemy->get_position();

    _lerp += bn::fixed(1).division(MISSILE_PURSUE_DURATION);
    if (_lerp > 1)
    {
        _lerp = 1;
    }

    _position = _starting_pos + (_end_pos - _starting_pos) * _lerp;

    if (_sprite)
    {
        _sprite->set_position(_position);
    }

    // Check distance to enemy (squared, avoids sqrt).
    fr::point_3d diff = _end_pos - _position;
    bn::fixed dist_sq = diff.x() * diff.x() + diff.y() * diff.y() + diff.z() * diff.z();

    if (_lerp >= 1 || dist_sq < 100) // 100 = 10^2
    {
        _target_enemy->handle_missile_hit();
        deactivate();
    }
}

// - Player Missiles

player_missiles::player_missiles(base_game_scene *base_scene)
    : _base_scene(base_scene),
      _controller(base_scene->get_controller()),
      _player_ship(base_scene->get_player_ship()),
      _enemy_manager(base_scene->get_enemy_manager()),
      _models(base_scene->get_models()),
      _missile_collider_detector(fr::model_3d_items::missile_collider_detector),
      _missiles{ missile(_models), missile(_models), missile(_models), missile(_models) },
      _last_player_y(base_scene->get_player_ship()->get_position().y())
{
    _is_active = false;
    _missile_collider_detector.set_initial_rotation(0, 0, 16383);
}

void player_missiles::update()
{
    if (!_enabled)
    {
        return;
    }

    // Update missiles
    // <-- Only if missiles are active
    bn::fixed current_player_y = _player_ship->get_position().y();
    bn::fixed delta_y = current_player_y - _last_player_y;
    _last_player_y = current_player_y;
    for (int i = 0; i < MAX_MISSILES; ++i)
    {
        _missiles[i].update(delta_y);
    }

    // Check if it should start missiles launch
    // if (!_is_active){
        if (_controller->is_missiles_pressed())
        {
            _is_active = true;
            fire_missiles();
        }
        else
        {
            _is_active = false; // <-- REMOVE
            return;
        }
    // }

    // Update missiles
    BN_LOG("[update][player_missiles] Update origin");
    _missile_collider_detector.set_origin(_player_ship->get_position()); // <-- MOVE
    fr::model_3d *ship_model = _player_ship->get_model();
    _missile_collider_detector.set_rotation(ship_model->phi(), ship_model->theta(), ship_model->psi()); // <-- MOVE
    
}

int player_missiles::statics_render(const fr::model_3d_item **static_model_items,
                                    int static_count)
{
    if (!_enabled)
    {
        return static_count;
    }

    int current_static_count = static_count;

    // TODO: render missile models into static_model_items
    if (_controller->is_collider_display_enabled() && _is_active)
    {
        BN_LOG("[statics_render][player_missiles] Initial value: " +
           bn::to_string<64>(static_count));
        current_static_count = _missile_collider_detector.debug_collider(
            static_model_items, current_static_count);
        BN_LOG("[statics_render][player_missiles] Final value: " +
           bn::to_string<64>(current_static_count));
    }

    return current_static_count;
}

void player_missiles::fire_missiles()
{
    // Sync detection colliders position and rotation.
    fr::model_3d *ship_model = _player_ship->get_model();
    _missile_collider_detector.set_origin(_player_ship->get_position());
    _missile_collider_detector.set_rotation(ship_model->phi(), ship_model->theta(), ship_model->psi());

    bn::vector<base_enemy*, enemy_manager::MAX_ENEMIES> hit_enemies;
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

    const int missile_count = bn::min(int(hit_enemies.size()), MAX_MISSILES);
    for (int i = 0; i < missile_count; ++i)
    {
        _missiles[i].activate(hit_enemies[i], _player_ship->get_position());
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
