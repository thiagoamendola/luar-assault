#ifndef MODEL_VIEWER_SCENE_DEFS_H
#define MODEL_VIEWER_SCENE_DEFS_H

// Separated model entries list for model_viewer_scene.
// Add or remove models here and update _models_count in model_viewer_scene.h if needed.

#include "models/asteroid1.h"
#include "models/bush.h"
#include "models/player_ship_01.h"
#include "models/player_ship_02.h"
#include "models/shot.h"

#include "model_viewer_scene.h"

// Static list of models to cycle through.
inline const model_viewer_scene::model_entry model_viewer_scene::_entries[_models_count] = {
    { &fr::model_3d_items::player_ship_01_full, "Player Ship 01" },
    { &fr::model_3d_items::player_ship_02_full, "Player Ship 02" },
    { &fr::model_3d_items::bush_full,          "Bush" },
    { &fr::model_3d_items::shot_full,          "Shot" },
    { &fr::model_3d_items::asteroid1_full,     "Asteroid" },
};

#endif // MODEL_VIEWER_SCENE_DEFS_H
