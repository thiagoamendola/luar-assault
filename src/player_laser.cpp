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
#include "enemy_manager.h"

player_laser::player_laser(player_ship *player_ship)
    : laser_full(laser_vertices, laser_faces, fr::model_3d_items::laser_colors),
    _player_ship(player_ship)
{
    laser_duration_count = 0;
}

void player_laser::update(enemy_manager& enemies)
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
            raycast_laser(enemies);
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

void player_laser::raycast_laser(enemy_manager& enemies)
{
    fr::point_3d player_ship_pos = _player_ship->get_position();
    bn::fixed psi = _player_ship->get_model()->psi();
    bn::fixed phi = _player_ship->get_model()->phi();

    // - Create laser vector (world-space end point of full-length beam before collision truncation)
    {
        int phi_raw = phi.right_shift_integer();
        int psi_raw = psi.right_shift_integer();

        // Build oriented beam. Forward is -Y. We rotate a base vector (0, -LASER_DISTANCE, 0)
        // using horizontal angles (phi, psi). We keep squared length later (no sqrt needed here).
        laser_target = fr::point_3d(0, -LASER_DISTANCE, 0);
        // We intentionally avoid normalizing or using magnitude (would need sqrt on GBA).
        laser_target.set_x(-laser_target.y() * fr::sin(phi_raw));
        laser_target.set_y(laser_target.y());
        laser_target.set_z(-laser_target.y() * fr::cos(psi_raw));

        laser_target += player_ship_pos;
    }

    // - Check laser cast against enemies
    {
        // If hit, apply damage to object (if applicable) and stop laser at hit point
        // If no hit, laser goes full distance.
        // <-- Check against static models?
        
        asteroid_slot* enemy_slots = enemies.get_enemies();
        fr::point_3d laser_origin = player_ship_pos;
        fr::point_3d laser_vec = laser_target - laser_origin; // segment vector
        // bn::fixed laser_len2 = laser_vec.dot_product(laser_vec);   // squared length (|laser_vec|^2)

        // === Integer preparation ===
        int lvx = laser_vec.x().integer();
        int lvy = laser_vec.y().integer();
        int lvz = laser_vec.z().integer();
        // <-- Should I create a helper for method below?
        // We're holding length squared in 64-bit to avoid overflow while skipping sqrt for the incoming calculations.
        int64_t laser_len2 = int64_t(lvx)*lvx + int64_t(lvy)*lvy + int64_t(lvz)*lvz; // 64-bit to avoid overflow
        if(laser_len2 <= 0)
        {
            return; // Invalid laser
        }

        int64_t clamped_closest_proj_dist = laser_len2; // Start with max length (no hit)
        bool found_hit = false;
        int best_enemy_index = -1; // <-- replace with ptr to enemy
        // bool best_is_endpoint = false;       // distinguishes interior vs endpoint intersection

        for(int i = 0; i < enemy_manager::MAX_ENEMIES; ++i)
        {
            if(!(enemy_slots[i].used && enemy_slots[i].ptr))
            {
                continue;
            }
            auto* target = enemy_slots[i].ptr->get_collider();
            auto target_origin = target->get_origin();
            auto collider_list = target->get_sphere_collider_list();
            size_t collider_count = target->get_sphere_collider_count();

            for(size_t c = 0; c < collider_count; ++c)
            {
                const sphere_collider& sc = collider_list[c];
                fr::point_3d col_position = target_origin + sc.position;
                int col_radius = sc.radius + LASER_SNAP_DISTANCE; // Adding to radius so laser "snaps" to closer target.
                // <-- Might still require a direction snap
                int64_t col_radius2 = int64_t(col_radius) * col_radius; // Also power of 2 to avoid sqrt.

                // Get collider location translated to laser origin.
                int cp_lo_x = col_position.x().integer() - laser_origin.x().integer();
                int cp_lo_y = col_position.y().integer() - laser_origin.y().integer();
                int cp_lo_z = col_position.z().integer() - laser_origin.z().integer();
                int64_t cp_lo_len2 = int64_t(cp_lo_x)*cp_lo_x + int64_t(cp_lo_y)*cp_lo_y + int64_t(cp_lo_z)*cp_lo_z; // squared distance from laser start to collider position.

                // Calculate projection of collider position (translated to laser origin) onto laser vector, to get it's parallel component.
                int64_t raw_proj_cp_lv = int64_t(cp_lo_x)*lvx + int64_t(cp_lo_y)*lvy + int64_t(cp_lo_z)*lvz; // formula is usually (cp_lo·lv)/|lv|, but we're intentionally skipping division. 

                // Now we can check a couple of scenarios:
                bool hit = false;
                int64_t clamped_proj_dist = 0;
                if(raw_proj_cp_lv <= 0)
                {
                    // Closest point with laser is origin. Check if collider sphere intersects it.
                    if(cp_lo_len2 <= col_radius2)
                    {
                        hit = true;
                        clamped_proj_dist = 0;
                    }
                }
                else if(raw_proj_cp_lv >= laser_len2)
                {
                    // Closest point with laser is target point. Check if collider sphere intersects it.
                    int cp_lt_x = col_position.x().integer() - laser_target.x().integer();
                    int cp_lt_y = col_position.y().integer() - laser_target.y().integer();
                    int cp_lt_z = col_position.z().integer() - laser_target.z().integer();
                    int64_t cp_lt_len2 = int64_t(cp_lt_x)*cp_lt_x + int64_t(cp_lt_y)*cp_lt_y + int64_t(cp_lt_z)*cp_lt_z;
                    if(cp_lt_len2 <= col_radius2)
                    {
                        hit = true;
                        clamped_proj_dist = laser_len2;
                    }
                }
                else
                {
                    // Closest point lies between laser origin and target. Calculate point.

                    // Now, we have a rectangle triangle with:
                    // - Hypothenuse: cp_lo_len (distance from laser origin to collider center). We currently have cp_lo_len2 as the squared version.
                    // - Adjacent cathetus: raw_proj_cp_lv / |laser_vec| (distance from laser origin to perpendicular foot). Intentionally skipping division by |laser_vec| for now.
                    // - We want to find the Opposite cathetus: perpendicular distance from collider center to laser.
                    // Considering H^2 = Ca^2 + Co^2 => Co^2 = H^2 - Ca^2
                    // By multiplying both sides by |laser_vec|^2, we avoid the division. This leads to:
                    // Co^2 * |laser_vec|^2 = H^2 * |laser_vec|^2 - Ca^2 * |laser_vec|^2
                    // Co^2 * |laser_vec|^2 = cp_lo_len2 * |laser_vec|^2 - raw_proj_cp_lv^2
                    // We can then compare Co^2 * |laser_vec|^2 with r^2 * |laser_vec|^2 to determine if intersection happened!
                    
                    int64_t dist_collider_laser_scaled = cp_lo_len2 * laser_len2 - raw_proj_cp_lv * raw_proj_cp_lv;
                    if(dist_collider_laser_scaled <= col_radius2 * laser_len2)
                    {
                        hit = true;
                        clamped_proj_dist = raw_proj_cp_lv; // projection (still scaled)
                    }
                }

                if(!hit)
                {
                    continue;
                }

                // Choose the nearest forward hit (smallest clamped_proj_dist)
                if(!found_hit || clamped_proj_dist < clamped_closest_proj_dist)
                {
                    found_hit = true;
                    // Even though values were scaled/squared, clamped_closest_proj_dist will be in the interval:
                    // 0 <  proj * |laser_vec| < |laser_vec|^2
                    clamped_closest_proj_dist = clamped_proj_dist;
                    best_enemy_index = i;
                    // best_is_endpoint = (clamped_proj_dist == 0 || clamped_proj_dist == laser_len2);
                }
            }
        }

        if(found_hit)
        {
            // <-- Check what to do and how to optimize here
            // --- Truncate beam cleanly using a scaled fraction ---
            // We want t = clamped_closest_proj_dist / laser_len2  (0..1) in fixed.
            // Instead of iterative shifting, compute a high-precision fraction first:
            // Use 8 fractional bits (scale 256) matching bn::fixed internal granularity.
            int64_t num = clamped_closest_proj_dist;
            int64_t den = laser_len2;
            if(num < 0) num = 0;        // guard
            if(num > den) num = den;    // clamp
            int64_t scaled = (den > 0) ? (num * 256) / den : 0; // 0..256
            // bn::fixed constructed from int holds the integer part; divide by 256 to reintroduce fraction.
            bn::fixed t = bn::fixed(int(scaled)) / 256;  // precise within 1/256
            fr::point_3d hit_offset = laser_vec * t;
            laser_target = laser_origin + hit_offset;

            BN_LOG("Laser hit: enemy=" + bn::to_string<64>(best_enemy_index) + " t=" + bn::to_string<64>(t));
            enemy_slots[best_enemy_index].ptr->handle_laser_hit();
        }
    }
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
        laser_vertices[2].reset(laser_target + fr::point_3d(3, 0, 0)); // <-- magic numbers
        laser_vertices[3].reset(player_ship_pos + fr::point_3d(-10, 0, 2));
        laser_vertices[4].reset(player_ship_pos + fr::point_3d(-10, 0, -2));
        laser_vertices[5].reset(laser_target + fr::point_3d(-3, 0, 0));

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
