#include "controller.h"

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_log.h"
#include "bn_string.h"
#include "fr_constants_3d.h"

#include "utils.h"

bn::fixed_point controller::get_norm_directional()
{
    bn::fixed_point raw_dir_input(0.0, 0.0);

    if (bn::keypad::up_held())
    {
        raw_dir_input.set_y(-1.0);
    }
    else if (bn::keypad::down_held())
    {
        raw_dir_input.set_y(1.0);
    }

    if (bn::keypad::right_held())
    {
        raw_dir_input.set_x(1.0);
    }
    else if (bn::keypad::left_held())
    {
        raw_dir_input.set_x(-1.0);
    }

    // Normalize manually

    raw_dir_input = unit_vector(raw_dir_input);

    return raw_dir_input;
}

bn::fixed_point controller::get_smooth_directional()
{
    bn::fixed_point current_raw_dir_input = get_norm_directional();

    // Check if it's different from previous one. If so, reset interpolation
    // counter.
    if (current_raw_dir_input != _previous_raw_dir_input
        //  || (current_raw_dir_input.x() == 0 && current_raw_dir_input.y() == 0)
    )
    {
        _interp = 0;
    }

    // Move interpolation counter
    _interp += INTERP_STEP;
    _interp = _interp > 1 ? 1 : _interp;

    // Grab diff
    bn::fixed_point diff_vec = current_raw_dir_input - _smooth_dir_input;

    // if (bn::keypad::a_held())
    // {
    //     BN_LOG("current: " + bn::to_string<32>(current_raw_dir_input.x()));
    //     BN_LOG("smooth: " + bn::to_string<32>(_smooth_dir_input.x()));
    //     BN_LOG("diff: " + bn::to_string<32>(diff_vec.x()));
    // }

    // Calculate fractional diff
    diff_vec *= _interp;

    // Update smooth dir
    _smooth_dir_input += diff_vec;
    
    // Make it zero when raw input is zero
    if (abs(_smooth_dir_input.x()) < 0.02 && abs(_smooth_dir_input.y()) < 0.02)
    {
        _smooth_dir_input.set_x(0);
        _smooth_dir_input.set_y(0);
    }
    
    // if (bn::keypad::a_held())
    // {
    //     BN_LOG("diff lerp: " + bn::to_string<32>(diff_vec.x()));
    //     BN_LOG("FINAL smooth: " + bn::to_string<32>(_smooth_dir_input.x()));
    //     BN_LOG("=====================================");
    // }

    _previous_raw_dir_input = current_raw_dir_input;

    return _smooth_dir_input;
}

void controller::update()
{
// Toggle collider visibility
#if SHOW_COLLIDERS
    if (bn::keypad::select_pressed())
    {
        enable_collider_display = !enable_collider_display;
    }
#endif

// Toggle debug text
#if SHOW_DEBUG_TEXT
    if (bn::keypad::select_pressed())
    {
        enable_debug_text = !enable_debug_text;
    }
#endif
}

bool controller::is_collider_display_enabled()
{
    return enable_collider_display;
}

bool controller::is_debug_text_enabled()
{
    return enable_debug_text;
}
