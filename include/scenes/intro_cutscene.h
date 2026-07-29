#ifndef INTRO_CUTSCENE_H
#define INTRO_CUTSCENE_H

#include "bn_affine_bg_ptr.h"
#include "bn_bg_palettes_actions.h"
#include "bn_optional.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_palettes_actions.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

#include "fr_camera_3d.h"
#include "fr_model_3d_item.h"
#include "fr_models_3d.h"
#include "fr_scene.h"

#include "scene_type.h"
#include "letterbox_manager.h"
#include "hyperlight_background.h"

#include "cutscene/cutscene_timeline.h"
#include "cutscene/cutscene_commands.h"

class intro_cutscene_scene : public fr::scene
{

public:
    intro_cutscene_scene();
    ~intro_cutscene_scene();

    bn::optional<scene_type> update();

private:
    static constexpr int SKIP_PROMPT_DURATION = 120;
    static constexpr int TAKE_2_START_TIME = 360;

    fr::camera_3d _camera;
    fr::models_3d _models;
    fr::model_3d *_model = nullptr;
    bn::optional<bn::regular_bg_ptr> _floor_bg;
    bn::optional<bn::affine_bg_ptr> _earth_bg;
    bn::optional<bn::sprite_ptr> _luar_sprite;
    bn::optional<bn::sprite_ptr> _explosion_sprite;

    letterbox_manager _letterbox;
    cutscene_timeline _timeline;
    
    // <-- Move this to a more general cutscene manager
    bn::sprite_text_generator _text_generator;
    bn::sprite_text_generator _text_generator_2;
    bn::vector<bn::sprite_ptr, 40> _subtitle_text_sprites;
    bn::vector<bn::sprite_ptr, 20> _skip_text_sprites;
    int _skip_prompt_timer = 0;
    bool _skip_triggered = false;

    bn::optional<bn::bg_palettes_fade_to_action>     _bgs_fade_out_action;
    bn::optional<bn::sprite_palettes_fade_to_action> _sprites_fade_out_action;

    bn::optional<hyperlight_background> _hyperlight_bg;
};

#endif // INTRO_CUTSCENE_H
