#include "hud_manager.h"

#include "bn_log.h"
#include "bn_math.h"
#include "bn_memory.h" // <-- Only import when debugging
#include "bn_blending.h"
#include "bn_blending_actions.h"
#include "bn_sprite_actions.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"

#include "fr_camera_3d.h"
#include "fr_point_3d.h"
#include "fr_constants_3d.h"

#include "controller.h"
#include "player_ship.h"
#include "base_game_scene.h"

#include "bn_sprite_items_target_ui.h"
#include "bn_sprite_items_lifebar_frame.h"
#include "bn_sprite_items_lifebar_tile.h"
#include "editundo_sprite_font.h"
#include "k8x8_sprite_font.h"
#include "common_variable_8x16_sprite_font.h"
#include "common_variable_8x8_sprite_font.h"

hud_manager::hud_manager(base_game_scene *base_scene)
    : _base_scene(base_scene), _controller(base_scene->get_controller()),
      _camera(base_scene->get_camera()), _player_ship(base_scene->get_player_ship()),
      //   _text_generator(vonwaon_bitmap_sprite_font),
      _text_generator(editundo_sprite_font),
      _target_spr(bn::sprite_items::target_ui.create_sprite(0, 0)),
      _target_growth_action()
{
    _is_hidden = HIDE_HUD;

    // Setup target sprite
    _target_spr.set_horizontal_scale(TARGET_INITIAL_SCALE);
    _target_spr.set_vertical_scale(TARGET_INITIAL_SCALE);
    _target_growth_action = bn::sprite_scale_loop_action(_target_spr, TARGET_GROWTH_STEPS, TARGET_GROWTH_MAX_SCALE);

    // Build lifebar out of sprite tiles.
    constexpr int lifebar_frame_graphics_indices[LIFEBAR_FRAME_TILE_COUNT] = { 0, 1, 1, 1, 1, 1, 1, 2 };
    for (int index = 0; index < LIFEBAR_FRAME_TILE_COUNT; ++index)
    {
        bn::sprite_ptr sprite = bn::sprite_items::lifebar_frame.create_sprite(
            0, 0, lifebar_frame_graphics_indices[index]);
        sprite.set_top_left_x(LIFEBAR_START_X + index * LIFEBAR_FRAME_TILE_WIDTH);
        sprite.set_top_left_y(LIFEBAR_START_Y);
        _lifebar_frame_sprites.push_back(bn::move(sprite));
    }

    if (_is_hidden)
    {
        _target_spr.set_visible(false);
        for (bn::sprite_ptr& sprite : _lifebar_frame_sprites)
        {
            sprite.set_visible(false);
        }
        _lifebar_tiles.clear();
        _lifebar_damage_tiles.clear();
        _damage_hold_frames = 0;
        _damage_shrink_frames = 0;
        _damage_shrink_per_tile = 0;
        _invalidate_cached_hud_values();
    }
}

void hud_manager::destroy()
{
    _target_growth_action.reset();
}

void hud_manager::update(fr::models_3d *models)
{
    _move_target();

    if (_is_hidden)
    {
        return;
    }

    // text generators should be created only once // <-- what to do with this?

    // Display location Y debug text.
    if (_controller->is_debug_text_enabled())
    {
        _text_sprites.clear();

        _text_generator.generate(-7 * 16, -72, "Location (Y): " + bn::to_string<64>(int(_camera->position().y())),
                                 _text_sprites);
        _text_generator.generate(-7 * 16, -60,
                                 "Dynamic Objs: " + bn::to_string<64>(models->dynamic_models_count()) + "/" +
                                     bn::to_string<64>(models->dynamic_models_max_count()),
                                 _text_sprites);
        // _text_generator.generate(-7 * 16, -60, "Player (Y): " + bn::to_string<64>(int(_player_ship->get_position().y())),
        //                          _text_sprites);
    }
    else
    {
        
        // Only update HUD if meaningful changes to avoid unnecessary redraws.
        if (_should_update_hud())
        {
            const int health = _player_ship->get_health();
            const int missile_charge = _player_ship->get_player_missiles().get_current_charge();
            const int score = _base_scene->get_score();
            _update_lifebar(health);
            _text_sprites.clear();
            _text_generator.set_right_alignment();
            _text_generator.generate(110, -68,
                         bn::to_string<64>(missile_charge) + "%",
                         _text_sprites);
            _text_generator.set_left_alignment();
            _text_generator.generate(-115, -58, bn::to_string<64>(score),
                                     _text_sprites); // <-- Get another font?
            _displayed_missile_charge = missile_charge;
            _displayed_score = score;
        }

        _update_lifebar_damage_tiles();
    }

    // While fading, opt every freshly-created text sprite into blending so
    // they are affected by the shared transparency alpha.
    if (_is_blending_active)
    {
        for (bn::sprite_ptr &spr : _text_sprites)
        {
            spr.set_blending_enabled(true);
        }
    }

    // Tick blending fade actions
    if (_fade_in_action)
    {
        if (!_fade_in_action->done())
        {
            _fade_in_action->update();
        }
        else
        {
            // Fade-in complete: sprites are fully opaque, clean up blending.
            _set_hud_blending_enabled(false);
            _fade_in_action.reset();
        }
    }

    if (_fade_out_action)
    {
        if (!_fade_out_action->done())
        {
            _fade_out_action->update();
        }
        else
        {
            // Fade-out complete: keep blending active so sprites stay invisible
            // (disabling blending would snap them back to fully opaque).
            // <-- Fix this
            _set_hud_blending_enabled(false);
            _fade_out_action.reset();
        }
    }
}

