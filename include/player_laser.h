#ifndef PLAYER_LASER_H
#define PLAYER_LASER_H

#include "bn_colors.h"

#include "fr_model_3d_item.h"
#include "fr_model_colors.h"

class player_ship; // Forward declaration
class enemy_manager; // Forward declaration

namespace fr::model_3d_items
{

constexpr inline bn::color laser_colors[] = {
    bn::color(24, 24, 0),
};

}

// Laser state machine
enum class laser_state {
    PAUSED,
    SHOOTING,
    COOLDOWN
};


class player_laser
{
  public:
    player_laser(player_ship *player_ship);

    // Calculates wether laser is being used, collision and so on.
    void update(enemy_manager& enemies);

    void raycast_laser(enemy_manager& enemies);

    // Controls laser mesh render as a static model (at the end of update)
    int render_player_laser(const fr::model_3d_item **static_model_items,
                            int static_count);

  private:
    fr::vertex_3d laser_vertices[6] = {
        fr::vertex_3d(1, 0, 0), fr::vertex_3d(0, 1, 0), fr::vertex_3d(0, 0, 1),
        fr::vertex_3d(1, 1, 0), fr::vertex_3d(0, 1, 1), fr::vertex_3d(1, 0, 1),
    };

    fr::face_3d laser_faces[4] = {
        fr::face_3d(laser_vertices, fr::vertex_3d(0, 1, 0), 0, 1, 2, 0, 7),
        fr::face_3d(laser_vertices, fr::vertex_3d(0, 1, 0), 0, 2, 1, 0, 7),
        fr::face_3d(laser_vertices, fr::vertex_3d(0, 1, 0), 3, 4, 5, 0, 7),
        fr::face_3d(laser_vertices, fr::vertex_3d(0, 1, 0), 3, 5, 4, 0, 7),
        // fr::face_3d(laser_vertices, fr::vertex_3d(0, 1, 0), 0, 1, 2, 0, 7),
        // fr::face_3d(laser_vertices, fr::vertex_3d(0, 1, 0), 0, 2, 1, 0, 7),
        // fr::face_3d(laser_vertices, fr::vertex_3d(0, 1, 0), 3, 4, 5, 0, 7),
        // fr::face_3d(laser_vertices, fr::vertex_3d(0, 1, 0), 3, 5, 4, 0, 7),
    };

    fr::model_3d_item laser_full;
    player_ship *_player_ship;

    const int LASER_DURATION = 3;
    const int COOLDOWN_DURATION = 3;
    const int LASER_DISTANCE = 300;

    bool render_laser = false;
    laser_state state = laser_state::PAUSED;
    int laser_duration_count = 0;

    fr::point_3d laser_target = fr::point_3d(0, 0, 0);
};

#endif
