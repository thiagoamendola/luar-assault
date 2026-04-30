#include "banner_manager.h"

#include "bn_sound_items.h"
#include "bn_sprite_builder.h"

banner_manager::banner_manager(int delay_frames, int display_frames, bool play_sound_effect,
                               const bn::optional<banner_sprite_background_config>& sprite_bg_config,
                               const bn::optional<banner_regular_bg_config>& regular_bg_config,
                               const bn::span<const banner_sprite_entry>& sprite_entries) :
    _delay_frames(delay_frames),
    _display_frames(display_frames),
    _play_sound_effect(play_sound_effect),
    _sprite_bg_config(sprite_bg_config),
    _regular_bg_config(regular_bg_config)
{
    // Pre-load sprite background tiles and palette if configured
    if (_sprite_bg_config.has_value())
    {
        _banner_tiles = _sprite_bg_config->sprite_item->tiles_item().create_tiles();
        _banner_palette = _sprite_bg_config->sprite_item->palette_item().create_palette();
    }
    
    // Store sprite entries for delayed creation
    for (const banner_sprite_entry& entry : sprite_entries)
    {
        _pending_sprite_entries.push_back(entry);
    }
    
    _banner_delay_timer = _delay_frames;
}

void banner_manager::update()
{
    if (_banner_delay_timer > 0)
    {
        --_banner_delay_timer;
        if (_banner_delay_timer == 0)
        {
            // Show sprite background if configured
            if (_sprite_bg_config.has_value())
            {
                _show_sprite_background();
            }
            // Show regular background if configured
            if (_regular_bg_config.has_value())
            {
                _show_regular_background();
            }
            // Play sound effect
            if (_play_sound_effect)
            {
                bn::sound_items::impact.play();
            }
            _banner_timer = _display_frames;
        }
    }
    else if (_banner_timer > 0)
    {
        // <-- DO I REALLY WANT TO DO THIS EVERY UPDATE?
        // Update sprite entries (handle delayed appearances)
        _update_sprite_entries();
        
        --_banner_timer;
        if (_banner_timer == 0)
        {
            _banner_sprites.clear();
            _content_sprites.clear();
            _content_bg.reset();
            _pending_sprite_entries.clear();
        }
    }
}

void banner_manager::_show_sprite_background()
{
    if (!_sprite_bg_config.has_value() || !_banner_tiles.has_value() || !_banner_palette.has_value())
    {
        return;
    }
    
    const int cols = _sprite_bg_config->cols;
    const int rows = _sprite_bg_config->rows;
    
    constexpr int tile_size = 16;
    bn::sprite_builder builder(_sprite_bg_config->sprite_item->shape_size(),
                               _banner_tiles.value(), _banner_palette.value());
    builder.set_bg_priority(0);
    builder.set_z_order(1);
    _banner_sprites.clear();

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
}

void banner_manager::_show_regular_background()
{
    if (!_regular_bg_config.has_value())
    {
        return;
    }
    
    _content_bg = _regular_bg_config->bg_item->create_bg(
        _regular_bg_config->x, _regular_bg_config->y);
    _content_bg->set_priority(_regular_bg_config->priority);
}

void banner_manager::_update_sprite_entries()
{
    // Calculate elapsed frames since banner started
    int elapsed_frames = _display_frames - _banner_timer;
    
    // Check each pending sprite entry
    for (int i = _pending_sprite_entries.size() - 1; i >= 0; --i)
    {
        const banner_sprite_entry& entry = _pending_sprite_entries[i];
        
        if (elapsed_frames >= entry.delay_frames)
        {
            // Time to show this sprite
            bn::sprite_ptr sprite = entry.sprite_item->create_sprite(
                entry.x, entry.y, entry.graphics_index);
            sprite.set_bg_priority(0);
            sprite.set_z_order(0);
            _content_sprites.push_back(bn::move(sprite));
            
            // Remove from pending list
            _pending_sprite_entries.erase(_pending_sprite_entries.begin() + i);
        }
    }
}
