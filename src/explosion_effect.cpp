#include "explosion_effect.h"

#include "bn_sprite_items_boom.h"

explosion_effect::explosion_effect(fr::point_3d position, fr::models_3d *models)
    : _position(position), 
      _models(models),
      _sprite_3d_item(bn::sprite_items::boom, 0), // Load all 8 explosion frames
      _current_frame(TOTAL_FRAMES),
      _initial_scale(INITIAL_SCALE),
      _theta(INITIAL_THETA)
{
    // Create the sprite
    _sprite = &_models->create_sprite(_sprite_3d_item);
    _sprite->set_position(_position);
    _sprite->set_theta(_theta);
    _sprite->set_scale(_initial_scale);
}

explosion_effect::~explosion_effect()
{
    destroy();
}

bool explosion_effect::update()
{
    if(is_finished())
    {
        return true; // Animation complete
    }

    _current_frame -= 1;

    if(_current_frame <= 0)
    {
        destroy();
        return true; // Animation complete
    }

    // Update explosion sprite scale
    bn::fixed lerp = 1 - bn::fixed(_current_frame).division(bn::fixed(TOTAL_FRAMES));
    
    bn::fixed scale = _initial_scale + lerp * (FINAL_SCALE - INITIAL_SCALE);
    _sprite->set_scale(scale);

    // // Update explosion graphics frame (8 frames total, cycling through animation)
    // int explosion_graphics_index = (-(_current_frame / ANIMATION_SPEED) % 8) + 7;
    // _sprite_3d_item.update_sprite(explosion_graphics_index);

    return false; // Animation still running
}

void explosion_effect::destroy()
{
    if(_sprite)
    {
        _models->destroy_sprite(*_sprite);
        _sprite = nullptr;
    }
}
