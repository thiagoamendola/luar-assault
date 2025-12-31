#ifndef PAUSE_MANAGER_H
#define PAUSE_MANAGER_H

#include "bn_sprite_ptr.h"
#include "bn_vector.h"
#include "bn_optional.h"

class hud_manager;

class pause_manager
{
  public:
    pause_manager(hud_manager *hud_manager);

    bool check_pause_toggle();

    bool is_paused() const
    {
        return _is_paused;
    }

    void menu_update();

  private:
    void show_menu();
    void hide_menu();

    hud_manager *_hud_manager;
    bool _is_paused = false;
  
};

#endif
