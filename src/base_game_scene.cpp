#include "base_game_scene.h"

#include "bn_keypad.h"

#include "scene_type.h"

base_game_scene::base_game_scene(const bn::span<const bn::color> &scene_colors,
                                     scene_colors_generator::color_mapping_handler *color_mapping,
                                     stage_section_list_ptr sections, size_t sections_count, int initial_position)
        : _sections(sections), _sections_count(sections_count), _player_ship(this),
            _enemy_manager(this), _hud_manager(this), _pause_manager(this),
            _game_over_manager(this), _end_stage_banner(this), _prepare_to_leave(false)
{
    // Initialize camera position.
    _player_ship.set_position(fr::point_3d(0, initial_position, 0));

    // Load 3D model colors.
    _models.load_colors(scene_colors, color_mapping);

    _score = 0;
    _next_scene_override = bn::nullopt;
}

bool base_game_scene::update()
{
    int static_count = 0;

    if (_prepare_to_leave)
    {
        return true;
    }

    // Handle game over state
    if (_game_over_manager.is_shown())
    {
        _game_over_manager.menu_update();
        return false;
    }

    // Handle end stage banner
    if (_end_stage_banner.is_shown())
    {
        _end_stage_banner.update();
        if (_end_stage_banner.is_ready_to_exit())
        {
            destroy();
        }
        return false;
    }

    // Handle pause state
    if (_pause_manager.check_pause_toggle())
    {
        // Is paused, only update pause menu.
        _pause_manager.menu_update();
        return false;
    }

    if (_hit_stop_cooldown > 0)
    {
        _hit_stop_cooldown--;
        return false;
    }
    
    // Handle normal updates
    {
        // - UI
        _hud_manager.update(&_models);

        // - Player
        _player_ship.update();

        // - Enemies
        _enemy_manager.process_section_enemies(_sections, _sections_count, _camera.position().y());
        _enemy_manager.update();

        // - Collisions
        static_count =
            stage_section_renderer::manage_section_render(_sections, _sections_count, _camera, _static_model_items);

        _static_collider_count =
            stage_section_renderer::collect_section_colliders(
                _sections, _sections_count, _camera.position().y(),
                _static_colliders, MAX_STATIC_COLLIDERS);

        _player_ship.collision_update(_static_model_items, static_count,
                                      _static_colliders, _static_collider_count,
                                      _enemy_manager);

        // - Debug render static colliders
        static_count = debug_render_static_colliders(_static_model_items, static_count);

        // - Static object rendering
        static_count = _player_ship.statics_render(_static_model_items, static_count);
        static_count = _enemy_manager.statics_render(_static_model_items, static_count);

        // - Final models update
        _models.set_static_model_items(_static_model_items, static_count);
        _models.update(_camera);
        _hud_manager.statics_update(static_count);
    }

    return false;
}

void base_game_scene::destroy()
{
    _prepare_to_leave = true;

    _player_ship.destroy();
    _enemy_manager.destroy();
}

int base_game_scene::debug_render_static_colliders(
    const fr::model_3d_item **static_model_items, int static_count)
{
    if (!_controller.is_collider_display_enabled())
    {
        return static_count;
    }

    for (int i = 0; i < _static_collider_count; i++)
    {
        auto &col = _static_colliders[i];
        auto &dbg = _static_collider_debuggers[i];

        dbg.debug_vertices[0].reset(
            col.position + fr::point_3d(col.radius, 0, 0));
        dbg.debug_vertices[1].reset(
            col.position + fr::point_3d(0, 0, col.radius));
        dbg.debug_vertices[2].reset(
            col.position + fr::point_3d(-col.radius, 0, 0));
        dbg.debug_vertices[3].reset(
            col.position + fr::point_3d(0, 0, -col.radius));

        dbg.debug_faces[0].reset(
            dbg.debug_vertices, fr::vertex_3d(0, 1, 0), 2, 1, 0, 0, 7);
        dbg.debug_faces[1].reset(
            dbg.debug_vertices, fr::vertex_3d(0, 1, 0), 0, 3, 2, 0, 7);

        if (static_count < fr::constants_3d::max_static_models)
        {
            static_model_items[static_count] = &dbg.debug_model;
            static_count++;
        }
    }

    return static_count;
}

void base_game_scene::set_hit_stop(int hit_stop_frames)
{
    _hit_stop_cooldown = hit_stop_frames;
}

void base_game_scene::restart_scene()
{
    _next_scene_override = scene_type::ALPHA_STAGE_V1;
    destroy();
}

void base_game_scene::return_to_main_menu()
{
    _next_scene_override = scene_type::TITLE;
    destroy();
}

void base_game_scene::prepare_to_finish_stage()
{
    _end_stage_banner.show();
}

bn::optional<scene_type> base_game_scene::get_next_scene_override()
{
    return _next_scene_override;
}

