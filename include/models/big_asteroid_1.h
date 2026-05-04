
    /*
     * Based on Nikku4211's Wavefront to Varooom Converter (github.com/nikku4211/)
     * Modified to support metadata (engine_scale / engine_brightness)
     * Source OBJ: big_asteroid_1.obj
     * Metadata: big_asteroid_1.json
     *
     */

    #ifndef FR_MODEL_3D_ITEMS_BIG_ASTEROID_1_H
    #define FR_MODEL_3D_ITEMS_BIG_ASTEROID_1_H

    #include "fr_model_3d_item.h"

    namespace fr::model_3d_items
    {
        constexpr inline vertex_3d big_asteroid_1_vertices[] = {
             vertex_3d(-19.57216,20.85656,-35.05644),
             vertex_3d(-19.57216,-20.85656,-35.05644),
             vertex_3d(32.34352,-13.88412,-22.02484),
             vertex_3d(4.15412,37.98388,-18.94732),
             vertex_3d(4.15412,-37.98388,-18.94732),
             vertex_3d(40.962880000000006,0.0,-21.08164),
             vertex_3d(32.00644,-27.90992,12.95976),
             vertex_3d(6.4041999999999994,-32.2252,29.3468),
             vertex_3d(6.4041999999999994,32.2252,29.3468),
             vertex_3d(3.0196,0.0,45.788000000000004),
             vertex_3d(-26.912319999999998,-30.685679999999998,19.66696),
             vertex_3d(-42.0762,0.0,2.8371600000000003),
             vertex_3d(-26.912319999999998,30.685679999999998,19.66696),
       };
    constexpr inline bn::color big_asteroid_1_colors[] = {
             bn::color(9,5,0),
    };
    constexpr inline int big_asteroid_1_color_0 = 0;

    constexpr inline face_3d big_asteroid_1_faces_full[] = {
        face_3d(big_asteroid_1_vertices, vertex_3d(0.2249,-0.0000,-0.9744),0,5,1,0,5),
        face_3d(big_asteroid_1_vertices, vertex_3d(0.2541,-0.0923,-0.9628),1,5,2,0,2),
        face_3d(big_asteroid_1_vertices, vertex_3d(0.6029,-0.7425,-0.2919),2,6,4,0,1),
        face_3d(big_asteroid_1_vertices, vertex_3d(0.8391,-0.5076,-0.1954),5,6,2,0,3),
        face_3d(big_asteroid_1_vertices, vertex_3d(0.8588,0.1584,0.4873),5,8,9,0,4),
        face_3d(big_asteroid_1_vertices, vertex_3d(0.5474,-0.3340,0.7673),9,7,6,0,3),
        face_3d(big_asteroid_1_vertices, vertex_3d(-0.2673,-0.4601,0.8467),9,10,7,0,2),
        face_3d(big_asteroid_1_vertices, vertex_3d(-0.6886,-0.0562,0.7230),9,12,11,0,4),
        face_3d(big_asteroid_1_vertices, vertex_3d(-0.8282,-0.5216,-0.2048),11,1,10,0,2),
        face_3d(big_asteroid_1_vertices, vertex_3d(-0.4704,-0.8554,-0.2167),10,1,4,0,1),
        face_3d(big_asteroid_1_vertices, vertex_3d(-0.8598,-0.0000,-0.5106),11,0,1,0,4),
        face_3d(big_asteroid_1_vertices, vertex_3d(-0.0811,0.9892,0.1217),8,3,12,0,7),
        face_3d(big_asteroid_1_vertices, vertex_3d(0.3272,0.3661,-0.8712),0,3,5,0,6),
        face_3d(big_asteroid_1_vertices, vertex_3d(0.2737,-0.4300,-0.8603),4,1,2,0,0),
        face_3d(big_asteroid_1_vertices, vertex_3d(0.2301,-0.9675,0.1047),6,7,4,0,2),
        face_3d(big_asteroid_1_vertices, vertex_3d(0.7186,0.6936,0.0492),5,3,8,0,5),
        face_3d(big_asteroid_1_vertices, vertex_3d(0.8277,0.3072,0.4696),5,9,6,0,5),
        face_3d(big_asteroid_1_vertices, vertex_3d(-0.2673,0.4601,0.8467),9,8,12,0,5),
        face_3d(big_asteroid_1_vertices, vertex_3d(-0.6886,0.0562,0.7230),9,11,10,0,5),
        face_3d(big_asteroid_1_vertices, vertex_3d(-0.8282,0.5216,-0.2048),11,12,0,0,5),
        face_3d(big_asteroid_1_vertices, vertex_3d(-0.4704,0.8554,-0.2167),12,3,0,0,5),
        face_3d(big_asteroid_1_vertices, vertex_3d(-0.0811,-0.9892,0.1217),4,7,10,0,0),
    };
    constexpr inline model_3d_item big_asteroid_1_full(big_asteroid_1_vertices, big_asteroid_1_faces_full, big_asteroid_1_colors);
    };
#endif // FR_MODEL_3D_ITEMS_BIG_ASTEROID_1_H