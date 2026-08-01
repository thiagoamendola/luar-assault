#ifndef DIALOG_MANAGER_H
#define DIALOG_MANAGER_H

#include "bn_string.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

class dialog_manager
{
public:
    dialog_manager();

    void update();

private:
    static constexpr int CHAR_SPEED = 1;
    static constexpr int DIALOG_COLS = 12;
    static constexpr int DIALOG_ROWS = 3;
    static constexpr int DIALOG_INNER_SPRITES = (DIALOG_COLS - 2) * (DIALOG_ROWS - 2);
    static constexpr int DIALOG_MAX_SPRITES = DIALOG_INNER_SPRITES + (DIALOG_ROWS - 1) +
                                             (DIALOG_COLS - 2) + (DIALOG_COLS - 1) +
                                             (DIALOG_ROWS - 1) + 2;
    static constexpr int DIALOG_TILE_SPACING = 15;
    static constexpr int DIALOG_START_X = -65;
    static constexpr int DIALOG_START_Y = 38;
    static constexpr int SUBTITLE_X = 20;
    static constexpr int SUBTITLE_Y = 52;

    bn::sprite_text_generator _subtitle_text_generator;
    bn::vector<bn::sprite_ptr, DIALOG_MAX_SPRITES> _dialog_sprites;
    bn::vector<bn::sprite_ptr, 40> _subtitle_text_sprites;
    bn::string<64> _subtitle_text;
    int _subtitle_character_index = 0;
    int _subtitle_frame_counter = 0;
};

#endif