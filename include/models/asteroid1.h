
    /*
     * Based on Nikku4211's Wavefront to Varooom Converter (github.com/nikku4211/)
     * Modified to support metadata (engine_scale / engine_brightness)
     * Source OBJ: asteroid1.obj
     * Metadata: asteroid1.json
     *
     */

    #ifndef FR_MODEL_3D_ITEMS_ASTEROID1_H
    #define FR_MODEL_3D_ITEMS_ASTEROID1_H

    #include "fr_model_3d_item.h"

    namespace fr::model_3d_items
    {
        constexpr inline vertex_3d asteroid1_vertices[] = {
             vertex_3d(12.11316,12.11316,12.11316),
             vertex_3d(-12.11316,13.4811,-6.32802),
             vertex_3d(-12.20064,12.11316,18.71763),
             vertex_3d(-9.5763,-6.95442,12.11316),
             vertex_3d(6.85854,-2.10126,-7.458600000000001),
       };
    constexpr inline bn::color asteroid1_colors[] = {
             bn::color(11,2,0),
    };
    constexpr inline int asteroid1_color_0 = 0;

    constexpr inline face_3d asteroid1_faces_full[] = {
        face_3d(asteroid1_vertices, vertex_3d(-0.9911,-0.1327,-0.0107),3,2,1,0,7),
        face_3d(asteroid1_vertices, vertex_3d(0.0148,0.9984,0.0546),0,1,2,0,3),
        face_3d(asteroid1_vertices, vertex_3d(0.2512,-0.2857,0.9248),3,0,2,0,7),
        face_3d(asteroid1_vertices, vertex_3d(-0.5309,-0.6031,-0.5953),1,4,3,0,7),
        face_3d(asteroid1_vertices, vertex_3d(0.6196,-0.7048,0.3455),3,4,0,0,7),
        face_3d(asteroid1_vertices, vertex_3d(0.4888,0.6382,-0.5948),4,1,0,0,7),
    };
    constexpr inline model_3d_item asteroid1_full(asteroid1_vertices, asteroid1_faces_full, asteroid1_colors);
    };
#endif // FR_MODEL_3D_ITEMS_ASTEROID1_H