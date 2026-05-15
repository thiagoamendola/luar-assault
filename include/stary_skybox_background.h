#ifndef STARY_SKYBOX_BACKGROUND_H
#define STARY_SKYBOX_BACKGROUND_H

#include "bn_fixed.h"
#include "bn_vector.h"
#include "bn_color.h"
#include "bn_bg_palette_item.h"
#include "bn_palette_bitmap_bg_ptr.h"
#include "bn_palette_bitmap_bg_painter.h"

#include "fr_camera_3d.h"

class stary_skybox_background
{
public:
    stary_skybox_background();

    // Render the skybox for the current camera orientation.
    void update(const fr::camera_3d& camera);

private:
    // A star in the skybox, stored as a direction on the unit sphere.
    struct star
    {
        bn::fixed x;  // Unit-sphere direction x
        bn::fixed y;  // Unit-sphere direction y
        bn::fixed z;  // Unit-sphere direction z
    };

    static constexpr int NUM_STARS = 256;

    // Palette colors
    static constexpr bn::color palette_colors[16] = {
        bn::color(3, 4, 9),    // 0: Dark blue/black background
        bn::color(10, 11, 22), // 1: Bright star center
        bn::color(6, 7, 15),   // 2: Medium star glow
        bn::color(4, 5, 11),   // 3: Dim star glow
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

    void _init_stars();
    void _draw_star(int sx, int sy, int color_index);
};

#endif
