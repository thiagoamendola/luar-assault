
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
             vertex_3d(-8.59758,-4.155075,0.0),
             vertex_3d(-8.59758,4.763415,0.0),
             vertex_3d(-35.935245,-0.0,0.0),
             vertex_3d(-2.57688,-0.0,16.5909),
             vertex_3d(-2.57688,-0.0,-16.5909),
             vertex_3d(8.13288,0.0507,-4.67061),
             vertex_3d(8.13288,0.0507,4.670625),
             vertex_3d(-2.38281,2.3817,8.55216),
             vertex_3d(-2.38281,2.3817150000000002,-8.55216),
             vertex_3d(-2.38281,-2.07753,-8.55216),
             vertex_3d(-2.38281,-2.07753,8.55216),
             vertex_3d(-19.25607,-0.0,4.71612),
             vertex_3d(-6.961335,3.572565,4.296855),
             vertex_3d(-6.961335,-3.11631,4.2968399999999995),
             vertex_3d(-19.25607,-0.0,-4.716105),
             vertex_3d(-6.961335,3.572565,-4.2968399999999995),
             vertex_3d(-6.961335,-3.116295,-4.296855),
             vertex_3d(-22.26642,1.9330949999999998,0.0),
             vertex_3d(-21.877785,-1.687995,0.0),
             vertex_3d(0.0,-4.700385,-0.0),
             vertex_3d(-0.0,4.700385,0.0),
             vertex_3d(-0.0,0.0,-4.700385),
             vertex_3d(0.0,0.0,4.700385),
             vertex_3d(-4.700385,-0.0,0.0),
             vertex_3d(4.700385,0.0,-0.0),
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