void hud_manager::statics_update(int)
{
    // Display static count debug text.
    if (_controller->is_debug_text_enabled())
    {
        // _text_generator.generate(-7 * 16, -60, "Static Objs: " + bn::to_string<64>(static_count), _text_sprites);
        // _text_generator.generate(-7 * 16, -48, "IWRAM stack: " + bn::to_string<64>(bn::memory::used_stack_iwram()),
        // _text_sprites); _text_generator.generate(-7 * 16, -36, "IWRAM static: " +
        // bn::to_string<64>(bn::memory::used_static_iwram()), _text_sprites); _text_generator.generate(-7 * 16, -24,
        // "EWRAM: " + bn::to_string<64>(bn::memory::used_static_ewram()), _text_sprites);
    }
}

void hud_manager::_move_target()
{
    // Get raw input vector
    bn::fixed_point dir_input = _controller->get_smooth_directional();

    // BN_LOG("[target] dir_input: " + bn::to_string<64>(dir_input.x()) + ", " + bn::to_string<64>(dir_input.y()));

    bn::fixed_point target_pos;

    if (bn::abs(dir_input.x()) < DIRECTION_DEADZONE && bn::abs(dir_input.y()) < DIRECTION_DEADZONE)
    {
        target_pos = _compute_target_return();
    }
    else
    {
        target_pos = _compute_target_move(dir_input);
    }

    _target_spr.set_y(target_pos.y());
    _target_spr.set_x(target_pos.x());

    _player_ship->set_target_position(target_pos);

    _target_growth_action->update();
}

void hud_manager::show()
{
    if constexpr (HIDE_HUD)
    {
        return;
    }
    _is_hidden = false;
    // <-- text should auto-gen on update
    _target_spr.set_visible(true);
    for (bn::sprite_ptr& sprite : _lifebar_frame_sprites)
    {
        sprite.set_visible(true);
    }
}

void hud_manager::hide()
{
    _is_hidden = true;
    _set_hud_blending_enabled(false);
    _text_sprites.clear();
    _lifebar_tiles.clear();
    _lifebar_damage_tiles.clear();
    _damage_hold_frames = 0;
    _damage_shrink_frames = 0;
    _damage_shrink_per_tile = 0;
    _invalidate_cached_hud_values();
    // <-- Hide other HUD elements like score, target sprite, etc.
    _target_spr.set_visible(false);
    for (bn::sprite_ptr& sprite : _lifebar_frame_sprites)
    {
        sprite.set_visible(false);
    }
}

void hud_manager::fade_in()
{
    if constexpr (HIDE_HUD)
    {
        return;
    }
    _is_hidden = false;
    _fade_out_action.reset();
    _invalidate_cached_hud_values();

    // Opt the persistent target sprite into blending.
    // Text sprites are opted in each frame inside update() while fading.
    _target_spr.set_visible(true);
    _target_spr.set_blending_enabled(true);
    for (bn::sprite_ptr& sprite : _lifebar_frame_sprites)
    {
        sprite.set_visible(true);
        sprite.set_blending_enabled(true);
    }
    _is_blending_active = true;

    // Start fully transparent and animate to opaque (alpha 0 → 1).
    bn::blending::set_transparency_alpha(0);
    _fade_in_action.emplace(FADE_FRAMES, bn::fixed(1));
}

void hud_manager::fade_out()
{
    if constexpr (HIDE_HUD)
    {
        return;
    }
    _is_hidden = false;
    _fade_in_action.reset();
    _invalidate_cached_hud_values();

    // Opt the persistent target sprite into blending.
    // Text sprites are opted in each frame inside update() while fading.
    _target_spr.set_visible(true);
    _target_spr.set_blending_enabled(true);
    for (bn::sprite_ptr& sprite : _lifebar_frame_sprites)
    {
        sprite.set_visible(true);
        sprite.set_blending_enabled(true);
    }
    _is_blending_active = true;

    // Animate from fully opaque to fully transparent (alpha 1 → 0).
    bn::blending::set_transparency_alpha(1);
    _fade_out_action.emplace(FADE_FRAMES, bn::fixed(0));
}

