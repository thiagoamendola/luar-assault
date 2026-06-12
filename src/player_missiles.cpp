#include "player_missiles.h"

#include "base_game_scene.h"
#include "player_ship.h"
#include "controller.h"
#include "enemy_manager.h"

player_missiles::player_missiles(base_game_scene *base_scene)
    : _base_scene(base_scene),
      _controller(base_scene->get_controller()),
      _player_ship(base_scene->get_player_ship()),
      _enemy_manager(base_scene->get_enemy_manager()),
      _missile_collider_detector(fr::model_3d_items::missile_collider_detector)
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

    // Check if it should start missiles launch
    // if (!_is_active){
        if (_controller->is_missiles_pressed())
        {
            _is_active = true;
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
