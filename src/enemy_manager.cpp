#include "enemy_manager.h"

#include "bn_fixed.h"
#include "bn_log.h"
#include "bn_string.h"

#include "fr_models_3d.h"

#include "controller.h"
#include "stage_section.h"
#include "player_ship.h"

enemy_manager::enemy_manager(fr::models_3d *models, controller *controller, player_ship* player)
    : _models(models), _controller(controller), _player(player)
{
}

void enemy_manager::destroy()
{
    for(int i = 0; i < max_asteroids; ++i)
    {
        if(_asteroids[i].used && _asteroids[i].ptr)
        {
            _asteroids[i].ptr->destroy();
            _asteroids[i].used = false;
            _asteroids[i].ptr = nullptr;
            _asteroids[i].source = nullptr;
        }
    }
}

void enemy_manager::update()
{
    for(int i = 0; i < max_asteroids; ++i)
    {
        if(_asteroids[i].used && _asteroids[i].ptr)
        {
            _asteroids[i].ptr->update();
        }
    }
}

int enemy_manager::statics_render(const fr::model_3d_item **static_model_items, int static_count)
{
    int current = static_count;
    for(int i = 0; i < max_asteroids; ++i)
    {
        if(_asteroids[i].used && _asteroids[i].ptr)
        {
            current = _asteroids[i].ptr->statics_render(static_model_items, current);
        }
    }
    return current;
}

void enemy_manager::process_section_enemies(stage_section_list_ptr sections, size_t sections_count, bn::fixed camera_y)
{
    for (size_t i = 0; i < sections_count; ++i)
    {
        const stage_section *section = sections[i];
        if (camera_y <= section->starting_pos() && camera_y > section->ending_pos() &&
           section->starting_pos() < _last_section_start_y)
        {
            _last_section_start_y = section->starting_pos();

            for (int e = 0; e < section->enemies_count(); ++e)
            {
                const enemy_descriptor& enemy = section->enemies()[e];
                // <-- Make switch case
                if(enemy.type == enemy_type::ASTEROID)
                {
                    // <-- Separate this into its own method
                    for(int slot = 0; slot < max_asteroids; ++slot)
                    {
                        if(!_asteroids[slot].used)
                        {
                            fr::point_3d movement(0, 30, 0); // placeholder movement
                            // <-- DO NOT INSTANTIATE. This is no true pooling since we're just holding an array of pointers.
                            _asteroids[slot].ptr = new asteroid(enemy.position, movement, _models, _controller);
                            _asteroids[slot].used = true;
                            _asteroids[slot].source = &enemy;
                            BN_LOG("[SPAWN] ASTEROID: y DEPTH=" + bn::to_string<64>(int(enemy.position.y())) +
                                   " x=" + bn::to_string<64>(int(enemy.position.x())) +
                                   " z=" + bn::to_string<64>(int(enemy.position.z())));
                            break;
                        }
                    }
                }
            }
        }
    }
}

bool enemy_manager::check_collision_with_enemies()
{
    auto& player_collider = _player->collider_set();
    for(int i = 0; i < max_asteroids; ++i)
    {
        if(_asteroids[i].used && _asteroids[i].ptr)
        {
            if(player_collider.colliding_with_dynamic(_asteroids[i].ptr->get_collider()))
            {
                return true;
            }
        }
    }
    return false;
}
