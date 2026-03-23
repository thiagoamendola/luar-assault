/*
 * Copyright (c) 2020-2024 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef FR_CAMERA_3D_H
#define FR_CAMERA_3D_H

#include "fr_point_3d.h"

namespace fr
{

    class camera_3d
    {

    public:
        camera_3d();

        [[nodiscard]] const point_3d &position() const
        {
            return _position;
        }

        void set_position(const point_3d &position);

        [[nodiscard]] bn::fixed phi() const
        {
            return _phi;
        }

        void set_phi(bn::fixed phi);

        [[nodiscard]] bn::fixed theta() const
        {
            return _theta;
        }

        void set_theta(bn::fixed theta);

        [[nodiscard]] bn::fixed psi() const
        {
            return _psi;
        }

        void set_psi(bn::fixed psi);

        [[nodiscard]] const point_3d &u() const
        {
            return _u;
        }

        [[nodiscard]] const point_3d &v() const
        {
            return _v;
        }

        [[nodiscard]] const point_3d &w() const
        {
            return _w;
        }

    private:
        point_3d _position;
        bn::fixed _phi;
        bn::fixed _phi_sin;
        bn::fixed _phi_cos = 1;
        bn::fixed _theta;
        bn::fixed _theta_sin;
        bn::fixed _theta_cos = 1;
        bn::fixed _psi;
        bn::fixed _psi_sin;
        bn::fixed _psi_cos = 1;
        point_3d _u;
        point_3d _v;
        point_3d _w;

        void _update_uvw();
    };

}

#endif
