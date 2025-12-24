#include "enemy_manager.h"

#include "bn_fixed.h"
#include "bn_log.h"
#include "bn_string.h"

#include "fr_models_3d.h"

#include "controller.h"
#include "stage_section.h"
#include "player_ship.h"
#include "asteroid.h"
#include "oyster.h"
#include "enemy_def.h"

enemy_manager::enemy_manager(fr::models_3d *models, controller *controller, player_ship *player)
    : _models(models), _controller(controller), _player(player)
{
}

void enemy_manager::destroy()
{
    for (int i = 0; i < MAX_ENEMIES; ++i)
    {
        if (_enemies[i].used && _enemies[i].ptr)
        {
            _enemies[i].ptr->destroy();
            _enemies[i].used = false;
            _enemies[i].ptr = nullptr;
            _enemies[i].source = nullptr;
        }
    }
}

void enemy_manager::update()
{
    for (int i = 0; i < MAX_ENEMIES; ++i)
    {
        if (_enemies[i].used && _enemies[i].ptr)
        {
            _enemies[i].ptr->update(_player);
            // Cleanup if enemy destroyed itself this frame
            if (_enemies[i].ptr->is_destroyed())
            {
                delete _enemies[i].ptr; // <-- Convert to a proper object pool later
                _enemies[i].ptr = nullptr;
                _enemies[i].used = false;
                _enemies[i].source = nullptr;
            }
        }
    }
}

int enemy_manager::statics_render(const fr::model_3d_item **static_model_items, int static_count)
{
    int current = static_count;
    for (int i = 0; i < MAX_ENEMIES; ++i)
    {
        if (_enemies[i].used && _enemies[i].ptr)
        {
            current = _enemies[i].ptr->statics_render(static_model_items, current);
        }
    }
    return current;
}

void enemy_manager::process_section_enemies(stage_section_list_ptr sections, size_t sections_count, bn::fixed camera_y)
{
    for (size_t i = 0; i < sections_count; ++i)
    {
        const stage_section *section = sections[i];

        // Instantiate enemies when entering a new section
        if (camera_y <= section->starting_pos() && camera_y > section->ending_pos() &&
            section->starting_pos() < _last_section_start_y)
        {
            // Instantiate enemies in this section
            _last_section_start_y = section->starting_pos();

            for (int e = 0; e < section->enemies_count(); ++e)
            {
                const enemy_def &enemy = section->enemies()[e];
                // <-- Make switch case
                if (enemy.type == enemy_type::ASTEROID)
                {
                    // <-- Separate this into its own method
                    for (int slot = 0; slot < MAX_ENEMIES; ++slot)
                    {
                        if (!_enemies[slot].used)
                        {
                            fr::point_3d movement(0, 30, 0); // placeholder movement
                            // <-- DO NOT INSTANTIATE. This is no true pooling since we're just holding an array of pointers.
                            _enemies[slot].ptr = new asteroid(enemy.position, movement, _models, _controller); // <-- Convert to a proper object pool later
                            _enemies[slot].used = true;
                            _enemies[slot].source = &enemy;
                            BN_LOG("[SPAWN] ASTEROID: y DEPTH=" + bn::to_string<64>(int(enemy.position.y())) +
                                   " x=" + bn::to_string<64>(int(enemy.position.x())) +
                                   " z=" + bn::to_string<64>(int(enemy.position.z())));
                            break;
                        }
                    }
                }
                else if (enemy.type == enemy_type::OYSTER)
                {
                    // <-- Separate this into its own method
                    for (int slot = 0; slot < MAX_ENEMIES; ++slot)
                    {
                        if (!_enemies[slot].used)
                        {
                            fr::point_3d movement(0, 25, 0); // placeholder movement

                            // Get oyster-specific properties if available
                            const oyster_properties *props = nullptr;
                            if (enemy.properties)
                            {
                                props = get_enemy_properties<oyster_properties>(enemy);
                            }

                            // <-- DO NOT INSTANTIATE. This is no true pooling since we're just holding an array of pointers.
                            _enemies[slot].ptr = new oyster(enemy.position, movement, _models, _controller, this, props); // <-- Convert to a proper object pool later
                            _enemies[slot].used = true;
                            _enemies[slot].source = &enemy;
                            BN_LOG("[SPAWN] OYSTER: y DEPTH=" + bn::to_string<64>(int(enemy.position.y())) +
                                   " x=" + bn::to_string<64>(int(enemy.position.x())) +
                                   " z=" + bn::to_string<64>(int(enemy.position.z())));
                            break;
                        }
                    }
                }
            }
        }

        // Deinstantiate enemies from sections that the camera has passed
        // Only destroy if: camera passed the end AND we haven't destroyed this section yet
        if (camera_y <= section->ending_pos() && section->ending_pos() < _last_section_end_y)
        {
            // This section needs cleanup - destroy its enemies
            for (int slot = 0; slot < MAX_ENEMIES; ++slot)
            {
                if (_enemies[slot].used && _enemies[slot].ptr && _enemies[slot].source)
                {
                    // Check if this enemy belongs to the current section by comparing its descriptor
                    // against all enemies in this section
                    for (int e = 0; e < section->enemies_count(); ++e)
                    {
                        if (_enemies[slot].source == &section->enemies()[e])
                        {
                            // This enemy belongs to this section, destroy it
                            _enemies[slot].ptr->destroy();
                            delete _enemies[slot].ptr;
                            _enemies[slot].ptr = nullptr;
                            _enemies[slot].used = false;
                            _enemies[slot].source = nullptr;
                            BN_LOG("[DESTROY] Section enemy destroyed at ending_pos=" + bn::to_string<64>(int(section->ending_pos())));
                            break;
                        }
                    }
                }
            }

            // Update the tracker to this section's ending pos
            _last_section_end_y = section->ending_pos();
        }
    }

    // Clean up refless objects such as bullets
    for (int slot = 0; slot < MAX_ENEMIES; ++slot)
    {
        if (_enemies[slot].used && _enemies[slot].ptr && !_enemies[slot].source)
        {
            // This is a refless object, check if it's past the player. // <-- only checking past Y now
            if (_enemies[slot].ptr->get_position().y() > camera_y + bn::fixed(100)) // <-- Magic number
            {
                // Out of bounds, destroy it.
                _enemies[slot].ptr->destroy();
                delete _enemies[slot].ptr;
                _enemies[slot].ptr = nullptr;
                _enemies[slot].used = false;
                _enemies[slot].source = nullptr;
                BN_LOG("[DESTROY] Refless object destroyed at y=" + bn::to_string<64>(int(camera_y)));
            }
        }
    }

}

void enemy_manager::create_bullet(fr::point_3d position)
{
    // <-- Separate this into its own method
    for (int slot = 0; slot < MAX_ENEMIES; ++slot)
    {
        if (!_enemies[slot].used)
        {
            fr::point_3d movement(0, 30, 0); // placeholder movement
            // <-- DO NOT INSTANTIATE. This is no true pooling since we're just holding an array of pointers.
            _enemies[slot].ptr = new asteroid(position, movement, _models, _controller); // <-- Convert to a proper object pool later
            _enemies[slot].used = true;
            _enemies[slot].source = nullptr; // Bullets may not need a source descriptor
            BN_LOG("[SPAWN] BULLET: y DEPTH=" + bn::to_string<64>(int(position.y())) +
                    " x=" + bn::to_string<64>(int(position.x())) +
                    " z=" + bn::to_string<64>(int(position.z())));
            break;
        }
    }
}

