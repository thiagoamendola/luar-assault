#ifndef HYPERLIGHT_BACKGROUND_H
#define HYPERLIGHT_BACKGROUND_H

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_vector.h"
#include "bn_color.h"
#include "bn_bg_palette_item.h"
#include "bn_palette_bitmap_bg_ptr.h"
#include "bn_palette_bitmap_bg_painter.h"

struct star
{
    bn::fixed x;
    bn::fixed y;
};

class hyperlight_background
{
public:
    hyperlight_background(bn::fixed_point speed, int trail_length);

    void update();

private:
    static constexpr int NUM_STARS = 32;
    
    // Palette colors: 0 = dark background, 1 = white star (padded to 16 colors)
    static constexpr bn::color palette_colors[16] = {
        bn::color(3, 4, 9),    // 0: Dark blue/black
        bn::color(8, 9, 18),   // 1: Main blue star
        bn::color(6, 6, 13),   // 2: Dim blue star
        // bn::color(31, 31, 31), // 1: White
        bn::color(0, 0, 0),    // Unused (black)
        bn::color(0, 0, 0),
        bn::color(0, 0, 0),
        bn::color(0, 0, 0),
        bn::color(0, 0, 0),
        bn::color(0, 0, 0),
        bn::color(0, 0, 0),
        bn::color(0, 0, 0),
        bn::color(0, 0, 0),
        bn::color(0, 0, 0),
        bn::color(0, 0, 0),
        bn::color(0, 0, 0),
        bn::color(0, 0, 0)
    };
    
    static constexpr bn::bg_palette_item palette_item{
        bn::span<const bn::color>(palette_colors),
        bn::bpp_mode::BPP_8
    };
    
    bn::palette_bitmap_bg_ptr _bg;
    bn::palette_bitmap_bg_painter _painter;
    
    bn::vector<star, NUM_STARS> _stars;
    bn::fixed_point _speed;
    int _trail_length;
    
    void _init_stars();
};

#endif
