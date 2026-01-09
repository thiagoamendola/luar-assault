#include "end_stage_banner.h"

#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_blending.h"

#include "base_game_scene.h"

#include "bn_regular_bg_items_black.h"
#include "common_variable_8x16_sprite_font.h"

end_stage_banner::end_stage_banner(base_game_scene *base_scene) : 
    _base_scene(base_scene),
    _text_generator(common::variable_8x16_sprite_font),
    _ending_bg(bn::regular_bg_items::black.create_bg(0, 0))
{
    _text_generator.set_bg_priority(2);
    _text_generator.set_center_alignment();
    _ending_bg.set_priority(2);
    _ending_bg.set_visible(false);
}

void end_stage_banner::show()
{
    if (_is_shown)
    {
        return;
    }

    _is_shown = true;

    // Setup fade in
    _ending_bg.set_visible(true);
    _ending_bg.set_blending_enabled(true);
    bn::blending::set_transparency_alpha(0);
    _bgs_fade_in_action.emplace(60, .6); // <-- MAGIC NUMBERS
    
    // Show ending text
    _text_sprites.clear();
    // <-- Make text customizable later
    _text_generator.generate(0, -40, "STAGE CLEARED!", _text_sprites);
    _text_generator.generate(0, -10, "Score: " + bn::to_string<64>(_base_scene->get_score()), _text_sprites);
    _text_generator.generate(0, 10, "Thank you for playing this demo!", _text_sprites);
    _text_generator.generate(0, 30, "Press START to return", _text_sprites);
}

void end_stage_banner::update()
{
    if (!_is_shown)
    {
        return;
    }

    // Update fade in
    if (!_bgs_fade_in_action->done())
    {
        _bgs_fade_in_action->update();
    }
    else
    {
        // Check for start button press to exit
        if (bn::keypad::start_pressed())
        {
            _ready_to_exit = true;
        }
    }
}
