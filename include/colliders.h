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
        
        for (size_t i = 0; i < collider_count; i++)
        {
            auto collider = _sphere_collider_list[i];
            auto& debugger = _sphere_collider_debuggers[i];

            // Calc vertices
            debugger.debug_vertices[0].reset(
                _origin_pos + collider.position + fr::point_3d(collider.radius, 0, 0));
            debugger.debug_vertices[1].reset(
                _origin_pos + collider.position + fr::point_3d(0, 0, collider.radius));
            debugger.debug_vertices[2].reset(
                _origin_pos + collider.position + fr::point_3d(-collider.radius, 0, 0));
            debugger.debug_vertices[3].reset(
                _origin_pos + collider.position + fr::point_3d(0, 0, -collider.radius));

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
        for (size_t i = 0; i < count ; i++)
        {
            if (colliding_with_static_model(*static_model_items[i]))
            {
                return true;
            }
        }

        return false;
    }

    bool colliding_with_dynamic(sphere_collider_set* target)
    {
        // BN_LOG("[collision] STARTING ----------------- ");

        size_t collider_count = _sphere_collider_list.size();
        
        for (size_t i = 0; i < collider_count; i++)
        {
            auto this_collider = _sphere_collider_list[i];

            auto target_origin = target->get_origin();
            auto target_collider_list = target->get_sphere_collider_list();
            auto target_collider_count = target->get_sphere_collider_count();
            
            for (size_t j = 0; j < target_collider_count; j++)
            {
                auto target_collider = target_collider_list[j];
                
                fr::point_3d collider_center_distance_vec = 
                    (_origin_pos + this_collider.position) - (target_origin + target_collider.position);
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
    
    bool colliding_with_point(fr::point_3d point)
    {
        size_t collider_count = _sphere_collider_list.size();
        
        for (size_t i = 0; i < collider_count; i++)
        {
            auto collider = _sphere_collider_list[i];
            
            fr::point_3d collider_center_distance_vec = point - (_origin_pos + collider.position);
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
