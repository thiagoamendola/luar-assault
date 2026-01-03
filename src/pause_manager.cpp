#include "pause_manager.h"

#include "bn_keypad.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"

#include "hud_manager.h"

#include "common_variable_8x16_sprite_font.h"

pause_manager::pause_manager(hud_manager *hud_manager) : 
    _hud_manager(hud_manager),
    _text_generator(common::variable_8x16_sprite_font)
{
    // Initialize pause menu resources
    bn::bg_palettes::set_fade(bn::colors::black, 0);
    _bgs_fade_in_action.emplace(10, .4); // <-- MAGIC NUMBERS
    _sprites_fade_in_action.emplace(10, .4);
}

bool pause_manager::check_pause_toggle()
{
    if (bn::keypad::start_pressed())
    {
        if (!_is_paused)
        {
            _is_paused = true;
            show_menu();
        }
        else
        {
            // Only unpause after fade out is complete
            hide_menu();
        }
    }

    return _is_paused;
}

void pause_manager::menu_update()
{
    // Update fades

    if (!_bgs_fade_in_action->done())
    {
        BN_LOG("Fading out pause menu...");
        _bgs_fade_in_action->update();
        _sprites_fade_in_action->update();
    }

    if (_bgs_fade_out_action)
    {
        if (!_bgs_fade_out_action->done())
        {
            BN_LOG("Fading out pause menu...");
            _bgs_fade_out_action->update();
            _sprites_fade_out_action->update();
        }
        else
        {
            BN_LOG("Pause menu hidden.");
            _is_paused = false;
            _bgs_fade_out_action.reset();
            _sprites_fade_out_action.reset();
        }
    }

    // <-- Handle pause menu animations or updates
    // <-- handle menu input
}

void pause_manager::show_menu()
{
    _hud_manager->hide_game_hud();
    // Set fade to black
    _bgs_fade_in_action.emplace(10, .4); // <-- MAGIC NUMBERS
    _sprites_fade_in_action.emplace(10, .4);

    // <-- Show pause menu sprites and text
    // set_priority??? set_blending???

    _text_generator.set_center_alignment();
    _text_generator.set_blending_enabled(false); // DOES NOT WORK
    _text_generator.generate(0, 0, "PAUSED",
                                 _text_sprites);

}

void pause_manager::hide_menu()
{
    _bgs_fade_out_action.emplace(10, 0);
    _sprites_fade_out_action.emplace(10, 0);
    // bn::bg_palettes::set_fade(fade_color, 1);

    // <-- Hide pause menu sprites and text
    _text_sprites.clear();
}
