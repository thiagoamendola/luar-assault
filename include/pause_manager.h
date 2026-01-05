#ifndef PAUSE_MANAGER_H
#define PAUSE_MANAGER_H

#include "bn_sprite_ptr.h"
#include "bn_vector.h"
#include "bn_string_view.h"
#include "bn_array.h"
#include "bn_color.h"
#include "bn_optional.h"
#include "bn_sprite_text_generator.h"
#include "bn_regular_bg_ptr.h"
#include "bn_blending_actions.h"

#include "controller.h"
#include "hud_manager.h"

// - Forward declaration
class base_game_scene;

class pause_manager
{
public:
  pause_manager(base_game_scene *base_scene);

  bool check_pause_toggle();

  bool is_paused() const
  {
    return _is_paused;
  }

  void menu_update();

private:
  void show_menu();
  void hide_menu();

  void render_menu();

  static constexpr bn::array<bn::string_view, 3> MENU_OPTIONS = {
    "Continue", "Restart", "Exit"
  };

  base_game_scene *_base_scene;
  hud_manager *_hud_manager;
  controller *_controller;

  bool _is_paused = false;

  // Text UI
  bn::sprite_text_generator _text_generator; // <-- move to common stuff?
  bn::vector<bn::sprite_ptr, 32> _text_sprites;

  // Fading
  bn::regular_bg_ptr _pause_bg;
  bn::optional<bn::blending_transparency_alpha_to_action> _bgs_fade_in_action;
  bn::optional<bn::blending_transparency_alpha_to_action> _bgs_fade_out_action;
  // bn::optional<bn::bg_palettes_fade_to_action> _bgs_fade_in_action;
  // bn::optional<bn::sprite_palettes_fade_to_action> _sprites_fade_in_action;
  // bn::optional<bn::bg_palettes_fade_to_action> _bgs_fade_out_action;
  // bn::optional<bn::sprite_palettes_fade_to_action> _sprites_fade_out_action;

  // Menu selection
  int _current_selection = 0;
};

#endif
