#ifndef BANNER_MANAGER_H
#define BANNER_MANAGER_H

#include "bn_fixed.h"
#include "bn_optional.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_item.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_tiles_ptr.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_vector.h"

// Defines a sprite entry with position and delay for appearing
struct banner_sprite_entry
{
    const bn::sprite_item* sprite_item;
    bn::fixed x;
    bn::fixed y;
    int delay_frames;       // Delay before this sprite appears (relative to banner start)
    int graphics_index = 0; // Optional: which graphics index to use from the sprite item
    // <-- ADD SOUNDFX
};

// Configuration for the tiled sprite background (made of multiple sprites)
struct banner_sprite_background_config
{
    const bn::sprite_item* sprite_item;
    int cols;
    int rows;
};

// Configuration for a regular background as content
struct banner_regular_bg_config
{
    const bn::regular_bg_item* bg_item;
    bn::fixed x = 0;
    bn::fixed y = 0;
    int priority = 0;
};

class banner_manager
{
public:
    // Constructor with optional configs
    banner_manager(int delay_frames, int display_frames,
                   const bn::optional<banner_sprite_background_config>& sprite_bg_config = bn::nullopt,
                   const bn::optional<banner_regular_bg_config>& regular_bg_config = bn::nullopt,
                   const bn::span<const banner_sprite_entry>& sprite_entries = {});

    void update();

    // Check if the banner has finished displaying
    [[nodiscard]] bool is_done() const { return _banner_delay_timer == 0 && _banner_timer == 0; }

private:
    // Sprite-based tiled background
    bn::optional<bn::sprite_tiles_ptr> _banner_tiles;
    bn::optional<bn::sprite_palette_ptr> _banner_palette;
    bn::vector<bn::sprite_ptr, 60> _banner_sprites;
    
    // Regular background content
    bn::optional<bn::regular_bg_ptr> _content_bg;
    
    // Sprite content
    bn::vector<bn::sprite_ptr, 32> _content_sprites;
    bn::vector<banner_sprite_entry, 16> _pending_sprite_entries;
    
    int _banner_delay_timer = 0;
    int _banner_timer = 0;
    int _delay_frames = 0;
    int _display_frames = 0;
    
    bn::optional<banner_sprite_background_config> _sprite_bg_config;
    bn::optional<banner_regular_bg_config> _regular_bg_config;

    void _show_sprite_background();
    void _show_regular_background();
    void _update_sprite_entries();
};

#endif
