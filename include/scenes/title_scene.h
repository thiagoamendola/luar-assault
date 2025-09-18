#ifndef TITLE_SCENE_H
#define TITLE_SCENE_H

#include "bn_bg_palettes_actions.h"
#include "bn_memory.h"
#include "bn_sprite_palettes_actions.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"

#include "controller.h"
#include "player_ship.h"
#include "scene_type.h"

#include "fr_camera_3d.h"
#include "fr_model_3d_item.h"
#include "fr_models_3d.h"
#include "fr_scene.h"

class title_scene : public fr::scene
{

  public:
    title_scene();
    ~title_scene();

    [[nodiscard]] bn::optional<scene_type> update() final;

  private:
    bn::vector<bn::sprite_ptr, 32> _text_sprites;

    controller _controller;

    fr::camera_3d _camera;
    fr::models_3d _models;

    fr::model_3d *_model;

    bool _prepare_to_leave;
    bn::optional<bn::bg_palettes_fade_to_action> _bgs_fade_out_action;
    bn::optional<bn::sprite_palettes_fade_to_action> _sprites_fade_out_action;
};

#endif
