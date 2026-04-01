#ifndef MOCK_CUTSCENE_SCENE_H
#define MOCK_CUTSCENE_SCENE_H

#include "bn_bg_palettes_actions.h"
#include "bn_optional.h"
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

#include "cutscene/cutscene_timeline.h"
#include "cutscene/cutscene_commands.h"

class mock_cutscene_scene : public fr::scene
{

public:
    mock_cutscene_scene();
    ~mock_cutscene_scene();

    bn::optional<scene_type> update();

private:
    static constexpr int SKIP_PROMPT_DURATION = 120;

    fr::camera_3d _camera;
    fr::models_3d _models;
    fr::model_3d *_model = nullptr;

    letterbox_manager _letterbox;

    cutscene_timeline _timeline;

    bn::sprite_text_generator _text_generator;
    bn::vector<bn::sprite_ptr, 20> _skip_text_sprites;
    int _skip_prompt_timer = 0;


    bn::optional<bn::bg_palettes_fade_to_action>     _bgs_fade_out_action;
    bn::optional<bn::sprite_palettes_fade_to_action> _sprites_fade_out_action;
};

#endif // MOCK_CUTSCENE_SCENE_H
