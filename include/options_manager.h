#ifndef OPTIONS_MANAGER_H
#define OPTIONS_MANAGER_H

#include "bn_array.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string_view.h"
#include "bn_vector.h"

class controller;

class options_manager
{
public:
  options_manager(controller *controller);

  bool is_open() const
  {
    return _is_open;
  }

  void show_menu();
  void close_menu();
  void menu_update();

private:
  void hide_menu();
  void render_menu();
  void toggle_invert_y_axis();

  // <-- Add sound
  // <-- Implement Light options (ex.: Varooom 3D)
  // <-- Implement Sound options (ex.: Volume, Music/SFX toggle)
  // <-- Implement Rumble options
  static constexpr int INVERT_Y_AXIS_OPTION = 0;
  static constexpr int BACK_OPTION = 1;
  static constexpr bn::array<bn::string_view, 2> MENU_OPTIONS = {
    "Invert Y axis", "Back"
  };

  bool _is_open = false;

  controller *_controller;

  bn::sprite_text_generator _text_generator;
  bn::vector<bn::sprite_ptr, 32> _text_sprites;

  int _current_selection = 0;
};

#endif