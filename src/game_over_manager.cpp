#include "game_over_manager.h"

#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_backdrop.h"
#include "bn_window.h"
#include "bn_blending.h"

#include "hud_manager.h"
#include "base_game_scene.h"
#include "controller.h"

#include "bn_regular_bg_items_red.h"
#include "common_variable_8x16_sprite_font.h"

game_over_manager::game_over_manager(base_game_scene *base_scene) : 
    _base_scene(base_scene),
    _hud_manager(base_scene->get_hud_manager()),
    _controller(base_scene->get_controller()),
    _text_generator(common::variable_8x16_sprite_font),
    _game_over_bg(bn::regular_bg_items::red.create_bg(0, 0))
{
    _text_generator.set_bg_priority(2);
    _text_generator.set_center_alignment();
    _game_over_bg.set_priority(2);
    _game_over_bg.set_visible(false);
}

void game_over_manager::show()
{
    if (_is_shown)
    {
        return;
    }

    _is_shown = true;

    // Hide game HUD
    _hud_manager->hide_game_hud();

    // Setup fade in
    _game_over_bg.set_visible(true);
    _game_over_bg.set_blending_enabled(true);
    bn::blending::set_transparency_alpha(0);
    _bgs_fade_in_action.emplace(60, .6); // <-- MAGIC NUMBERS
    
    // Show menu
    _current_selection = 0;
    // <-- Stop music
}

void game_over_manager::menu_update()
{
    if (!_is_shown)
    {
        return;
    }

    // Update fades
    if (!_bgs_fade_in_action->done())
    {
        _bgs_fade_in_action->update();
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
                case 0: // Restart
                    _base_scene->restart_scene();
                    return;
                    break;
                case 1: // Exit
                    _base_scene->return_to_main_menu();
                    return;
                    break;
            }
        }
    
        render_menu();
    }
}

void game_over_manager::hide_menu()
{
    // <-- Hide game over menu sprites and text
    _text_sprites.clear();
}

void game_over_manager::render_menu()
{
    _text_sprites.clear();

    _text_generator.generate(0, -40, "GAME OVER",
                                 _text_sprites);
    auto STARTING_Y = -10;
    auto OFFSET_Y = 15;

    // <-- Update selection text color or similar

    for (int i = 0; i < MENU_OPTIONS.size(); ++i)
    {
        bn::string<32> option_name = MENU_OPTIONS[i];
        if (i == _current_selection)
        {
            option_name = "> " + option_name + " <";
        }
        _text_generator.generate(0, STARTING_Y + i * OFFSET_Y, option_name,
                                _text_sprites);           
    }
}
