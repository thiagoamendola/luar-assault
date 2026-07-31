#include "dialog_manager.h"

#include "bn_sprite_builder.h"
#include "bn_sprite_items_dialog_bg.h"
#include "bn_utility.h"

#include "common_variable_8x16_sprite_font.h"

namespace
{
    constexpr char SUBTITLE_TEXT[] = "This is my subtitle";
}

dialog_manager::dialog_manager() :
    _subtitle_text_generator(common::variable_8x16_sprite_font)
{
    _subtitle_text_generator.set_bg_priority(0);
    _subtitle_text_generator.set_z_order(-1);
    _subtitle_text_generator.set_center_alignment();

    bn::sprite_tiles_ptr dialog_tiles = bn::sprite_items::dialog_bg.tiles_item().create_tiles();
    bn::sprite_palette_ptr dialog_palette = bn::sprite_items::dialog_bg.palette_item().create_palette();
    bn::sprite_builder builder(bn::sprite_items::dialog_bg.shape_size(), dialog_tiles, dialog_palette);
    builder.set_bg_priority(0);
    builder.set_z_order(0);

    for (int row = 0; row < DIALOG_ROWS; ++row)
    {
        builder.set_y(DIALOG_START_Y + row * DIALOG_TILE_SPACING);

        for (int col = 0; col < DIALOG_COLS; ++col)
        {
            builder.set_x(DIALOG_START_X + col * DIALOG_TILE_SPACING);
            _dialog_sprites.push_back(builder.build());
        }
    }
}

void dialog_manager::update()
{
    if (SUBTITLE_TEXT[_subtitle_character_index] == '\0')
    {
        return;
    }

    ++_subtitle_frame_counter;
    if (_subtitle_frame_counter < CHAR_SPEED)
    {
        return;
    }

    _subtitle_frame_counter = 0;
    _subtitle_text.push_back(SUBTITLE_TEXT[_subtitle_character_index]);
    ++_subtitle_character_index;

    _subtitle_text_sprites.clear();
    _subtitle_text_generator.generate(SUBTITLE_X, SUBTITLE_Y, _subtitle_text, _subtitle_text_sprites);
}