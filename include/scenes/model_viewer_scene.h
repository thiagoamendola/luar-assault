#ifndef MODEL_VIEWER_SCENE_H
#define MODEL_VIEWER_SCENE_H

#include "bn_bg_palettes_actions.h"
#include "bn_sprite_palettes_actions.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"
#include "bn_fixed.h"
#include "bn_sprite_text_generator.h"

#include "fr_scene.h"
#include "fr_camera_3d.h"
#include "fr_models_3d.h"

#include "scene_type.h"

namespace fr { class model_3d; }

// Simple model viewer which cycles through all 3D model items found in include/models.
// Controls:
//  LEFT / RIGHT : rotate (phi)
//  UP / DOWN    : rotate (psi)
//  L / R        : rotate (theta)
//  A            : next model
//  B            : previous model
//  START        : return to TITLE scene
// Text at bottom shows current model name and index.
class model_viewer_scene : public fr::scene
{
public:
    model_viewer_scene();
    ~model_viewer_scene();

    [[nodiscard]] bn::optional<scene_type> update() final;

private:
    struct model_entry
    {
        const fr::model_3d_item* item;
        const char* name;
    };

    static constexpr int _models_count = 5; // update if adding more entries
    static const model_entry _entries[_models_count];

    int _current_index = 0;

    fr::camera_3d _camera;
    fr::models_3d _models;
    fr::model_3d* _model = nullptr;

    // UI
    bn::vector<bn::sprite_ptr, 32> _text_sprites;
    bn::sprite_text_generator _text_generator;

    // Fade out actions for exit
    bn::optional<bn::bg_palettes_fade_to_action> _bgs_fade_out_action;
    bn::optional<bn::sprite_palettes_fade_to_action> _sprites_fade_out_action;

    void _load_palette();
    void _create_model();
    void _destroy_model();
    void _show_model_name();
};

#endif // MODEL_VIEWER_SCENE_H
