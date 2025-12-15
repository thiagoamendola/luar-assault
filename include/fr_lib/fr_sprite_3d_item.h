/*
 * Copyright (c) 2020-2024 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef FR_SPRITE_3D_ITEM_H
#define FR_SPRITE_3D_ITEM_H

#include "bn_log.h"
#include "bn_string.h"

#include "bn_sprite_item.h"
#include "bn_sprite_tiles_ptr.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_affine_mat_ptr.h"
#include <initializer_list>

namespace fr
{

class sprite_3d_item
{
public:
    // Maximum number of frames for animation.
    static constexpr int MAX_FRAMES = 8;

    // Single-frame constructor
    sprite_3d_item(const bn::sprite_item& item, int graphics_index) :
        _tiles_item(item.tiles_item()),
        _palette(item.palette_item().create_palette()),
        _affine_mat(bn::sprite_affine_mat_ptr::create())
    {
        BN_ASSERT(item.shape_size().width() == 64 && item.shape_size().height() == 64, "Invalid shape size");
        BN_ASSERT(graphics_index >= 0, "Invalid graphics index");

        _frame_count = 1;
        _current_frame = 0;
        _frame_tiles[0] = new bn::sprite_tiles_ptr(_tiles_item.create_tiles(graphics_index));
        _frame_tile_ids[0] = _frame_tiles[0]->id();

        _tiles_id = _frame_tile_ids[0];
        _palette_id = _palette.id();
        _affine_mat_id = _affine_mat.id();

        // BN_LOG("[sprite_3d_item] created (single frame). tiles_id: " +
        //     bn::to_string<128>(_tiles_id) + 
        //     ", palette_id: " + bn::to_string<128>(_palette_id) +
        //     ", affine_mat_id: " + bn::to_string<128>(_affine_mat_id));
    }

    // Multi-frame constructor: preload all provided graphics indices into VRAM
    sprite_3d_item(const bn::sprite_item& item, std::initializer_list<int> graphics_indices) :
        _tiles_item(item.tiles_item()),
        _palette(item.palette_item().create_palette()),
        _affine_mat(bn::sprite_affine_mat_ptr::create())
    {
        BN_ASSERT(item.shape_size().width() == 64 && item.shape_size().height() == 64, "Invalid shape size");
        BN_ASSERT(!graphics_indices.size() || int(graphics_indices.size()) <= MAX_FRAMES, "Too many frames requested");
        BN_ASSERT(graphics_indices.size() > 0, "No graphics indices provided");

        _frame_count = int(graphics_indices.size());
        _current_frame = 0;

        int i = 0;
        for(int idx : graphics_indices)
        {
            BN_ASSERT(idx >= 0, "Invalid graphics index");
            _frame_tiles[i] = new bn::sprite_tiles_ptr(_tiles_item.create_tiles(idx));
            _frame_tile_ids[i] = _frame_tiles[i]->id();
            ++i;
        }

        _tiles_id = _frame_tile_ids[0];
        _palette_id = _palette.id();
        _affine_mat_id = _affine_mat.id();

        BN_LOG("[sprite_3d_item] created (multi frame). frame_count: " + bn::to_string<128>(_frame_count) +
            ", first tiles_id: " + bn::to_string<128>(_tiles_id) +
            ", palette_id: " + bn::to_string<128>(_palette_id) +
            ", affine_mat_id: " + bn::to_string<128>(_affine_mat_id));
    }

    [[nodiscard]] const bn::sprite_tiles_ptr& tiles() const
    {
        return *_frame_tiles[_current_frame];
    }

    [[nodiscard]] bn::sprite_tiles_ptr& tiles()
    {
        return *_frame_tiles[_current_frame];
    }

    [[nodiscard]] int tiles_id() const
    {
        return _tiles_id;
    }

    [[nodiscard]] const bn::sprite_palette_ptr& palette() const
    {
        return _palette;
    }

    [[nodiscard]] bn::sprite_palette_ptr& palette()
    {
        return _palette;
    }

    [[nodiscard]] int palette_id() const
    {
        return _palette_id;
    }

    [[nodiscard]] const bn::sprite_affine_mat_ptr& affine_mat() const
    {
        return _affine_mat;
    }

    [[nodiscard]] bn::sprite_affine_mat_ptr& affine_mat()
    {
        return _affine_mat;
    }

    [[nodiscard]] int affine_mat_id() const
    {
        return _affine_mat_id;
    }

    // Frame update: switch to a preloaded frame without touching tile refs.
    void update_sprite(int frame_index)
    {
        if(_frame_count <= 1)
        {
            return; // single frame
        }

        int new_frame = frame_index % _frame_count;
        if(new_frame != _current_frame)
        {
            _current_frame = new_frame;
            _tiles_id = _frame_tile_ids[_current_frame];
            BN_LOG("[sprite_3d_item] frame changed. new_frame: " +
                bn::to_string<128>(_current_frame) +
                ", tiles_id: " + bn::to_string<128>(_tiles_id));
        }
    }

    [[nodiscard]] int current_frame() const { return _current_frame; }

    ~sprite_3d_item()
    {
        for(int i = 0; i < _frame_count; ++i)
        {
            delete _frame_tiles[i];
            _frame_tiles[i] = nullptr;
        }
    }

private:
    int _tiles_id;
    int _palette_id;
    int _affine_mat_id;
    const bn::sprite_tiles_item& _tiles_item;
    // <-- In case these changes make the object too big, consider refactoring back.
    bn::sprite_tiles_ptr* _frame_tiles[MAX_FRAMES]; // pointers to allocated sprite tiles objects
    int _frame_tile_ids[MAX_FRAMES]; // parallel tile ids
    int _frame_count = 0;
    int _current_frame = 0;
    bn::sprite_palette_ptr _palette;
    bn::sprite_affine_mat_ptr _affine_mat;
};

}

#endif
