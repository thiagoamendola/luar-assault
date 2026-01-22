#include "model_viewer_scene.h"
#include "model_viewer_scene_defs.h"

#include "bn_bg_palettes.h"
#include "bn_colors.h"
#include "bn_keypad.h"
#include "bn_music.h"
#include "bn_sound_items.h"
#include "bn_string.h"
#include "common_variable_8x16_sprite_font.h"
#include "fr_model_3d_item.h"
#include "fr_model_colors.h"

// (Model entries definition moved to model_viewer_scene_defs.h)

model_viewer_scene::model_viewer_scene() : _text_generator(common::variable_8x16_sprite_font)
{
    _camera.set_position(fr::point_3d(0, 0, 0));
    _camera.set_phi(0);
    bn::bg_palettes::set_transparent_color(bn::color(2, 2, 8));

    _create_model();            // Create first model (loads its own palette)
    _show_model_name();
}

model_viewer_scene::~model_viewer_scene()
{
    _destroy_model();
    // No need to clear palettes; next scene loads what it needs.
}

void model_viewer_scene::_load_palette() { /* unused: per-model palette loading */ }

void model_viewer_scene::_create_model()
{
    _destroy_model();
    const auto& entry = model_viewer_defs::entries[_current_index];
    const fr::model_3d_item& item = *entry.item;

    // Per-model palette: clear previous colors, then load only required colors.
    _models.clear_colors();
    if(const bn::color* palette_ptr = item.palette())
    {
        int max_index = -1;
        for(const fr::face_3d& face : item.faces())
        {
            if(face.color_index() > max_index)
            {
                max_index = face.color_index();
            }
        }
        if(max_index >= 0)
        {
            int palette_size = max_index + 1;
            _models.load_colors(bn::span<const bn::color>(palette_ptr, palette_size));
        }
    }
    else
    {
        // Fallback if model provides no palette.
        _models.load_colors(fr::default_model_colors);
    }

    _model = &_models.create_dynamic_model(item);
    _model->set_position(fr::point_3d(0, -100, 0)); // Requested showcase position
    _model->set_phi(0);
    _model->set_theta(0);
    _model->set_psi(0);
}

void model_viewer_scene::_destroy_model()
{
    if(_model)
    {
        _models.destroy_dynamic_model(*_model);
        _model = nullptr;
    }
}

void model_viewer_scene::_show_model_name()
{
    _text_sprites.clear();
    bn::string<48> text("MODEL ");
    text.append(bn::to_string<4>(_current_index + 1));
    text.push_back('/');
    text.append(bn::to_string<4>(model_viewer_defs::models_count));
    text.append(": ");
    text.append(model_viewer_defs::entries[_current_index].name);

    _text_generator.set_center_alignment();
    _text_generator.generate(0, 70, text, _text_sprites);
    _text_generator.set_left_alignment();
}

bn::optional<scene_type> model_viewer_scene::update()
{
    bn::optional<scene_type> result;

    // Inputs
    if (bn::keypad::start_pressed())
    {
        result = scene_type::TITLE;
        bn::sound_items::menu_confirm.play();
    }
    else if (bn::keypad::select_pressed())
    {
        result = scene_type::TITLE;
        bn::sound_items::menu_confirm.play();
    }
    else if (bn::keypad::a_pressed())
    {
        _current_index = (_current_index + 1) % model_viewer_defs::models_count;
        bn::sound_items::menu_focus.play();
        _create_model();
        _show_model_name();
    }
    else if (bn::keypad::b_pressed())
    {
        _current_index = (_current_index + model_viewer_defs::models_count - 1) % model_viewer_defs::models_count;
        bn::sound_items::menu_focus.play();
        _create_model();
        _show_model_name();
    }
    else
    {
        // Rotation controls
        if (bn::keypad::left_held())
        {
            _model->set_phi(_model->phi() - 256);
        }
        else if (bn::keypad::right_held())
        {
            _model->set_phi(_model->phi() + 256);
        }

        if (bn::keypad::up_held())
        {
            _model->set_psi(_model->psi() - 256);
        }
        else if (bn::keypad::down_held())
        {
            _model->set_psi(_model->psi() + 256);
        }

        if (bn::keypad::l_held())
        {
            _model->set_theta(_model->theta() - 256);
        }
        else if (bn::keypad::r_held())
        {
            _model->set_theta(_model->theta() + 256);
        }
    }

    _models.update(_camera);
    return result;
}
