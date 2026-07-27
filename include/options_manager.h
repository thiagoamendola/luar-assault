#ifndef OPTIONS_MANAGER_H
#define OPTIONS_MANAGER_H

#include "bn_array.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string_view.h"
#include "bn_vector.h"

class options_manager
{
public:
  options_manager();

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

  // <-- Implement Light options (ex.: Varooom 3D)
  // <-- Implement Sound options (ex.: Volume, Music/SFX toggle)
  // <-- Implement Rumble options
  static constexpr bn::array<bn::string_view, 3> MENU_OPTIONS = {
    "Option 1", "Option 2", "Back"
  };

  bool _is_open = false;

  bn::sprite_text_generator _text_generator;
  bn::vector<bn::sprite_ptr, 32> _text_sprites;

  int _current_selection = 0;
};

#endif