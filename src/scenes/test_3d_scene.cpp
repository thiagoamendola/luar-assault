#include "test_3d_scene.h"
// #include "test_3d_scene_defs.h"
// #include "test_automated_scene_defs.h"
#include "stage_1_scene_defs.h"

#include "bn_bg_palettes_actions.h"
#include "bn_regular_bg_animate_actions.h"
#include "bn_colors.h"
#include "bn_core.h"
#include "bn_fixed.h"
#include "bn_keypad.h"
#include "bn_log.h"
#include "bn_music.h"
#include "bn_music_items.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_actions.h"
#include "bn_sound_items.h"
#include "bn_regular_bg_ptr.h"
#include "fr_model_3d_item.h"
#include "fr_model_colors.h"

#include "collision_detection.h"
#include "scene_type.h"
#include "base_game_scene.h"

// #include "bn_sprite_items_butano_background_2.h"
// #include "bn_sprite_items_ninja.h"
#include "models/shot.h"
#include "bn_regular_bg_items_black.h"
// #include "bn_regular_bg_items_moon.h"
#include "bn_regular_bg_items_bg_anim.h"
#include "common_variable_8x16_sprite_font.h"


test_3d_scene::test_3d_scene()
    : _base_game_scene(scene_colors, get_scene_color_mapping(), sections, sections_count),
    //   _enemy_manager(&_models, &_controller),
      _prepare_to_leave(false),
    //   _ninja_action(bn::create_sprite_animate_action_forever(
    //       _ninja_spr, 16, bn::sprite_items::ninja.tiles_item(), 0, 1, 2, 3)),
      _anim_bg(bn::regular_bg_items::bg_anim.create_bg(0, 0)),
      _anim_bg_action(bn::create_regular_bg_cached_animate_action_forever(
            _anim_bg, 6, bn::regular_bg_items::bg_anim.map_item(), 0, 1, 2, 3))
    //   _scene_bg(bn::regular_bg_items::floor.create_bg(0, 0)),
//   _moon_bg(bn::regular_bg_items::moon.create_bg(0, 20))
//   _test_sprite_sprite_3d_item(bn::sprite_items::butano_background_2, 0)
{

    // Scenario setup
    // bn::bg_palettes::set_transparent_color(bn::color(25, 18, 25));

    // Set music
    bn::music_items::gameplay_p1.play(0.5); // WORKS!!!
    // bn::music_items::gameplay_p2.play(0.5); // DOESN'T WORK =(!!!

    // Test sprite 3D
    // _test_sprite = &_models.create_sprite(_test_sprite_sprite_3d_item);
    // _test_sprite->set_position(fr::point_3d(0, 760, -20));
    // _test_sprite->set_theta(32000);
    // bn::sound_items::dialog_test1.play(1);
}

bn::optional<scene_type> test_3d_scene::update()
{
    bn::optional<scene_type> result;

    _anim_bg_action.update(); // <-- Pause on pause

    bool change_scene = _base_game_scene.update();
    
    if (change_scene)
    {
        auto scene_override = _base_game_scene.get_next_scene_override();
        if (scene_override)
        {
            return scene_override;
        }
        // Stage cleared - return to title
        result = scene_type::TITLE;
    }

    return result;
}

// <-- Make Destroy
