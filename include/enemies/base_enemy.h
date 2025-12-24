#ifndef BASE_ENEMY_H
#define BASE_ENEMY_H

#include "fr_models_3d.h"

#include "colliders.h"
#include "player_ship.h"

enum class enemy_state {
    IDLE,
    ACTIVE,
    DESTROYING,
    DESTROYED
};

class base_enemy
{
public:
    base_enemy() = default;

    virtual void destroy() = 0;
    virtual void update(player_ship* player) = 0;
    
    virtual int statics_render(const fr::model_3d_item **static_model_items,
        int static_count) = 0;
    
    virtual void kill() = 0;

    virtual void handle_laser_hit() = 0;

    virtual sphere_collider_set *get_collider() = 0;
    
    fr::point_3d get_position() const
    {
        return _position;
    }

    bool is_destroyed() const { return _state == enemy_state::DESTROYED; }
    bool is_killed() const { return _state != enemy_state::ACTIVE; }
    
protected:
    enemy_state _state = enemy_state::IDLE;

    fr::point_3d _position;
};

#endif // BASE_ENEMY_H