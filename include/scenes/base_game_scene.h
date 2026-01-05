#ifndef COMMON_GAME_SCENE_H
#define COMMON_GAME_SCENE_H

#include "bn_optional.h"
#include "bn_span.h"
#include "fr_camera_3d.h"
#include "fr_model_3d_item.h"
#include "fr_models_3d.h"

#include "scene_type.h"
#include "controller.h"
#include "enemy_manager.h"
#include "player_ship.h"
#include "hud_manager.h"
#include "pause_manager.h"
#include "stage_section.h"
#include "stage_section_renderer.h"

class base_game_scene // : public fr::scene
{
  public:
    base_game_scene(const bn::span<const bn::color> &scene_colors,
                      scene_colors_generator::color_mapping_handler *color_mapping, stage_section_list_ptr sections,
                      size_t sections_count);

    void destroy();

    // Returns true when ready to go to next scene.
    bool update();

    void set_hit_stop(int hit_stop_frames);

    void restart_scene();
    void return_to_main_menu();
    void prepare_to_finish_stage();

    bn::optional<scene_type> get_next_scene_override();

    controller* get_controller()
    {
      return &_controller;
    }
    fr::camera_3d* get_camera()
    {
      return &_camera;
    }
    fr::models_3d* get_models()
    {
      return &_models;
    }
    player_ship* get_player_ship()
    {
      return &_player_ship;
    }
    hud_manager* get_hud_manager()
    {
      return &_hud_manager;
    }

  private:
    stage_section_list_ptr _sections;
    size_t _sections_count;

    controller _controller;
    fr::camera_3d _camera;
    fr::models_3d _models;

    player_ship _player_ship;
    enemy_manager _enemy_manager;
    hud_manager _hud_manager;
    pause_manager _pause_manager;

    bn::span<const fr::model_3d_item> _model_items; // <-- CAN BEW REMOVED NOW?
    const fr::model_3d_item *_static_model_items[fr::constants_3d::max_static_models];

    bool _prepare_to_leave = false;
    bn::optional<scene_type> _next_scene_override;

    int _hit_stop_cooldown = 0;
};

#endif
