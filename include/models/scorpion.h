
    /*
     * Based on Nikku4211's Wavefront to Varooom Converter (github.com/nikku4211/)
     * Modified to support metadata (engine_scale / engine_brightness)
     * Source OBJ: scorpion.obj
     * Metadata: scorpion.json
     *
     */

    #ifndef FR_MODEL_3D_ITEMS_SCORPION_H
    #define FR_MODEL_3D_ITEMS_SCORPION_H

    #include "fr_model_3d_item.h"

    namespace fr::model_3d_items
    {
        constexpr inline vertex_3d scorpion_vertices[] = {
             vertex_3d(0.0,-2.77005,-5.73172),
             vertex_3d(0.0,3.17561,-5.73172),
             vertex_3d(0.0,-0.0,-23.95683),
             vertex_3d(-11.0606,-0.0,-1.71792),
             vertex_3d(11.0606,-0.0,-1.71792),
             vertex_3d(3.11374,0.033800000000000004,5.42192),
             vertex_3d(-3.11375,0.033800000000000004,5.42192),
             vertex_3d(-5.70144,1.5878,-1.58854),
             vertex_3d(5.70144,1.5878100000000002,-1.58854),
             vertex_3d(5.70144,-1.38502,-1.58854),
             vertex_3d(-5.70144,-1.38502,-1.58854),
             vertex_3d(-3.14408,-0.0,-12.83738),
             vertex_3d(-2.86457,2.38171,-4.64089),
             vertex_3d(-2.86456,-2.07754,-4.64089),
             vertex_3d(3.14407,-0.0,-12.83738),
             vertex_3d(2.86456,2.38171,-4.64089),
             vertex_3d(2.86457,-2.07753,-4.64089),
             vertex_3d(0.0,1.28873,-14.844280000000001),
             vertex_3d(0.0,-1.12533,-14.585189999999999),
             vertex_3d(0.0,-3.13359,0.0),
             vertex_3d(0.0,3.13359,-0.0),
             vertex_3d(3.13359,0.0,0.0),
             vertex_3d(-3.13359,0.0,0.0),
             vertex_3d(0.0,-0.0,-3.13359),
             vertex_3d(0.0,0.0,3.13359),
       };
    constexpr inline bn::color scorpion_colors[] = {
             bn::color(24,0,1),
             bn::color(24,18,5),
    };
    constexpr inline int scorpion_color_0 = 0;
    constexpr inline int scorpion_color_1 = 1;

    constexpr inline face_3d scorpion_faces_full[] = {
        face_3d(scorpion_vertices, vertex_3d(0.2466,-0.9281,0.2789),5,9,4,1,1),
        face_3d(scorpion_vertices, vertex_3d(0.3027,-0.9352,-0.1840),16,14,4,9,1,2),
        face_3d(scorpion_vertices, vertex_3d(-0.3027,-0.9352,-0.1840),11,13,10,3,1,2),
        face_3d(scorpion_vertices, vertex_3d(0.3403,0.9173,-0.2068),15,8,4,14,1,7),
        face_3d(scorpion_vertices, vertex_3d(0.2773,0.9113,0.3044),4,8,5,1,6),
        face_3d(scorpion_vertices, vertex_3d(-0.2466,-0.9281,0.2789),6,3,10,1,1),
        face_3d(scorpion_vertices, vertex_3d(-0.9381,-0.0000,-0.3463),5,8,9,1,2),
        face_3d(scorpion_vertices, vertex_3d(-0.3559,-0.0000,0.9345),16,15,1,0,1,3),
        face_3d(scorpion_vertices, vertex_3d(0.7325,-0.0000,0.6808),13,12,7,10,1,3),
        face_3d(scorpion_vertices, vertex_3d(0.9381,-0.0000,-0.3463),10,7,6,1,2),
        face_3d(scorpion_vertices, vertex_3d(0.3559,-0.0000,0.9345),0,1,12,13,1,2),
        face_3d(scorpion_vertices, vertex_3d(-0.3403,0.9173,-0.2068),12,11,3,7,1,7),
        face_3d(scorpion_vertices, vertex_3d(-0.3885,-0.9149,-0.1099),2,18,11,1,2),
        face_3d(scorpion_vertices, vertex_3d(-0.7325,-0.0000,0.6808),9,8,15,16,1,4),
        face_3d(scorpion_vertices, vertex_3d(0.4156,0.8846,-0.2114),17,1,15,14,1,5),
        face_3d(scorpion_vertices, vertex_3d(0.3664,-0.9103,-0.1927),0,18,14,16,1,3),
        face_3d(scorpion_vertices, vertex_3d(-0.4156,0.8846,-0.2114),17,11,12,1,1,4),
        face_3d(scorpion_vertices, vertex_3d(-0.2773,0.9113,0.3044),3,6,7,1,6),
        face_3d(scorpion_vertices, vertex_3d(-0.4438,0.8873,-0.1255),2,11,17,1,5),
        face_3d(scorpion_vertices, vertex_3d(0.4438,0.8873,-0.1255),2,17,14,1,6),
        face_3d(scorpion_vertices, vertex_3d(0.3885,-0.9149,-0.1099),14,18,2,1,4),
        face_3d(scorpion_vertices, vertex_3d(-0.3664,-0.9103,-0.1927),11,18,0,13,1,4),
        face_3d(scorpion_vertices, vertex_3d(-0.5774,0.5774,0.5774),20,22,24,0,7),
        face_3d(scorpion_vertices, vertex_3d(0.5774,-0.5774,0.5774),19,21,24,0,2),
        face_3d(scorpion_vertices, vertex_3d(0.5774,-0.5774,-0.5774),21,19,23,0,1),
        face_3d(scorpion_vertices, vertex_3d(-0.5774,-0.5774,0.5774),24,22,19,0,1),
        face_3d(scorpion_vertices, vertex_3d(0.5774,0.5774,-0.5774),20,21,23,0,7),
        face_3d(scorpion_vertices, vertex_3d(-0.5774,0.5774,-0.5774),22,20,23,0,5),
        face_3d(scorpion_vertices, vertex_3d(-0.5774,-0.5774,-0.5774),22,23,19,0,2),
        face_3d(scorpion_vertices, vertex_3d(0.5774,0.5774,0.5774),21,20,24,0,5),
    };
    constexpr inline model_3d_item scorpion_full(scorpion_vertices, scorpion_faces_full, scorpion_colors);
    };
#endif // FR_MODEL_3D_ITEMS_SCORPION_H