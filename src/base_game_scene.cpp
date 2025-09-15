#include "base_game_scene.h"

#include "bn_keypad.h"

base_game_scene::base_game_scene(const bn::span<const bn::color> &scene_colors,
                                     scene_colors_generator::color_mapping_handler *color_mapping,
                                     stage_section_list_ptr sections, size_t sections_count)
        : _sections(sections), _sections_count(sections_count), _player_ship(&_controller, &_camera, &_models),
            _enemy_manager(&_models, &_controller, &_player_ship), _hud_manager(&_controller, &_camera, &_player_ship), _prepare_to_leave(false)
{
    // Initialize camera position.
    _camera.set_position(fr::point_3d(0, 1040, 0));

    // Load 3D model colors.
    _models.load_colors(scene_colors, color_mapping);
}

bool base_game_scene::update()
{
    int static_count = 0;

    if (_prepare_to_leave)
    {
        return true;
    }
    else if (bn::keypad::start_pressed())
    {
        // Start destroy process.
        destroy();
        return false;
    }
    else
    {
        // - UI
        _hud_manager.update();

        // - Player
        _player_ship.update();

        // - Enemies
        _enemy_manager.process_section_enemies(_sections, _sections_count, _camera.position().y());
        _enemy_manager.update();
    }

    {
        // - Collisions
        static_count =
            stage_section_renderer::manage_section_render(_sections, _sections_count, _camera, _static_model_items);

        _player_ship.collision_update(_static_model_items, static_count, _enemy_manager);
    }

    {
        // - Static object rendering

        // Player Laser
        static_count = _player_ship.statics_render(_static_model_items, static_count);

        // Enemies
        static_count = _enemy_manager.statics_render(_static_model_items, static_count);

        // Final models update
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
