#include "letterbox_manager.h"

#include "bn_log.h"
#include "bn_string.h"
#include "bn_fixed.h"
#include "bn_sprite_items_letterbox_tile.h"

letterbox_manager::letterbox_manager() :
    _letterboxing_tiles(bn::sprite_items::letterbox_tile.tiles_item().create_tiles()),
    _letterboxing_palette(bn::sprite_items::letterbox_tile.palette_item().create_palette()),
    _letterboxing_builder(bn::sprite_items::letterbox_tile.shape_size(), _letterboxing_tiles, _letterboxing_palette)
{
    _letterboxing_up_sprites.clear();
    _letterboxing_down_sprites.clear();
    _letterboxing_builder.set_bg_priority(2); // <-- Change to 1 after UI fade
}

bool letterbox_manager::is_shown() const
{
    return _shown;
}

void letterbox_manager::show()
{
    if (_shown)
    {
        return;
    }

    if (_letterboxing_up_sprites.empty())
    {

        // Upper letterbox
        for (bn::fixed sprite_y = -72; sprite_y <= -56; sprite_y += 16) // <-- MAGIC NUMBERS
        {
            for (bn::fixed sprite_x = -112; sprite_x <= 112; sprite_x += 16) // <-- MAGIC NUMBERS
            {
                _letterboxing_builder.set_position(sprite_x, sprite_y);
                _letterboxing_up_sprites.push_back(_letterboxing_builder.build());
            }
        }

        // Lower letterbox
        for (bn::fixed sprite_y = 72; sprite_y >= 56; sprite_y -= 16) // <-- MAGIC NUMBERS
        {
            for (bn::fixed sprite_x = -112; sprite_x <= 112; sprite_x += 16) // <-- MAGIC NUMBERS
            {
                BN_LOG("Creating letterbox sprite at: " + bn::to_string<64>(sprite_x) + ", " + bn::to_string<64>(sprite_y));
                _letterboxing_builder.set_position(sprite_x, sprite_y);
                _letterboxing_down_sprites.push_back(_letterboxing_builder.build());
            }
        }
    }
    else
    {
        for (bn::sprite_ptr& sprite : _letterboxing_up_sprites)
        {
            sprite.set_visible(true);
        }

        for (bn::sprite_ptr& sprite : _letterboxing_down_sprites)
        {
            sprite.set_visible(true);
        }
    }

    _shown = true;
}

void letterbox_manager::hide()
{
    clear();
}

void letterbox_manager::clear()
{
    _letterboxing_up_sprites.clear();
    _letterboxing_down_sprites.clear();
    _shown = false;
}