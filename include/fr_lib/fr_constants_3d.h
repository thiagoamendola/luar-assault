/*
 * Copyright (c) 2020-2024 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef FR_CONSTANTS_3D_H
#define FR_CONSTANTS_3D_H

#include "bn_common.h"

// <-- Separate into my own file to decouple from fr_lib

// <-- ALL OF THE FOLLOWING ARE CONFIGURABLE, MOSTLY FOR DEBUGGING. CHANGE TO
// USEFUL STUFF FOR ME

#ifndef FR_PROFILE
#define FR_PROFILE true
#endif

#ifndef FR_DETAILED_PROFILE
#define FR_DETAILED_PROFILE false
#endif

#ifndef FR_SHOW_CPU_USAGE
#define FR_SHOW_CPU_USAGE false
#endif

#ifndef FR_SHOW_CPU_USAGE_MAX
#define FR_SHOW_CPU_USAGE_MAX false
#endif

#ifndef FR_SHOW_CPU_USAGE_MIN
#define FR_SHOW_CPU_USAGE_MIN false
#endif

#ifndef FR_SHOW_CPU_USAGE_CURRENT
#define FR_SHOW_CPU_USAGE_CURRENT false
#endif

#ifndef FR_LOG_POLYGONS_PER_SECOND
#define FR_LOG_POLYGONS_PER_SECOND false
#endif

// == GAME VARS

#ifndef LOG_STATIC_RENDER_STEPS
#define LOG_STATIC_RENDER_STEPS false
#endif

// Enables player collider debug by pressing SELECT
#ifndef SHOW_COLLIDERS
#define SHOW_COLLIDERS false
#endif

// Enables debug text by pressing SELECT
#ifndef SHOW_DEBUG_TEXT
#define SHOW_DEBUG_TEXT true
#endif

// <-- Create FF for stage skips

namespace fr::constants_3d
{
constexpr int focal_length_shift = 8; // 8 minimum

constexpr int max_dynamic_models = 20;
constexpr int max_static_models = 64 - max_dynamic_models; // Original: 32
constexpr int max_stage_models = 1024;
constexpr int max_sprites = 8;

// <-- What are these?
constexpr int camera_min_y = 224;
constexpr int camera_max_y = 256;
constexpr int camera_diff_y = camera_max_y - camera_min_y;

constexpr int music_volume_data = 1 << 11; // <-- why this?
} // namespace fr::constants_3d

#endif
