#include "explosion_effect.h"

#include "bn_sprite_items_boom.h"

explosion_effect::explosion_effect(fr::point_3d position, fr::models_3d *models)
    : _position(position), 
      _models(models),
      _sprite_3d_item_1(bn::sprite_items::boom, 1),
      _sprite_3d_item_2(bn::sprite_items::boom, 0),
      _current_frame(TOTAL_FRAMES),
      _theta_1(INITIAL_THETA_1),
      _theta_2(INITIAL_THETA_2)
{
    // Create the sprites
    _sprite_2 = &_models->create_sprite(_sprite_3d_item_2);
    _sprite_2->set_position(_position);
    _sprite_2->set_theta(_theta_2);
    _sprite_2->set_scale(INITIAL_SCALE_2);

    _sprite_1 = &_models->create_sprite(_sprite_3d_item_1);
    _sprite_1->set_position(_position);
    _sprite_1->set_theta(_theta_1);
    _sprite_1->set_scale(INITIAL_SCALE_1);
}

explosion_effect::~explosion_effect()
{
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

    // Get interpolation factor (0 to 1)
    bn::fixed lerp = 1 - bn::fixed(_current_frame).division(bn::fixed(TOTAL_FRAMES));
    
    // Update sprite 1
    bn::fixed scale_1 = INITIAL_SCALE_1 + lerp * (FINAL_SCALE_1 - INITIAL_SCALE_1);
    _theta_1 += THETA_SPEED_1;
    _sprite_1->set_scale(scale_1);
    _sprite_1->set_theta(_theta_1);
    
    // Update sprite 2
    bn::fixed scale_2 = INITIAL_SCALE_2 + lerp * (FINAL_SCALE_2 - INITIAL_SCALE_2);
    _theta_2 += THETA_SPEED_2;
    _sprite_2->set_scale(scale_2);
    _sprite_2->set_theta(_theta_2);

    // // Update explosion graphics frame (8 frames total, cycling through animation)
    // int explosion_graphics_index = (-(_current_frame / ANIMATION_SPEED) % 8) + 7;
    // _sprite_3d_item_1.update_sprite(explosion_graphics_index);

    return false; // Animation still running
}

void explosion_effect::destroy()
{
    if(_sprite_1)
    {
        _models->destroy_sprite(*_sprite_1);
        _sprite_1 = nullptr;
    }
    if (_sprite_2)
    {
        _models->destroy_sprite(*_sprite_2);
        _sprite_2 = nullptr;
    }
}
