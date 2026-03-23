
    /*
     * Based on Nikku4211's Wavefront to Varooom Converter (github.com/nikku4211/)
     * Modified to support metadata (engine_scale / engine_brightness)
     * Source OBJ: shot.obj
     * Metadata: shot.json
     *
     */

    #ifndef FR_MODEL_3D_ITEMS_SHOT_H
    #define FR_MODEL_3D_ITEMS_SHOT_H

    #include "fr_model_3d_item.h"

    namespace fr::model_3d_items
    {
        constexpr inline vertex_3d shot_vertices[] = {
             vertex_3d(12.95334,0.22710000000000002,-0.024260000000000004),
             vertex_3d(-0.04558,9.0689,-0.06837),
             vertex_3d(0.024260000000000004,0.22710000000000002,10.06563),
             vertex_3d(0.02427,-9.84092,-0.02427),
             vertex_3d(0.107,0.22710000000000002,-9.86874),
             vertex_3d(-43.087939999999996,0.22710000000000002,0.024260000000000004),
       };
    constexpr inline bn::color shot_colors[] = {
             bn::color(24,0,2),
    };
    constexpr inline int shot_color_0 = 0;

    constexpr inline face_3d shot_faces_full[] = {
        face_3d(shot_vertices, vertex_3d(0.4554,0.6724,0.5835),0,1,2,0,7),
        face_3d(shot_vertices, vertex_3d(0.4794,-0.6156,-0.6255),3,4,0,0,5),
        face_3d(shot_vertices, vertex_3d(0.4827,-0.6199,0.6186),3,0,2,0,0),
        face_3d(shot_vertices, vertex_3d(-0.1623,-0.6985,0.6970),2,5,3,0,5),
        face_3d(shot_vertices, vertex_3d(-0.1618,-0.6892,-0.7062),3,5,4,0,1),
        face_3d(shot_vertices, vertex_3d(0.4543,0.6649,-0.5928),4,1,0,0,2),
        face_3d(shot_vertices, vertex_3d(-0.1515,0.7443,0.6504),2,1,5,0,3),
        face_3d(shot_vertices, vertex_3d(-0.1519,0.7327,-0.6634),5,1,4,0,7),
    };
    constexpr inline model_3d_item shot_full(shot_vertices, shot_faces_full, shot_colors);
    };
#endif // FR_MODEL_3D_ITEMS_SHOT_H