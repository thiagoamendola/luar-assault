#ifndef LETTERBOX_MANAGER_H
#define LETTERBOX_MANAGER_H

#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_tiles_ptr.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_builder.h"

enum fading_state {
    NONE,
    FADING_IN,
    FADING_OUT
};

class letterbox_manager
{
public:
    letterbox_manager();

    void update();

    void show();
    void hide();

    bool is_shown() const
    {
        return _is_shown;
    };

    bool is_fading() const
    {
        return _fading_state != NONE;
    }

    void fade_out();
    void fade_in();

    void clear();

private:
    bn::sprite_tiles_ptr _letterboxing_tiles;
    bn::sprite_palette_ptr _letterboxing_palette;
    bn::sprite_builder _letterboxing_builder;
    bn::vector<bn::sprite_ptr, 36> _letterboxing_up_sprites;
    bn::vector<bn::sprite_ptr, 36> _letterboxing_down_sprites;
    bool _is_shown = false;
    fading_state _fading_state = NONE;
};

#endif