#include "pause_manager.h"

#include "bn_keypad.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_backdrop.h"
#include "bn_window.h"
#include "bn_blending.h"

#include "hud_manager.h"

#include "bn_regular_bg_items_black.h"
#include "common_variable_8x16_sprite_font.h"

pause_manager::pause_manager(hud_manager *hud_manager) : 
    _hud_manager(hud_manager),
    _text_generator(common::variable_8x16_sprite_font),
    _pause_bg(bn::regular_bg_items::black.create_bg(0, 0))
{
    _text_generator.set_bg_priority(2);
    _text_generator.set_center_alignment();
    _pause_bg.set_priority(2);
    _pause_bg.set_visible(false);

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
        BN_LOG("Fading in pause menu...");
        _bgs_fade_in_action->update();
    }

    if (_bgs_fade_out_action)
    {
        if (!_bgs_fade_out_action->done())
        {
            BN_LOG("Fading out pause menu...");
            _bgs_fade_out_action->update();
        }
        else
        {
            BN_LOG("Pause menu hidden.");
            _is_paused = false;
            _pause_bg.set_visible(false);
            _bgs_fade_out_action.reset();
        }
    }

    // <-- handle menu input
}

void pause_manager::show_menu()
{
    _hud_manager->hide_game_hud();

    _pause_bg.set_visible(true);
    _pause_bg.set_blending_enabled(true);
    bn::blending::set_transparency_alpha(0);
    _bgs_fade_in_action.emplace(10, .4); // <-- MAGIC NUMBERS
    
    // <-- Show pause menu sprites and text

    _text_generator.generate(0, 0, "PAUSED",
                                 _text_sprites);

}

void pause_manager::hide_menu()
{
    _bgs_fade_out_action.emplace(10, 0); // <-- MAGIC NUMBERS   

    // <-- Hide pause menu sprites and text
    _text_sprites.clear();
}
