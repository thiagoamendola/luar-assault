#ifndef COLLIDERS_H
#define COLLIDERS_H

#include "bn_array.h"
#include "bn_colors.h"
#include "bn_fixed.h"
#include "bn_log.h"
#include "bn_span.h"
#include "bn_string.h"
#include "bn_vector.h"

#include "fr_constants_3d.h"
#include "fr_model_3d_item.h"
#include "fr_models_3d.h"
#include "fr_point_3d.h"
#include "fr_sin_cos.h"

namespace fr::model_3d_items
{

constexpr inline bn::color debug_collider_colors[] = {
    bn::color(0, 0, 24),
};

}

// = Collider Debugging Classes

struct sphere_collider_debugger
{
    fr::vertex_3d debug_vertices[4] = {
        fr::vertex_3d(1, 0, 0),
        fr::vertex_3d(0, 1, 0),
        fr::vertex_3d(0, 0, 1),
        fr::vertex_3d(1, 1, 0),
    };

    fr::face_3d debug_faces[2] = {
        fr::face_3d(debug_vertices, fr::vertex_3d(0, 1, 0), 0, 1, 2, 0, 7),
        fr::face_3d(debug_vertices, fr::vertex_3d(0, 1, 0), 2, 3, 0, 0, 7),
    };

    fr::model_3d_item debug_model;

    constexpr sphere_collider_debugger()
        : debug_model(debug_vertices, debug_faces,
                      fr::model_3d_items::debug_collider_colors)
    {
    }
};

// = Colliders

struct sphere_collider
{
    fr::point_3d position;
    int radius;

    constexpr sphere_collider()
        : position(fr::point_3d(0, 0, 0)), radius(0)
    {
    }

    constexpr sphere_collider(fr::point_3d _position, int _radius)
        : position(_position), radius(_radius)
    {
    }

    int squared_radius(){
        return radius*radius;
    }
};

class sphere_collider_set
{
  public:
    sphere_collider_set(const bn::span<const sphere_collider> &sphere_colliders)
        : _sphere_collider_list(sphere_colliders)
    {
    }

    void set_origin(fr::point_3d origin_pos)
    {
        _origin_pos = origin_pos;
    }

    const fr::point_3d get_origin() const
    {
        return _origin_pos;
    }

    // Stores parent object's rotation for lazy rebuild later.
    void set_rotation(bn::fixed phi, bn::fixed theta, bn::fixed psi)
    {
        bn::fixed rel_phi = phi - _initial_phi;
        bn::fixed rel_theta = theta - _initial_theta;
        bn::fixed rel_psi = psi - _initial_psi;

        if (rel_phi != _phi || rel_theta != _theta || rel_psi != _psi)
        {
            _phi = rel_phi;
            _theta = rel_theta;
            _psi = rel_psi;
            _rotation_dirty = true;
        }
    }

    // Sets the reference (zero) rotation.
    void set_initial_rotation(bn::fixed phi, bn::fixed theta, bn::fixed psi)
    {
        _initial_phi = phi;
        _initial_theta = theta;
        _initial_psi = psi;
        _rotation_dirty = true;
    }


    const bn::span<const sphere_collider> get_sphere_collider_list() const
    {
        return _sphere_collider_list;
    }

    size_t get_sphere_collider_count() const
    {
        return _sphere_collider_list.size();
    }

    int debug_collider(const fr::model_3d_item **static_model_items,
                       int static_count)
    {
        size_t collider_count = _sphere_collider_list.size();
        
        // Ensure we don't exceed array size
        if (collider_count > _sphere_collider_debuggers.size())
        {
            BN_LOG("[debug_collider] Too many colliders: " + 
                bn::to_string<32>(collider_count) + " (max: " + 
                bn::to_string<32>(_sphere_collider_debuggers.size()) + ")");
            collider_count = _sphere_collider_debuggers.size();
        }

        _update_rotation_matrix();

        for (size_t i = 0; i < collider_count; i++)
        {
            auto collider = _sphere_collider_list[i];
            auto& debugger = _sphere_collider_debuggers[i];

            // Rotate the local collider offset and its radius axes by the
            // parent object's rotation so the debug wireframe follows the model.
            fr::point_3d rotated_offset = _rotate(collider.position);
            fr::point_3d rx = _rotate(fr::point_3d(collider.radius, 0, 0));
            fr::point_3d rz = _rotate(fr::point_3d(0, 0, collider.radius));
            fr::point_3d center = _origin_pos + rotated_offset;

            // Calc vertices
            debugger.debug_vertices[0].reset(center + rx);
            debugger.debug_vertices[1].reset(center + rz);
            debugger.debug_vertices[2].reset(center - rx);
            debugger.debug_vertices[3].reset(center - rz);

            // Calc faces
            debugger.debug_faces[0].reset(
                debugger.debug_vertices, fr::vertex_3d(0, 1, 0), 2, 1,
                0, 0, 7);
            debugger.debug_faces[1].reset(
                debugger.debug_vertices, fr::vertex_3d(0, 1, 0), 0, 3,
                2, 0, 7);

            // Add mesh as static object
            if (static_count >= fr::constants_3d::max_static_models)
            {
                BN_LOG("[debug_collider] Stage Section Renderer: reached static model max limit: " +
                    bn::to_string<64>(fr::constants_3d::max_static_models));
                return static_count;
            }
            static_model_items[static_count] = &debugger.debug_model;
            static_count += 1;
        }

        return static_count;
    }

