#include "pause_manager.h"

#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_backdrop.h"
#include "bn_window.h"
#include "bn_blending.h"

#include "hud_manager.h"
#include "controller.h"

#include "bn_regular_bg_items_black.h"
#include "common_variable_8x16_sprite_font.h"
// #include "bn_sprite_items_variable_8x16_font_red.h"
// #include "bn_sprite_items_variable_8x16_font_white.h"

pause_manager::pause_manager(hud_manager *hud_manager, controller* controller) : 
    _hud_manager(hud_manager),
    _controller(controller),
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
    else if (_bgs_fade_out_action)
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
    else
    {
        // Handle menu input and render
    
        if (bn::keypad::up_pressed())
        {
            _current_selection--;
            if (_current_selection < 0)
            {
                _current_selection = MENU_OPTIONS.size() - 1;
            }
        }
        else if (bn::keypad::down_pressed())
        {
            _current_selection++;
            if (_current_selection >= MENU_OPTIONS.size())
            {
                _current_selection = 0;
            }
        }
        else if (bn::keypad::a_pressed())
        {
            // Handle selection
            switch (_current_selection)
            {
                case 0: // Continue
                    hide_menu();
                    return;
                    break;
                case 1: // Restart
                    break;
                case 2: // Exit
                    break;
            }
        }
    
        render_menu();
    }
}

void pause_manager::show_menu()
{
    // Hide game HUD
    _hud_manager->hide_game_hud();

    // Setup fade in
    _pause_bg.set_visible(true);
    _pause_bg.set_blending_enabled(true);
    bn::blending::set_transparency_alpha(0);
    _bgs_fade_in_action.emplace(10, .4); // <-- MAGIC NUMBERS
    
    // Show menu
    _current_selection = 0;
    render_menu();
}

void pause_manager::hide_menu()
{
    _bgs_fade_out_action.emplace(10, 0); // <-- MAGIC NUMBERS   

    // <-- Hide pause menu sprites and text
    _title_sprites.clear();
}

void pause_manager::render_menu()
{
    _title_sprites.clear();

    _text_generator.generate(0, -40, "PAUSED",
                                 _title_sprites);
    auto STARTING_Y = -10;
    auto OFFSET_Y = 15;

    for (int i = 0; i < MENU_OPTIONS.size(); ++i)
    {
        bn::string<32> option_name = MENU_OPTIONS[i];
        if (i == _current_selection)
        {
            option_name = "> " + option_name + " <";
        }
        _text_generator.generate(0, STARTING_Y + i * OFFSET_Y, option_name,
                                _title_sprites);           
    }

}
