#include "dialog_manager.h"

#include "bn_sprite_items_dialog.h"
#include "bn_utility.h"

#include "common_variable_8x16_sprite_font.h"

namespace
{
    constexpr char SUBTITLE_TEXT[] = "This is my subtitle";

    struct dialog_sprite_entry
    {
        bn::fixed x;
        bn::fixed y;
        int graphics_index;
    };

    static const dialog_sprite_entry dialog_entries[] = {
        // { -76, 70, 0 },
        // { -12, 70, 1 },
        // {  52, 70, 2 },
        // {  116, 70, 3 },
    };
}

dialog_manager::dialog_manager() :
    _subtitle_text_generator(common::variable_8x16_sprite_font)
{
    _subtitle_text_generator.set_bg_priority(0);
    _subtitle_text_generator.set_z_order(-1);
    _subtitle_text_generator.set_center_alignment();

    for (const dialog_sprite_entry& entry : dialog_entries)
    {
        bn::sprite_ptr sprite = bn::sprite_items::dialog.create_sprite(entry.x, entry.y, entry.graphics_index);
        sprite.set_bg_priority(0);
        sprite.set_z_order(0);
        _dialog_sprites.push_back(bn::move(sprite));
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