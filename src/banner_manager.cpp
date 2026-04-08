#include "banner_manager.h"

#include "bn_sprite_builder.h"
#include "bn_sprite_items_banner_background.h"
#include "vonwaon_bitmap_black_sprite_font.h"

banner_manager::banner_manager(int delay_frames, int display_frames) :
    _banner_tiles(bn::sprite_items::banner_background.tiles_item().create_tiles()),
    _banner_palette(bn::sprite_items::banner_background.palette_item().create_palette()),
    _text_generator(vonwaon_bitmap_black_sprite_font),
    _delay_frames(delay_frames),
    _display_frames(display_frames)
{
    _text_generator.set_center_alignment();
    _text_generator.set_bg_priority(0);
    _text_generator.set_z_order(0);
    _banner_delay_timer = _delay_frames;
}

void banner_manager::update()
{
    if (_banner_delay_timer > 0)
    {
        --_banner_delay_timer;
        if (_banner_delay_timer == 0)
        {
            _show_banner(15, 4);
        }
    }
    else if (_banner_timer > 0)
    {
        --_banner_timer;
        if (_banner_timer == 0)
        {
            _banner_sprites.clear();
            _text_sprites.clear();
        }
    }
}

void banner_manager::_show_banner(int cols, int rows)
{
    constexpr int tile_size = 16;

    bn::sprite_builder builder(bn::sprite_items::banner_background.shape_size(),
                               _banner_tiles, _banner_palette);
    builder.set_bg_priority(0);
    builder.set_z_order(1);
    _banner_sprites.clear();
    _banner_timer = _display_frames;

    bn::fixed sprite_start_x = (-bn::fixed(cols - 1) / 2) * tile_size;
    bn::fixed sprite_y = (-bn::fixed(rows - 1) / 2) * tile_size;

    for (int y = 0; y < rows; ++y)
    {
        bn::fixed sprite_x = sprite_start_x;
        builder.set_y(sprite_y);
        sprite_y += tile_size;

        for (int x = 0; x < cols; ++x)
        {
            builder.set_x(sprite_x);
            sprite_x += tile_size;
            _banner_sprites.push_back(builder.build());
        }
    }

    // Centered label
    _text_sprites.clear();
    _text_generator.set_one_sprite_per_character(true);
    _text_generator.generate(0, 0, "MISSION START", _text_sprites);
    _text_generator.set_one_sprite_per_character(false);
    constexpr bn::fixed text_scale = 1.5;
    for (bn::sprite_ptr& spr : _text_sprites)
    {
        spr.set_x(spr.x() * text_scale);
        spr.set_y(spr.y() * text_scale);
        spr.set_scale(text_scale);
    }
}
