
    /*
     * Based on Nikku4211's Wavefront to Varooom Converter (github.com/nikku4211/)
     * Modified to support metadata (engine_scale / engine_brightness)
     * Source OBJ: moon_oyster.obj
     * Metadata: moon_oyster.json
     *
     */

    #ifndef FR_MODEL_3D_ITEMS_MOON_OYSTER_H
    #define FR_MODEL_3D_ITEMS_MOON_OYSTER_H

    #include "fr_model_3d_item.h"

    namespace fr::model_3d_items
    {
        constexpr inline vertex_3d moon_oyster_vertices[] = {
             vertex_3d(0.0,-14.302605,0.0),
             vertex_3d(0.0,14.302605,-0.0),
             vertex_3d(14.302605,0.0,0.0),
             vertex_3d(-14.302605,0.0,0.0),
             vertex_3d(-7.816635000000001,7.816635000000001,-0.0),
             vertex_3d(7.816635000000001,7.816635000000001,-0.0),
             vertex_3d(7.816635000000001,-7.816635000000001,0.0),
             vertex_3d(-7.816635000000001,-7.816635000000001,0.0),
             vertex_3d(0.0,-19.838475,12.37797),
             vertex_3d(0.0,19.838475,12.37797),
             vertex_3d(-19.838475,0.0,12.37797),
             vertex_3d(19.838475,0.0,12.37797),
             vertex_3d(0.0,-0.0,-13.544715),
       };
    constexpr inline bn::color moon_oyster_colors[] = {
             bn::color(24,24,24),
    };
    constexpr inline int moon_oyster_color_0 = 0;

    constexpr inline face_3d moon_oyster_faces_full[] = {
        face_3d(moon_oyster_vertices, vertex_3d(0.6038,-0.7277,-0.3254),0,6,8,0,1),
        face_3d(moon_oyster_vertices, vertex_3d(-0.6038,-0.7277,-0.3254),7,0,8,0,0),
        face_3d(moon_oyster_vertices, vertex_3d(-0.7174,-0.0000,0.6967),5,6,11,0,4),
        face_3d(moon_oyster_vertices, vertex_3d(-0.0000,-0.0000,1.0000),4,7,6,5,0,2),
        face_3d(moon_oyster_vertices, vertex_3d(0.6038,0.7277,-0.3254),5,1,9,0,7),
        face_3d(moon_oyster_vertices, vertex_3d(-0.0000,0.7174,0.6967),6,7,8,0,5),
        face_3d(moon_oyster_vertices, vertex_3d(-0.0000,-0.7174,0.6967),4,5,9,0,3),
        face_3d(moon_oyster_vertices, vertex_3d(-0.6038,0.7277,-0.3254),1,4,9,0,7),
        face_3d(moon_oyster_vertices, vertex_3d(0.7174,-0.0000,0.6967),7,4,10,0,4),
        face_3d(moon_oyster_vertices, vertex_3d(-0.7277,0.6038,-0.3254),4,3,10,0,5),
        face_3d(moon_oyster_vertices, vertex_3d(0.7277,0.6038,-0.3254),11,2,5,0,5),
        face_3d(moon_oyster_vertices, vertex_3d(0.7277,-0.6038,-0.3254),11,6,2,0,1),
        face_3d(moon_oyster_vertices, vertex_3d(-0.7277,-0.6038,-0.3254),7,10,3,0,1),
        face_3d(moon_oyster_vertices, vertex_3d(0.5972,0.4956,-0.6307),12,5,2,0,6),
        face_3d(moon_oyster_vertices, vertex_3d(0.4956,0.5972,-0.6307),1,5,12,0,5),
        face_3d(moon_oyster_vertices, vertex_3d(-0.4956,0.5972,-0.6307),1,12,4,0,5),
        face_3d(moon_oyster_vertices, vertex_3d(-0.5972,0.4956,-0.6307),12,3,4,0,6),
        face_3d(moon_oyster_vertices, vertex_3d(-0.5972,-0.4956,-0.6307),12,7,3,0,5),
        face_3d(moon_oyster_vertices, vertex_3d(-0.4956,-0.5972,-0.6307),0,7,12,0,2),
        face_3d(moon_oyster_vertices, vertex_3d(0.4956,-0.5972,-0.6307),0,12,6,0,3),
        face_3d(moon_oyster_vertices, vertex_3d(0.5972,-0.4956,-0.6307),12,2,6,0,5),
    };
    constexpr inline model_3d_item moon_oyster_full(moon_oyster_vertices, moon_oyster_faces_full, moon_oyster_colors);
    };
#endif // FR_MODEL_3D_ITEMS_MOON_OYSTER_H