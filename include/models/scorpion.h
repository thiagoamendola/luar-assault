
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
             vertex_3d(-5.73172,-2.77005,0.0),
             vertex_3d(-5.73172,3.17561,0.0),
             vertex_3d(-23.95683,-0.0,0.0),
             vertex_3d(-1.71792,-0.0,11.0606),
             vertex_3d(-1.71792,-0.0,-11.0606),
             vertex_3d(5.42192,0.033800000000000004,-3.11374),
             vertex_3d(5.42192,0.033800000000000004,3.11375),
             vertex_3d(-1.58854,1.5878,5.70144),
             vertex_3d(-1.58854,1.5878100000000002,-5.70144),
             vertex_3d(-1.58854,-1.38502,-5.70144),
             vertex_3d(-1.58854,-1.38502,5.70144),
             vertex_3d(-12.83738,-0.0,3.14408),
             vertex_3d(-4.64089,2.38171,2.86457),
             vertex_3d(-4.64089,-2.07754,2.86456),
             vertex_3d(-12.83738,-0.0,-3.14407),
             vertex_3d(-4.64089,2.38171,-2.86456),
             vertex_3d(-4.64089,-2.07753,-2.86457),
             vertex_3d(-14.844280000000001,1.28873,0.0),
             vertex_3d(-14.585189999999999,-1.12533,0.0),
             vertex_3d(0.0,-3.13359,-0.0),
             vertex_3d(-0.0,3.13359,0.0),
             vertex_3d(-0.0,0.0,-3.13359),
             vertex_3d(0.0,0.0,3.13359),
             vertex_3d(-3.13359,-0.0,0.0),
             vertex_3d(3.13359,0.0,-0.0),
       };
    constexpr inline bn::color scorpion_colors[] = {
             bn::color(24,0,1),
             bn::color(24,18,5),
    };
    constexpr inline int scorpion_color_0 = 0;
    constexpr inline int scorpion_color_1 = 1;

    constexpr inline face_3d scorpion_faces_full[] = {
        face_3d(scorpion_vertices, vertex_3d(0.2789,-0.9281,-0.2466),5,9,4,1,1),
        face_3d(scorpion_vertices, vertex_3d(-0.1840,-0.9352,-0.3027),16,14,4,9,1,2),
        face_3d(scorpion_vertices, vertex_3d(-0.1840,-0.9352,0.3027),11,13,10,3,1,2),
        face_3d(scorpion_vertices, vertex_3d(-0.2068,0.9173,-0.3403),15,8,4,14,1,7),
        face_3d(scorpion_vertices, vertex_3d(0.3044,0.9113,-0.2773),4,8,5,1,6),
        face_3d(scorpion_vertices, vertex_3d(0.2789,-0.9281,0.2466),6,3,10,1,1),
        face_3d(scorpion_vertices, vertex_3d(-0.3463,-0.0000,0.9381),5,8,9,1,2),
        face_3d(scorpion_vertices, vertex_3d(0.9345,-0.0000,0.3559),16,15,1,0,1,3),
        face_3d(scorpion_vertices, vertex_3d(0.6808,-0.0000,-0.7325),13,12,7,10,1,3),
        face_3d(scorpion_vertices, vertex_3d(-0.3463,-0.0000,-0.9381),10,7,6,1,2),
        face_3d(scorpion_vertices, vertex_3d(0.9345,-0.0000,-0.3559),0,1,12,13,1,2),
        face_3d(scorpion_vertices, vertex_3d(-0.2068,0.9173,0.3403),12,11,3,7,1,7),
        face_3d(scorpion_vertices, vertex_3d(-0.1099,-0.9149,0.3885),2,18,11,1,2),
        face_3d(scorpion_vertices, vertex_3d(0.6808,-0.0000,0.7325),9,8,15,16,1,4),
        face_3d(scorpion_vertices, vertex_3d(-0.2114,0.8846,-0.4156),17,1,15,14,1,5),
        face_3d(scorpion_vertices, vertex_3d(-0.1927,-0.9103,-0.3664),0,18,14,16,1,3),
        face_3d(scorpion_vertices, vertex_3d(-0.2114,0.8846,0.4156),17,11,12,1,1,4),
        face_3d(scorpion_vertices, vertex_3d(0.3044,0.9113,0.2773),3,6,7,1,6),
        face_3d(scorpion_vertices, vertex_3d(-0.1255,0.8873,0.4438),2,11,17,1,5),
        face_3d(scorpion_vertices, vertex_3d(-0.1255,0.8873,-0.4438),2,17,14,1,6),
        face_3d(scorpion_vertices, vertex_3d(-0.1099,-0.9149,-0.3885),14,18,2,1,4),
        face_3d(scorpion_vertices, vertex_3d(-0.1927,-0.9103,0.3664),11,18,0,13,1,4),
        face_3d(scorpion_vertices, vertex_3d(0.5774,0.5774,0.5774),20,22,24,0,7),
        face_3d(scorpion_vertices, vertex_3d(0.5774,-0.5774,-0.5774),19,21,24,0,2),
        face_3d(scorpion_vertices, vertex_3d(-0.5774,-0.5774,-0.5774),21,19,23,0,1),
        face_3d(scorpion_vertices, vertex_3d(0.5774,-0.5774,0.5774),24,22,19,0,1),
        face_3d(scorpion_vertices, vertex_3d(-0.5774,0.5774,-0.5774),20,21,23,0,7),
        face_3d(scorpion_vertices, vertex_3d(-0.5774,0.5774,0.5774),22,20,23,0,5),
        face_3d(scorpion_vertices, vertex_3d(-0.5774,-0.5774,0.5774),22,23,19,0,2),
        face_3d(scorpion_vertices, vertex_3d(0.5774,0.5774,-0.5774),21,20,24,0,5),
    };
    constexpr inline model_3d_item scorpion_full(scorpion_vertices, scorpion_faces_full, scorpion_colors);
    };
#endif // FR_MODEL_3D_ITEMS_SCORPION_H