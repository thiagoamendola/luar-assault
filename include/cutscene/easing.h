#ifndef CUTSCENE_EASING_H
#define CUTSCENE_EASING_H

#include "bn_fixed.h"

enum class easing
{
    LINEAR,
    EASE_IN,
    EASE_OUT,
    EASE_IN_OUT
};

/**
 * Maps a normalised time value t in [0, 1] through the chosen easing curve.
 * All curves are quadratic (cheap on GBA) and guarantee f(0)=0, f(1)=1.
 *
 *  LINEAR       : t
 *  EASE_IN      : t²              (slow start, fast end)
 *  EASE_OUT     : t(2 - t)        (fast start, slow end)
 *  EASE_IN_OUT  : 2t²             if t < 0.5
 *                 -1 + (4 - 2t)t  if t >= 0.5
 */
inline bn::fixed apply_easing(bn::fixed t, easing e)
{
    switch(e)
    {
        case easing::EASE_IN:
            return t * t;

        case easing::EASE_OUT:
            return t * (bn::fixed(2) - t);

        case easing::EASE_IN_OUT:
            if(t < (bn::fixed(1) / 2))
                return bn::fixed(2) * t * t;
            else
                return bn::fixed(-1) + (bn::fixed(4) - bn::fixed(2) * t) * t;

        case easing::LINEAR:
        default:
            return t;
    }
}

#endif // CUTSCENE_EASING_H
