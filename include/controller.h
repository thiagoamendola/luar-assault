#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "bn_fixed.h"
#include "bn_fixed_point.h"

class controller
{
  public:
    bn::fixed_point get_norm_directional();
    bn::fixed_point get_smooth_directional();

    void update();

    bool is_collider_display_enabled();
    bool is_debug_text_enabled();

    bool is_pause_pressed();

  private:
    const bn::fixed INTERP_STEP = 0.1;

    bn::fixed_point _previous_raw_dir_input;
    bn::fixed_point _smooth_dir_input;
    bn::fixed _interp;

    bool enable_collider_display = false; 
    bool enable_debug_text = false; 
};

#endif
