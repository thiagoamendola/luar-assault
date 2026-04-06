#include "bn_math.h"
#include "bn_fixed.h"
#include "bn_fixed_point.h"

#include "fr_point_3d.h"

// Original way of calculating unit vector. Uses square root, so slow.
constexpr bn::fixed_point unit_vector(bn::fixed_point original_vector)
{
    // Uses division, so not super efficient.
    bn::fixed magnitude = bn::sqrt((original_vector.x() * original_vector.x()) + (original_vector.y() * original_vector.y()));
    magnitude = magnitude == 0 ? 1 : magnitude;
    return original_vector / magnitude;
}
// <-- Implement Newton-Raphson optimization

constexpr fr::point_3d unit_vector(fr::point_3d original_vector)
{
    bn::fixed distance_mag = bn::sqrt(
        original_vector.x() * original_vector.x() +
        original_vector.y() * original_vector.y() +
        original_vector.z() * original_vector.z()); // <-- Can be optimized to avoid sqrt?

    // Normalize to unit vector and multiply by movement speed
    if (distance_mag > 0)
    {
        return fr::point_3d(
            (original_vector.x().division(distance_mag)),
            (original_vector.y().division(distance_mag)),
            (original_vector.z().division(distance_mag)));
    }

    return fr::point_3d(0, 0, 0);
}