    bool colliding_with_statics(const fr::model_3d_item **static_model_items, size_t count)
    {
        _update_rotation_matrix();
        for (size_t i = 0; i < count ; i++)
        {
            if (colliding_with_static_model(*static_model_items[i]))
            {
                return true;
            }
        }

        return false;
    }

    bool colliding_with_static_colliders(const sphere_collider *static_colliders, size_t static_collider_count)
    {
        size_t collider_count = _sphere_collider_list.size();
        _update_rotation_matrix();

        for (size_t i = 0; i < collider_count; i++)
        {
            auto this_collider = _sphere_collider_list[i];
            fr::point_3d this_center = _origin_pos + _rotate(this_collider.position);

            for (size_t j = 0; j < static_collider_count; j++)
            {
                auto static_col = static_colliders[j];

                fr::point_3d dist_vec = this_center - static_col.position;
                int xv = dist_vec.x().integer();
                int yv = dist_vec.y().integer();
                int zv = dist_vec.z().integer();
                int dist_squared = (xv * xv) + (yv * yv) + (zv * zv);
                int radii_sum = this_collider.radius + static_col.radius;
                int radii_sum_squared = radii_sum * radii_sum;

                if (dist_squared <= radii_sum_squared)
                {
                    BN_LOG("[colliding_with_static_colliders] COLLIDED!!!");
                    return true;
                }
            }
        }

        return false;
    }

    bool colliding_with_dynamic(sphere_collider_set* target)
    {
        // BN_LOG("[collision] STARTING ----------------- ");

        size_t collider_count = _sphere_collider_list.size();
        _update_rotation_matrix();
        target->_update_rotation_matrix();

        for (size_t i = 0; i < collider_count; i++)
        {
            auto this_collider = _sphere_collider_list[i];
            fr::point_3d this_center = _origin_pos + _rotate(this_collider.position);

            auto target_origin = target->get_origin();
            auto target_collider_list = target->get_sphere_collider_list();
            auto target_collider_count = target->get_sphere_collider_count();
            
            for (size_t j = 0; j < target_collider_count; j++)
            {
                auto target_collider = target_collider_list[j];
                fr::point_3d target_center = target_origin + target->_rotate(target_collider.position);
                
                fr::point_3d collider_center_distance_vec = 
                    this_center - target_center;
                int xv = collider_center_distance_vec.x().integer();
                int yv = collider_center_distance_vec.y().integer();
                int zv = collider_center_distance_vec.z().integer();
                int collider_center_distance_squared = 
                    (xv * xv) + 
                    (yv * yv) + 
                    (zv * zv);

                if (collider_center_distance_squared <= this_collider.squared_radius() + target_collider.squared_radius())
                {
                    BN_LOG("[colliding_with_dynamic] vertex dist (squared): " +
                        bn::to_string<128>(collider_center_distance_squared) + 
                        "; squared_radius: "+
                        bn::to_string<128>(this_collider.squared_radius()) + 
                        "; raw dist vec: (" + 
                        bn::to_string<128>(collider_center_distance_vec.x()) + ", " +
                        bn::to_string<128>(collider_center_distance_vec.y()) + ", " +
                        bn::to_string<128>(collider_center_distance_vec.z()) + ")"
                        );
                    BN_LOG("[colliding_with_dynamic] COLLIDED!!!");
                    return true;
                }
            }
        }

        return false;
    }

  private:
    const bn::span<const sphere_collider> _sphere_collider_list;
    bn::array<sphere_collider_debugger, 8> _sphere_collider_debuggers;  // Max 8 colliders per entity
    fr::point_3d _origin_pos;

    // Parent rotation state (Euler angles in fr-lib encoding) and the cached
    // 3x3 rotation matrix. Rebuilt only when angles change.
    bn::fixed _phi = 0;
    bn::fixed _theta = 0;
    bn::fixed _psi = 0;
    bn::fixed _initial_phi = 0;
    bn::fixed _initial_theta = 0;
    bn::fixed _initial_psi = 0;
    bn::fixed _xx = 1, _xy = 0, _xz = 0;
    bn::fixed _yx = 0, _yy = 1, _yz = 0;
    bn::fixed _zx = 0, _zy = 0, _zz = 1;
    bool _rotation_dirty = false;

