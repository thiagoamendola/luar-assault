#include "pause_manager.h"
#include "bn_keypad.h"
#include "hud_manager.h"

pause_manager::pause_manager(hud_manager *hud_manager) : _hud_manager(hud_manager)
{
    // Initialize pause menu resources
}

bool pause_manager::check_pause_toggle()
{
    if (bn::keypad::start_pressed())
    {
        _is_paused = !_is_paused;
        
        if (_is_paused)
        {
            show_menu();
        }
        else
        {
            hide_menu();
        }
    }
    
    return _is_paused;
}

void pause_manager::menu_update()
{
    // <-- Handle pause menu animations or updates
    // <-- handle menu input 
}

void pause_manager::show_menu()
{
    _hud_manager->hide_game_hud();
    // <-- Show pause menu sprites and text
    // <-- Create black filter with alpha

}

void pause_manager::hide_menu()
{
    // <-- Hide pause menu sprites and text
    // <-- Remove black filter
}
