#ifndef ALPHA_STAGE_V1_SCENE_H
#define ALPHA_STAGE_V1_SCENE_H

#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_animate_actions.h"

#include "fr_camera_3d.h"
#include "fr_model_3d_item.h"
#include "fr_models_3d.h"
#include "fr_scene.h"
// #include "fr_sprite_3d.h"
// #include "fr_sprite_3d_item.h"

#include "base_game_scene.h"
#include "controller.h"
#include "player_ship.h"
#include "hud_manager.h"
#include "enemy_manager.h"
#include "scene_type.h"
#include "letterbox_manager.h"

class alpha_stage_v1_scene : public fr::scene
{

  public:
    alpha_stage_v1_scene();

    [[nodiscard]] bn::optional<scene_type> update() final;

  private:

    base_game_scene _base_game_scene;

    // fr::model_3d *_model; // <-- RETURN FOR TEST

    bool _prepare_to_leave;

    // <-- 3D SPRITE TEST. REMOVE
    // fr::sprite_3d *_test_sprite = nullptr;
    // fr::sprite_3d_item _test_sprite_sprite_3d_item;

    letterbox_manager _letterbox_manager;
    
    // Background
    // bn::regular_bg_ptr _scene_bg;
    bn::regular_bg_ptr _anim_bg;
    bn::regular_bg_cached_animate_action<5> _anim_bg_action;
    // bn::regular_bg_ptr _moon_bg;

};

#endif
