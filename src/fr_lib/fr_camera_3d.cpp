/*
 * Copyright (c) 2020-2024 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#include "fr_camera_3d.h"

#include "bn_math.h"

#include "fr_sin_cos.h"

namespace fr
{

camera_3d::camera_3d() : _position(0, 256, 0)
{
    set_phi(0);
}

void camera_3d::set_position(const point_3d &position)
{
    // BN_ASSERT(position.y() >= 2, "Invalid y: ", position.y());

    _position = position;
}

void camera_3d::set_phi(bn::fixed phi)
{
    if (phi > 0xFFFF)
    {
        phi -= 0xFFFF;
    }
    else if (phi < 0)
    {
        phi += 0xFFFF;
    }

    BN_ASSERT(phi >= 0 && phi <= 0xFFFF, "Invalid phi: ", phi);

    int angle = phi.right_shift_integer();
    _phi = phi;
    _phi_sin = sin(angle);
    _phi_cos = cos(angle);
    _update_uvw();
}

void camera_3d::set_theta(bn::fixed theta)
{
    if (theta > 0xFFFF)
    {
        theta -= 0xFFFF;
    }
    else if (theta < 0)
    {
        theta += 0xFFFF;
    }

    BN_ASSERT(theta >= 0 && theta <= 0xFFFF, "Invalid theta: ", theta);

    int angle = theta.right_shift_integer();
    _theta = theta;
    _theta_sin = sin(angle);
    _theta_cos = cos(angle);
    _update_uvw();
}

void camera_3d::set_psi(bn::fixed psi)
{
    if (psi > 0xFFFF)
    {
        psi -= 0xFFFF;
    }
    else if (psi < 0)
    {
        psi += 0xFFFF;
    }

    BN_ASSERT(psi >= 0 && psi <= 0xFFFF, "Invalid psi: ", psi);

    int angle = psi.right_shift_integer();
    _psi = psi;
    _psi_sin = sin(angle);
    _psi_cos = cos(angle);
    _update_uvw();
}

void camera_3d::_update_uvw()
{
    bn::fixed sf = _phi_sin,   cf = _phi_cos;
    bn::fixed st = _theta_sin, ct = _theta_cos;
    bn::fixed ss = _psi_sin,   cs = _psi_cos;

    // R = R_Y(phi) . R_X(theta) . R_Z(psi)
    //
    // _u = Row 0 = camera right (screen X axis in world space)
    _u.set_x(cf.unsafe_multiplication(cs) + sf.unsafe_multiplication(st).unsafe_multiplication(ss));
    _u.set_y((-cf).unsafe_multiplication(ss) + sf.unsafe_multiplication(st).unsafe_multiplication(cs));
    _u.set_z(sf.unsafe_multiplication(ct));

    // _w = Row 1 = camera depth axis (world Y when theta=psi=0)
    _w.set_x(ct.unsafe_multiplication(ss));
    _w.set_y(ct.unsafe_multiplication(cs));
    _w.set_z(-st);

    // _v = -Row 2 = camera screen-up axis in world space (sign matches original convention)
    _v.set_x(sf.unsafe_multiplication(cs) - cf.unsafe_multiplication(st).unsafe_multiplication(ss));
    _v.set_y((-sf).unsafe_multiplication(ss) - cf.unsafe_multiplication(st).unsafe_multiplication(cs));
    _v.set_z((-cf).unsafe_multiplication(ct));
}

} // namespace fr
