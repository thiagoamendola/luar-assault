#ifndef LETTERBOX_MANAGER_H
#define LETTERBOX_MANAGER_H

#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_tiles_ptr.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_builder.h"

class letterbox_manager
{
public:
    letterbox_manager();

    // void update();
    bool is_shown() const;

    void show();
    void hide();

    // void fade_in(int frames);
    // void fade_out(int frames);

    void clear();

private:
    bn::sprite_tiles_ptr _letterboxing_tiles;
    bn::sprite_palette_ptr _letterboxing_palette;
    bn::sprite_builder _letterboxing_builder;
    bn::vector<bn::sprite_ptr, 36> _letterboxing_up_sprites;
    bn::vector<bn::sprite_ptr, 36> _letterboxing_down_sprites;
    bool _shown = false;
};

#endif