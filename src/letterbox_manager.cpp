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

void letterbox_manager::update()
{
    if (_fading_state == NONE)
    {
        return;
    }

    // Apply fading

    int fade_direction = (_fading_state == FADING_OUT) ? 1 : -1;

    for (bn::sprite_ptr& sprite : _letterboxing_up_sprites)
    {
        sprite.set_y(sprite.y() - .5 * fade_direction); // <-- MAGIC NUMBERS
    }
    for (bn::sprite_ptr& sprite : _letterboxing_down_sprites)
    {
        sprite.set_y(sprite.y() + .5 * fade_direction); // <-- MAGIC NUMBERS
    }

    // Check if fade is complete

    if (_fading_state == FADING_OUT && _letterboxing_up_sprites.front().y() <= -72-32) // <-- MAGIC NUMBERS
    {
        BN_LOG("Fade out complete");
        _fading_state = NONE;
        clear();
    }
    else if (_fading_state == FADING_IN && _letterboxing_up_sprites.front().y() >= -72+32) // <-- MAGIC NUMBERS
    {
        BN_LOG("Fade in complete");
        _fading_state = NONE;
    }
}

void letterbox_manager::show()
{
    if (_is_shown)
    {
        return;
    }

    // Clear existing sprites if any
    if (!_letterboxing_up_sprites.empty())
    {
        _letterboxing_up_sprites.clear();
    }
    if (!_letterboxing_down_sprites.empty())
    {
        _letterboxing_down_sprites.clear();
    }

    // Create upper letterbox
    for (bn::fixed sprite_y = -72; sprite_y <= -56; sprite_y += 16) // <-- MAGIC NUMBERS
    {
        for (bn::fixed sprite_x = -112; sprite_x <= 112; sprite_x += 16) // <-- MAGIC NUMBERS
        {
            _letterboxing_builder.set_position(sprite_x, sprite_y);
            _letterboxing_up_sprites.push_back(_letterboxing_builder.build());
        }
    }

    // Create lower letterbox
    for (bn::fixed sprite_y = 72; sprite_y >= 56; sprite_y -= 16) // <-- MAGIC NUMBERS
    {
        for (bn::fixed sprite_x = -112; sprite_x <= 112; sprite_x += 16) // <-- MAGIC NUMBERS
        {
            _letterboxing_builder.set_position(sprite_x, sprite_y);
            _letterboxing_down_sprites.push_back(_letterboxing_builder.build());
        }
    }

    _is_shown = true;
    _fading_state = NONE;
}

void letterbox_manager::hide()
{
    clear();
}

void letterbox_manager::fade_out()
{
    // <-- Use frames
    show();
    _fading_state = FADING_OUT;
}

void letterbox_manager::fade_in()
{
    // <-- Use frames
    show();
    _fading_state = FADING_IN;
    // Move sprites out of screen for a start.
    for (bn::sprite_ptr& sprite : _letterboxing_up_sprites)
    {
        sprite.set_y(sprite.y() - 32); // <-- MAGIC NUMBERS
    }
    for (bn::sprite_ptr& sprite : _letterboxing_down_sprites)
    {
        sprite.set_y(sprite.y() + 32); // <-- MAGIC NUMBERS
    }
    
}


void letterbox_manager::clear()
{
    _letterboxing_up_sprites.clear();
    _letterboxing_down_sprites.clear();
    _is_shown = false;
    _fading_state = NONE;
}