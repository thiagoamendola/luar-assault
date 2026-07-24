#ifndef AUDIO_VIEWER_SCENE_H
#define AUDIO_VIEWER_SCENE_H

#include "bn_optional.h"
#include "bn_sound_item.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

#include "fr_scene.h"
#include "scene_type.h"

// Audio viewer for testing and listening to sound effect files.
// Controls:
//  UP / DOWN    : navigate sound list
//  A            : play selected sound
//  SELECT       : go to MODEL_VIEWER scene
//  START        : return to TITLE scene
class audio_viewer_scene : public fr::scene
{
public:
    audio_viewer_scene();

    bn::optional<scene_type> update() final;

    struct sound_entry
    {
        bn::sound_item item;
        const char* name;
    };

private:
    int _current_index = 0;

    // UI
    bn::vector<bn::sprite_ptr, 64> _text_sprites;
    bn::sprite_text_generator _text_generator;

    void _update_display();
};

// Runtime sound list is auto-generated in audio_viewer_scene_defs.h
namespace audio_viewer_defs {
    extern const int sound_count;
    extern const audio_viewer_scene::sound_entry entries[];
}

#endif // AUDIO_VIEWER_SCENE_H