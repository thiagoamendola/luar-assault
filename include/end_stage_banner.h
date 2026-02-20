#ifndef END_STAGE_BANNER_H
#define END_STAGE_BANNER_H

#include "bn_sprite_ptr.h"
#include "bn_vector.h"
#include "bn_optional.h"
#include "bn_sprite_text_generator.h"
#include "bn_regular_bg_ptr.h"
#include "bn_blending_actions.h"

// - Forward declaration
class base_game_scene;

class end_stage_banner
{
public:
  end_stage_banner(base_game_scene *base_scene);

  void show();

  bool is_shown() const
  {
    return _is_shown;
  }

  void update();

  bool is_ready_to_exit() const
  {
    return _ready_to_exit;
  }

  // <-- Did we clear?

private:
  base_game_scene *_base_scene;

  bool _is_shown = false;
  bool _ready_to_exit = false;

  // Text UI
  bn::sprite_text_generator _text_generator;
  bn::vector<bn::sprite_ptr, 32> _text_sprites;

  // Fading
  bn::regular_bg_ptr _ending_bg;
  bn::optional<bn::blending_transparency_alpha_to_action> _bgs_fade_in_action;
};

#endif
