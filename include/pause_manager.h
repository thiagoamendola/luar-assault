#ifndef PAUSE_MANAGER_H
#define PAUSE_MANAGER_H

#include "bn_sprite_ptr.h"
#include "bn_vector.h"
#include "bn_color.h"
#include "bn_optional.h"
#include "bn_bg_palettes_actions.h"
#include "bn_sprite_palettes_actions.h"

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

    const bn::color fade_color{0, 0, 0};

    hud_manager *_hud_manager;
    bool _is_paused = false;
  
    bn::optional<bn::bg_palettes_fade_to_action> _bgs_fade_in_action;
    bn::optional<bn::sprite_palettes_fade_to_action> _sprites_fade_in_action;
    bn::optional<bn::bg_palettes_fade_to_action> _bgs_fade_out_action;
    bn::optional<bn::sprite_palettes_fade_to_action> _sprites_fade_out_action;
};

#endif
