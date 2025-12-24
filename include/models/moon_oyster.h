
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
             vertex_3d(0.0,-9.1248,10.735980000000001),
             vertex_3d(0.0,9.1248,10.735980000000001),
             vertex_3d(9.1248,0.0,10.735980000000001),
             vertex_3d(-9.1248,0.0,10.735980000000001),
             vertex_3d(0.0,0.0,1.6111799999999998),
             vertex_3d(0.0,0.0,19.86078),
       };
    constexpr inline bn::color moon_oyster_colors[] = {
             bn::color(24,0,1),
             bn::color(24,24,24),
    };
    constexpr inline int moon_oyster_color_0 = 0;
    constexpr inline int moon_oyster_color_1 = 1;

    constexpr inline face_3d moon_oyster_faces_full[] = {
        face_3d(moon_oyster_vertices, vertex_3d(0.6038,-0.7277,-0.3254),0,6,8,1,1),
        face_3d(moon_oyster_vertices, vertex_3d(-0.6038,-0.7277,-0.3254),7,0,8,1,0),
        face_3d(moon_oyster_vertices, vertex_3d(-0.7174,-0.0000,0.6967),5,6,11,1,4),
        face_3d(moon_oyster_vertices, vertex_3d(-0.0000,-0.0000,1.0000),4,7,6,5,1,2),
        face_3d(moon_oyster_vertices, vertex_3d(0.6038,0.7277,-0.3254),5,1,9,1,7),
        face_3d(moon_oyster_vertices, vertex_3d(-0.0000,0.7174,0.6967),6,7,8,1,5),
        face_3d(moon_oyster_vertices, vertex_3d(-0.0000,-0.7174,0.6967),4,5,9,1,3),
        face_3d(moon_oyster_vertices, vertex_3d(-0.6038,0.7277,-0.3254),1,4,9,1,7),
        face_3d(moon_oyster_vertices, vertex_3d(0.7174,-0.0000,0.6967),7,4,10,1,4),
        face_3d(moon_oyster_vertices, vertex_3d(-0.7277,0.6038,-0.3254),4,3,10,1,5),
        face_3d(moon_oyster_vertices, vertex_3d(0.7277,0.6038,-0.3254),11,2,5,1,5),
        face_3d(moon_oyster_vertices, vertex_3d(0.7277,-0.6038,-0.3254),11,6,2,1,1),
        face_3d(moon_oyster_vertices, vertex_3d(-0.7277,-0.6038,-0.3254),7,10,3,1,1),
        face_3d(moon_oyster_vertices, vertex_3d(0.5972,0.4956,-0.6307),12,5,2,1,6),
        face_3d(moon_oyster_vertices, vertex_3d(0.4956,0.5972,-0.6307),1,5,12,1,5),
        face_3d(moon_oyster_vertices, vertex_3d(-0.4956,0.5972,-0.6307),1,12,4,1,5),
        face_3d(moon_oyster_vertices, vertex_3d(-0.5972,0.4956,-0.6307),12,3,4,1,6),
        face_3d(moon_oyster_vertices, vertex_3d(-0.5972,-0.4956,-0.6307),12,7,3,1,5),
        face_3d(moon_oyster_vertices, vertex_3d(-0.4956,-0.5972,-0.6307),0,7,12,1,2),
        face_3d(moon_oyster_vertices, vertex_3d(0.4956,-0.5972,-0.6307),0,12,6,1,3),
        face_3d(moon_oyster_vertices, vertex_3d(0.5972,-0.4956,-0.6307),12,2,6,1,5),
        face_3d(moon_oyster_vertices, vertex_3d(-0.5774,0.5774,0.5774),14,16,18,0,7),
        face_3d(moon_oyster_vertices, vertex_3d(0.5774,-0.5774,0.5774),13,15,18,0,7),
        face_3d(moon_oyster_vertices, vertex_3d(0.5774,-0.5774,-0.5774),15,13,17,0,7),
        face_3d(moon_oyster_vertices, vertex_3d(-0.5774,-0.5774,0.5774),18,16,13,0,7),
        face_3d(moon_oyster_vertices, vertex_3d(0.5774,0.5774,-0.5774),14,15,17,0,7),
        face_3d(moon_oyster_vertices, vertex_3d(-0.5774,0.5774,-0.5774),16,14,17,0,7),
        face_3d(moon_oyster_vertices, vertex_3d(-0.5774,-0.5774,-0.5774),16,17,13,0,7),
        face_3d(moon_oyster_vertices, vertex_3d(0.5774,0.5774,0.5774),15,14,18,0,7),
    };
    constexpr inline model_3d_item moon_oyster_full(moon_oyster_vertices, moon_oyster_faces_full, moon_oyster_colors);
    };
#endif // FR_MODEL_3D_ITEMS_MOON_OYSTER_H