#ifndef EXPLOSION_EFFECT_H
#define EXPLOSION_EFFECT_H

#include "bn_fixed.h"
#include "fr_point_3d.h"
#include "fr_models_3d.h"
#include "fr_sprite_3d_item.h"

class explosion_effect
{
  public:
    explosion_effect(fr::point_3d position, fr::models_3d *models);
    
    ~explosion_effect();

    // Update the explosion animation. Returns true if animation is complete.
    bool update();

    // Check if the explosion has finished its animation.
    bool is_finished() const { return _current_frame <= 0; }

    // Destroy the explosion sprite immediately.
    void destroy();

    // Getters
    const fr::point_3d& position() const { return _position; }
    int current_frame() const { return _current_frame; }
    int total_frames() const { return TOTAL_FRAMES; }

  private:
    fr::point_3d _position;
    fr::models_3d *_models;

    fr::sprite_3d_item _sprite_3d_item_1;
    fr::sprite_3d_item _sprite_3d_item_2;
    fr::sprite_3d* _sprite_1 = nullptr;
    fr::sprite_3d* _sprite_2 = nullptr;

    int _current_frame;
    bn::fixed _theta_1;
    bn::fixed _theta_2;

    static constexpr int ANIMATION_SPEED = 4; // frames per animation frame
    static constexpr int TOTAL_FRAMES = 10;

    static constexpr bn::fixed INITIAL_SCALE_1 = 1.5;
    static constexpr bn::fixed FINAL_SCALE_1 = 2;
    static constexpr bn::fixed INITIAL_THETA_1 = 40000;
    static constexpr bn::fixed THETA_SPEED_1 = 900;

    static constexpr bn::fixed INITIAL_SCALE_2 = .5;
    static constexpr bn::fixed FINAL_SCALE_2 = 1.5;
    static constexpr bn::fixed INITIAL_THETA_2 = 20000;
    static constexpr bn::fixed THETA_SPEED_2 = 500;
};

#endif
