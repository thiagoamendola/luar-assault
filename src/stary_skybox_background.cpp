#include "stary_skybox_background.h"

#include "bn_math.h"
#include "bn_random.h"

#include "fr_sin_cos.h"

stary_skybox_background::stary_skybox_background() :
    _bg(bn::palette_bitmap_bg_ptr::create(palette_item)),
    _painter(_bg)
{
    _bg.set_priority(3); // Behind everything
    _init_stars();
}

void stary_skybox_background::_init_stars()
{
    bn::random random;

    // Distribute NUM_STARS roughly uniformly on the unit sphere.
    // fr::sin/cos use 65536 angle units = 360 degrees.
    // So 16384 = 90 degrees, 32768 = 180 degrees, etc.

    constexpr int FULL_CIRCLE = 65536;
    constexpr int QUARTER = 16384;  // 90 degrees

    constexpr int BANDS = 8;   // elevation bands
    constexpr int PER_BAND = (NUM_STARS + BANDS - 1) / BANDS;

    int created = 0;
    for (int band = 0; band < BANDS && created < NUM_STARS; ++band)
    {
        // Elevation from -90° to +90° => -16384 to +16384
        int theta_center = -QUARTER + (2 * QUARTER * band + QUARTER) / BANDS;
        int theta_jitter_range = 2 * QUARTER / BANDS;

        for (int i = 0; i < PER_BAND && created < NUM_STARS; ++i)
        {
            // Jittered elevation
            int theta_angle = theta_center + random.get_int(theta_jitter_range) - theta_jitter_range / 2;
            if (theta_angle < -QUARTER) theta_angle = -QUARTER;
            if (theta_angle > QUARTER) theta_angle = QUARTER;

            // Random azimuth across full 360 degrees (0..65535)
            int phi_angle = random.get_int(FULL_CIRCLE);

            // Convert spherical to cartesian (unit sphere)
            bn::fixed cos_theta = fr::cos(theta_angle);
            bn::fixed sin_theta = fr::sin(theta_angle);
            bn::fixed cos_phi   = fr::cos(phi_angle);
            bn::fixed sin_phi   = fr::sin(phi_angle);

            star s;
            s.x = cos_theta * cos_phi;
            s.y = cos_theta * sin_phi;
            s.z = sin_theta;
            _stars.push_back(s);
            ++created;
        }
    }
}

void stary_skybox_background::_draw_star(int sx, int sy, int color_index)
{
    // 3x3 cross pattern
    _painter.plot(sx, sy, color_index);         // center
    // _painter.plot(sx - 1, sy, color_index);     // left
    // _painter.plot(sx + 1, sy, color_index);     // right
    // _painter.plot(sx, sy - 1, color_index);     // top
    // _painter.plot(sx, sy + 1, color_index);     // bottom
}

void stary_skybox_background::update(const fr::camera_3d& camera)
{
    // Camera basis vectors:
    //   u = right   (screen +X)
    //   v = up      (screen -Y, since screen Y grows downward)
    //   w = forward (into screen / depth)
    const fr::point_3d& cam_right   = camera.u();
    const fr::point_3d& cam_up      = camera.v();
    const fr::point_3d& cam_forward = camera.w();

    // We perform all dot-product math in raw integer with extended precision.
    // bn::fixed is 20.12 (12 fractional bits). We upshift star and camera
    // values to 20-bit fractional before multiplying, then downshift the
    // result so we end up with a high-precision integer screen coordinate.
    //
    // star components are unit-sphere values in [-1, 1] stored as 20.12.
    // camera basis vectors are also 20.12.
    //
    // Strategy:
    //   dot = sum of (star_component * cam_component)
    //   Each product: raw(star) * raw(cam) gives a 40.24 result.
    //   Sum three of those, then shift right by 12 to get 40.12.
    //   Multiply by PROJ_SCALE (integer), shift right by 12 to get screen pixels.

    constexpr int FRAC_BITS = 12; // bn::fixed fractional bits
    constexpr int PROJ_SCALE = 200;

    // Extract raw integer representations of camera basis vectors once
    int ru_x = cam_right.x().data();
    int ru_y = cam_right.y().data();
    int ru_z = cam_right.z().data();
    int rv_x = cam_up.x().data();
    int rv_y = cam_up.y().data();
    int rv_z = cam_up.z().data();
    int rw_x = cam_forward.x().data();
    int rw_y = cam_forward.y().data();
    int rw_z = cam_forward.z().data();

    _painter.clear();

    for (const star& s : _stars)
    {
        int sx = s.x.data();
        int sy = s.y.data();
        int sz = s.z.data();

        // Dot product with forward vector (result in 24-bit fractional)
        // Each multiplication is 12.12 * 12.12 => result has 24 fractional bits
        int dot_w_raw = (sx * rw_x + sy * rw_y + sz * rw_z);
        // Shift right by FRAC_BITS to get back to 12-bit fractional
        int dot_w = dot_w_raw >> FRAC_BITS;

        // Skip stars behind the camera (dot_w <= 0 in 12-bit fixed)
        if (dot_w <= 0)
        {
            continue;
        }

        // Dot products with right and up vectors (keep in 24-bit fractional for precision)
        int dot_u_raw = (sx * ru_x + sy * ru_y + sz * ru_z);
        int dot_v_raw = (sx * rv_x + sy * rv_y + sz * rv_z);

        // Scale to screen pixels:
        // dot_u_raw has 24 fractional bits, multiply by PROJ_SCALE,
        // then shift right by 24 to get integer screen offset.
        // We do this in two steps to avoid overflow:
        //   first shift right by FRAC_BITS (now 12 frac bits),
        //   multiply by scale, shift right by remaining FRAC_BITS.
        int screen_x = -(((dot_u_raw >> FRAC_BITS) * PROJ_SCALE) >> FRAC_BITS) + 120;
        int screen_y =  (((dot_v_raw >> FRAC_BITS) * PROJ_SCALE) >> FRAC_BITS) + 80;

        // Bounds check (with margin for 3x3 star)
        if (screen_x >= -1 && screen_x <= 240 && screen_y >= -1 && screen_y <= 160)
        {
            _draw_star(screen_x, screen_y, 1);
        }
    }

    _painter.flip_page_later();
}
