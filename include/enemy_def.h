#ifndef ENEMY_DEF_H
#define ENEMY_DEF_H

#include "bn_common.h"
#include "bn_fixed.h"
#include "fr_point_3d.h"

#include "enemy_type.h"

// Enemy-specific property structs
struct oyster_properties
{
    bn::fixed player_distance;
};

struct naiah_properties
{
    // Add naiah-specific properties here as needed
};

struct enemy_def
{
    fr::point_3d position;
    bn::fixed theta;
    enemy_type type;
    const void* properties;  // Pointer to enemy-specific properties
};

// Helper to access properties with type safety
template<typename T>
inline const T* get_enemy_properties(const enemy_def& def)
{
    return static_cast<const T*>(def.properties);
}

#endif
