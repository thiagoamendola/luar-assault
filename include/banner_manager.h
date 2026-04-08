#ifndef BANNER_MANAGER_H
#define BANNER_MANAGER_H

#include "bn_sprite_ptr.h"
#include "bn_sprite_tiles_ptr.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

class banner_manager
{
public:
    banner_manager(int delay_frames, int display_frames);

    void update();

private:
    bn::sprite_tiles_ptr _banner_tiles;
    bn::sprite_palette_ptr _banner_palette;
    bn::sprite_text_generator _text_generator;
    bn::vector<bn::sprite_ptr, 60> _banner_sprites;
    bn::vector<bn::sprite_ptr, 16> _text_sprites;
    int _banner_delay_timer = 0;
    int _banner_timer = 0;
    int _delay_frames = 0;
    int _display_frames = 0;

    void _show_banner(int cols, int rows = 1);
};

#endif