    // Rebuilds the cached 3x3 rotation matrix from the current Euler angles
    // (_phi, _theta, _psi). The matrix follows the same Z-Y-X composition from fr::model_3d:
    //   R = Rz(phi) * Ry(theta) * Rx(psi)
    // Angles use the fr-lib encoding (right_shift_integer() of a bn::fixed
    // gives the integer index passed to fr::sin/cos). Once built, _rotate()
    // applies it to a local-space point as `R * v`.
    void _update_rotation_matrix()
    {
        if (!_rotation_dirty)
        {
            return;
        }
        _rotation_dirty = false;

        int phi_angle = _phi.right_shift_integer();
        int theta_angle = _theta.right_shift_integer();
        int psi_angle = _psi.right_shift_integer();

        // Precompute the trig terms used by the rotation matrix below.
        // Each Euler angle contributes a sin/cos pair, and the two cross-terms
        // (phi_cos*theta_sin, phi_sin*theta_sin) appear in multiple matrix
        // entries — caching them avoids redoing the same multiplication.
        bn::fixed phi_sin = fr::sin(phi_angle);
        bn::fixed phi_cos = fr::cos(phi_angle);
        bn::fixed theta_sin = fr::sin(theta_angle);
        bn::fixed theta_cos = fr::cos(theta_angle);
        bn::fixed psi_sin = fr::sin(psi_angle);
        bn::fixed psi_cos = fr::cos(psi_angle);
        bn::fixed phi_cos_theta_sin = phi_cos.unsafe_multiplication(theta_sin);
        bn::fixed phi_sin_theta_sin = phi_sin.unsafe_multiplication(theta_sin);

        // Rebuild rotation matrix.
        _xx = phi_cos.unsafe_multiplication(theta_cos);
        _xy = phi_cos_theta_sin.unsafe_multiplication(psi_sin) -
              phi_sin.unsafe_multiplication(psi_cos);
        _xz = phi_cos_theta_sin.unsafe_multiplication(psi_cos) +
              phi_sin.unsafe_multiplication(psi_sin);

        _yx = phi_sin.unsafe_multiplication(theta_cos);
        _yy = phi_sin_theta_sin.unsafe_multiplication(psi_sin) +
              phi_cos.unsafe_multiplication(psi_cos);
        _yz = phi_sin_theta_sin.unsafe_multiplication(psi_cos) -
              phi_cos.unsafe_multiplication(psi_sin);

        _zx = -theta_sin;
        _zy = theta_cos.unsafe_multiplication(psi_sin);
        _zz = theta_cos.unsafe_multiplication(psi_cos);
    }

    // Applies the cached rotation matrix to a local-space point.
    fr::point_3d _rotate(const fr::point_3d &v) const
    {
        return fr::point_3d(
            _xx.safe_multiplication(v.x()) + _xy.safe_multiplication(v.y()) + _xz.safe_multiplication(v.z()),
            _yx.safe_multiplication(v.x()) + _yy.safe_multiplication(v.y()) + _yz.safe_multiplication(v.z()),
            _zx.safe_multiplication(v.x()) + _zy.safe_multiplication(v.y()) + _zz.safe_multiplication(v.z()));
    }

    bool colliding_with_point(fr::point_3d point)
    {
        size_t collider_count = _sphere_collider_list.size();
        
        for (size_t i = 0; i < collider_count; i++)
        {
            auto collider = _sphere_collider_list[i];
            
            fr::point_3d collider_center_distance_vec = point - (_origin_pos + _rotate(collider.position));
            int xv = collider_center_distance_vec.x().integer();
            int yv = collider_center_distance_vec.y().integer();
            int zv = collider_center_distance_vec.z().integer();
            int collider_center_distance_squared = 
                (xv * xv) + 
                (yv * yv) + 
                (zv * zv);

            if (collider_center_distance_squared <= collider.squared_radius())
            {
                BN_LOG("[colliding_with_point] vertex dist (squared): " +
                    bn::to_string<128>(collider_center_distance_squared) + 
                    "; squared_radius: "+
                    bn::to_string<128>(collider.squared_radius()) + 
                    "; raw dist vec: (" + 
                    bn::to_string<128>(collider_center_distance_vec.x()) + ", " +
                    bn::to_string<128>(collider_center_distance_vec.y()) + ", " +
                    bn::to_string<128>(collider_center_distance_vec.z()) + ")"
                    );
                BN_LOG("[colliding_with_point] COLLIDED!!!");
                return true;
            }
        }

        return false;
    }

    // Current Strategy: check if colliding with any vertex.
    bool colliding_with_static_model(const fr::model_3d_item &model_item)
    {
        // BN_LOG("[colliding_with_static_model] new model: ");

        const auto _vertices = model_item.vertices();

        // <-- Optimization: skip vertices if too far from model

        for (auto& vertex: _vertices)
        {
            if (colliding_with_point(vertex.point()))
            {
                return true;
            }
        }

        return false;
    }


};

#endif
