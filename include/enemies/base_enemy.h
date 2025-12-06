#ifndef BASE_ENEMY_H
#define BASE_ENEMY_H

#include "fr_models_3d.h"

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
    virtual void update() = 0;
    
    virtual int statics_render(const fr::model_3d_item **static_model_items,
        int static_count) = 0;
    
    virtual void kill() = 0;

    bool is_destroyed() const { return _state == enemy_state::DESTROYED; }
    bool is_killed() const { return _state != enemy_state::ACTIVE; }
    
protected:
    enemy_state _state = enemy_state::IDLE;
};

#endif // BASE_ENEMY_H