void hud_manager::_set_hud_blending_enabled(bool blending_enabled)
{
    _is_blending_active = blending_enabled;

    _target_spr.set_blending_enabled(blending_enabled);
    for (bn::sprite_ptr& sprite : _lifebar_frame_sprites)
    {
        sprite.set_blending_enabled(blending_enabled);
    }
    for (bn::sprite_ptr& tile : _lifebar_tiles)
    {
        tile.set_blending_enabled(blending_enabled);
    }
    for (lifebar_damage_tile& dt : _lifebar_damage_tiles)
    {
        dt.spr.set_blending_enabled(blending_enabled);
    }
    for (bn::sprite_ptr& sprite : _text_sprites)
    {
        sprite.set_blending_enabled(blending_enabled);
    }
}

void hud_manager::_update_lifebar(int health)
{
    int tiles_to_show = health; // 1 health = 1 tile, 20 tiles max
    if (tiles_to_show > LIFEBAR_MAX_TILES)
    {
        tiles_to_show = LIFEBAR_MAX_TILES;
    }
    if (tiles_to_show < 0)
    {
        tiles_to_show = 0;
    }

    // Only rebuild if health changed
    if (tiles_to_show == _displayed_health)
    {
        return;
    }

    // Spawn red damage tiles for each lost slot (rightmost first kept visually).
    if (_displayed_health > 0 && tiles_to_show < _displayed_health)
    {
        int lost_start = tiles_to_show;
        int lost_end = _displayed_health;
        for (int i = lost_start; i < lost_end; ++i)
        {
            if (_lifebar_damage_tiles.full())
            {
                break;
            }
            bn::fixed left_x = LIFEBAR_START_X + i * LIFEBAR_TILE_SPACING;
            lifebar_damage_tile dt{
                bn::sprite_items::lifebar_tile.create_sprite(0, 0, LIFEBAR_RED_GRAPHICS_INDEX),
                bn::fixed(0)
            };
            dt.spr.set_top_left_x(left_x);
            dt.spr.set_top_left_y(LIFEBAR_START_Y);
            // Capture the actual center coord (screen-center origin) after placement
            // so the shrink phase can keep the visible left edge anchored correctly.
            dt.center_x = dt.spr.x();
            if (_is_blending_active)
            {
                dt.spr.set_blending_enabled(true);
            }
            _lifebar_damage_tiles.push_back(bn::move(dt));
        }

        _damage_hold_frames = LIFEBAR_DAMAGE_HOLD_FRAMES;
    }

    _displayed_health = tiles_to_show;
    _lifebar_tiles.clear();

    for (int i = 0; i < tiles_to_show; ++i)
    {
        bn::sprite_ptr tile = bn::sprite_items::lifebar_tile.create_sprite(0, 0);
        tile.set_top_left_x(LIFEBAR_START_X + i * LIFEBAR_TILE_SPACING);
        tile.set_top_left_y(LIFEBAR_START_Y);
        if (_is_blending_active)
        {
            tile.set_blending_enabled(true);
        }
        _lifebar_tiles.push_back(bn::move(tile));
    }
}

void hud_manager::_update_lifebar_damage_tiles()
{
    if (_lifebar_damage_tiles.empty())
    {
        return;
    }

    // Phase 1: hold timer on red tiles
    if (_damage_hold_frames > 0)
    {
        --_damage_hold_frames;
        return;
    }

    // Phase 2: shrink one tile at a time, rightmost first.
    if (_damage_shrink_per_tile == 0)
    {
        int n = _lifebar_damage_tiles.size();
        _damage_shrink_per_tile = LIFEBAR_DAMAGE_SHRINK_FRAMES / n;
        if (_damage_shrink_per_tile < 1)
        {
            _damage_shrink_per_tile = 1;
        }
        _damage_shrink_frames = _damage_shrink_per_tile;
    }

    auto active = _lifebar_damage_tiles.begin();
    for (auto it = _lifebar_damage_tiles.begin(); it != _lifebar_damage_tiles.end(); ++it)
    {
        if (it->center_x > active->center_x)
        {
            active = it;
        }
    }

    --_damage_shrink_frames;
    if (_damage_shrink_frames <= 0)
    {
        _lifebar_damage_tiles.erase(active);
        _damage_shrink_frames = _lifebar_damage_tiles.empty() ? 0 : _damage_shrink_per_tile;
        return;
    }

    bn::fixed scale = bn::fixed(_damage_shrink_frames) / _damage_shrink_per_tile;
    active->spr.set_horizontal_scale(scale);
    // Keep the visible left edge anchored: as scale shrinks, the center
    // shifts left by half-width * (1 - scale).
    active->spr.set_x(active->center_x - bn::fixed(LIFEBAR_TILE_WIDTH) / 2 * (bn::fixed(1) - scale));
}

bool hud_manager::_should_update_hud()
{
    const int health = _player_ship->get_health();
    const int missile_charge = _player_ship->get_player_missiles().get_current_charge();
    const int score = _base_scene->get_score();
    return health != _displayed_health ||
           missile_charge != _displayed_missile_charge ||
           score != _displayed_score;
}

void hud_manager::_invalidate_cached_hud_values()
{
    _displayed_health = -1;
    _displayed_score = -1;
    _displayed_missile_charge = -1;
}