#ifndef COMMON_GAME_SCENE_H
#define COMMON_GAME_SCENE_H

#include "bn_optional.h"
#include "bn_span.h"
#include "fr_camera_3d.h"
#include "fr_model_3d_item.h"
#include "fr_models_3d.h"

#include "controller.h"
#include "enemy_manager.h"
#include "player_ship.h"
#include "hud_manager.h"
#include "stage_section.h"
#include "stage_section_renderer.h"

class base_game_scene // : public fr::scene
{
  public:
    base_game_scene(const bn::span<const bn::color> &scene_colors,
                      scene_colors_generator::color_mapping_handler *color_mapping, stage_section_list_ptr sections,
                      size_t sections_count);

    void destroy();

    // When ready to move, will return true to owner, who will control as they see fit.
    bool update();

    controller* get_controller()
    {
        return &_controller;
    }

    void set_hit_stop(int hit_stop_frames);

  private:
    stage_section_list_ptr _sections;
    size_t _sections_count;

    controller _controller;
    fr::camera_3d _camera;
    fr::models_3d _models;

    player_ship _player_ship;
    enemy_manager _enemy_manager;
    hud_manager _hud_manager;

    bn::span<const fr::model_3d_item> _model_items; // <-- CAN BEW REMOVED NOW?
    const fr::model_3d_item *_static_model_items[fr::constants_3d::max_static_models];

    bool _prepare_to_leave = false;
    int _hit_stop_cooldown = 0;
};

#endif
