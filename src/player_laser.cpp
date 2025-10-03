#include "player_laser.h"

#include "bn_colors.h"
#include "bn_keypad.h"
#include "bn_log.h"
#include "bn_string.h"
#include "bn_sound_items.h"

#include "fr_constants_3d.h"
#include "fr_model_3d_item.h"
#include "fr_sin_cos.h"

#include "player_ship.h"

player_laser::player_laser(player_ship *player_ship)
    : laser_full(laser_vertices, laser_faces, fr::model_3d_items::laser_colors),
    _player_ship(player_ship)
{
    laser_duration_count = 0;
}

void player_laser::update()
{
    switch (state)
    {
    case laser_state::PAUSED:
        // We can start the laser    
        if (bn::keypad::a_held())
        {
            // Ready to shoot
            render_laser = true;
            laser_duration_count = LASER_DURATION;
            state = laser_state::SHOOTING;
            bn::sound_items::player_laser.play();

            // Check for collision
            raycast_laser();
        }
        break;
    case laser_state::SHOOTING:
        // Continue laser render    
        render_laser = true;
        break;
    case laser_state::COOLDOWN:
        // Don't render laser  
        render_laser = false;
        break;
    }

    if (laser_duration_count > 0)
    {
        laser_duration_count--;

        if (laser_duration_count <= 0) {
            if (state == laser_state::SHOOTING) {
                state = laser_state::COOLDOWN;
                laser_duration_count = COOLDOWN_DURATION;
                render_laser = false;
            } else if (state == laser_state::COOLDOWN) {
                state = laser_state::PAUSED;
                render_laser = false;
            }
        }
    }
}

void player_laser::raycast_laser()
{
    fr::point_3d player_ship_pos = _player_ship->get_position();
    bn::fixed psi = _player_ship->get_model()->psi();
    bn::fixed phi = _player_ship->get_model()->phi();

    // - Create laser vector
    {
        int phi_raw = phi.right_shift_integer();
        int psi_raw = psi.right_shift_integer();

        // <-- Maybe create the trajectory above? I'll just render here.
        // Calculate laser trajectory.
        laser_vec = fr::point_3d(0, -LASER_DISTANCE, 0);
        // <-- use magnitude instead? Will require sqrt, so maybe not.
        laser_vec.set_x(-laser_vec.y() * fr::sin(phi_raw));
        laser_vec.set_y(laser_vec.y());
        laser_vec.set_z(-laser_vec.y() * fr::cos(psi_raw));

        laser_vec += player_ship_pos;
    }

    // <-- CONTINUE
    // Check against static models only?
    // If hit, apply damage to object (if applicable) and stop laser at hit point
    // If no hit, laser goes full distance
}

int player_laser::render_player_laser(
    const fr::model_3d_item **static_model_items, int static_count)
{
    if (render_laser)
    {
        fr::point_3d player_ship_pos = _player_ship->get_position();

        // Update vertices.
        //<-- Make const for these points for easy changing later
        laser_vertices[0].reset(player_ship_pos + fr::point_3d(10, 0, 2)); // <-- magic numbers
        laser_vertices[1].reset(player_ship_pos + fr::point_3d(10, 0, -2));
        laser_vertices[2].reset(laser_vec + fr::point_3d(3, 0, 0)); // <-- magic numbers
        laser_vertices[3].reset(player_ship_pos + fr::point_3d(-10, 0, 2));
        laser_vertices[4].reset(player_ship_pos + fr::point_3d(-10, 0, -2));
        laser_vertices[5].reset(laser_vec + fr::point_3d(-3, 0, 0));

        // Update faces.
        laser_faces[0].reset(laser_vertices, fr::vertex_3d(0, 1, 0), 0, 1, 2, 0,
                             7);
        laser_faces[1].reset(laser_vertices, fr::vertex_3d(0, 1, 0), 0, 2, 1, 0,
                             7);
        laser_faces[2].reset(laser_vertices, fr::vertex_3d(0, 1, 0), 3, 4, 5, 0,
                             7);
        laser_faces[3].reset(laser_vertices, fr::vertex_3d(0, 1, 0), 3, 5, 4, 0,
                             7);
        // laser_faces[4].reset(laser_vertices, fr::vertex_3d(0, 1, 0), 0, 2, 5, 0,
        //                      7);
        // laser_faces[5].reset(laser_vertices, fr::vertex_3d(0, 1, 0), 0, 5, 2, 0,
        //                      7);
        // laser_faces[6].reset(laser_vertices, fr::vertex_3d(0, 1, 0), 3, 5, 2, 0,
        //                      7);
        // laser_faces[7].reset(laser_vertices, fr::vertex_3d(0, 1, 0), 3, 2, 5, 0,
        //                      7);

        // Add nem mesh as static object
        if (static_count >= fr::constants_3d::max_static_models)
        {
            BN_LOG("Stage Section Renderer: reached static model max "
                   "limit: " +
                   bn::to_string<64>(fr::constants_3d::max_static_models));
            return static_count;
        }
        static_model_items[static_count] = &laser_full;
        return static_count + 1;
    }
    return static_count;
}
