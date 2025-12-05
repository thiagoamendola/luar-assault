#ifndef ENEMY_DEF_H
#define ENEMY_DEF_H

#include "bn_common.h"
#include "bn_fixed.h"
#include "fr_point_3d.h"

#include "enemy_type.h"

struct enemy_def
{
    fr::point_3d position;
    bn::fixed theta;
    enemy_type type;
};

#endif
