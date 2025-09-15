#include "stage_section_renderer.h"

#include "stage_section.h"

#include "bn_fixed.h"
#include "bn_log.h"
#include "bn_string.h"

#include "fr_camera_3d.h"
#include "fr_model_3d_item.h"
#include "fr_models_3d.h"

void stage_section_renderer::render_single_section(
    const stage_section *section, fr::models_3d &models,
    const fr::model_3d_item **static_model_items)
{
    const int static_model_count = section->static_model_count();
    BN_LOG("STATICS SIZE: " + bn::to_string<32>(static_model_count));

    // <-- If new section has less models, we might run into an issue
    for (int i = 0; i < static_model_count; i++)
    {
        static_model_items[i] = &section->static_model_items()[i];
    }

    models.set_static_model_items(static_model_items, static_model_count);
}

int stage_section_renderer::render_sections(
    const bn::fixed camera_position, stage_section_list_ptr sections,
    size_t sections_count, const fr::model_3d_item **static_model_items)
{
    int current_model = 0;

    // Iterate through sections.
    for (size_t section_iter = 0; section_iter < sections_count; section_iter++)
    {
        const stage_section *current_section = sections[section_iter];
        // Check if we should render section.
        if (camera_position <= current_section->starting_pos() &&
            camera_position > current_section->ending_pos())
        {
            // Render section's models.
            for (int i = 0; i < current_section->static_model_count(); i++)
            {
                // Check if we're rendering more models than we can!
                if (current_model >= fr::constants_3d::max_static_models)
                {
                    BN_LOG(
                        "Stage Section Renderer: reached static model max "
                        "limit: " +
                        bn::to_string<64>(fr::constants_3d::max_static_models));
                    return current_model;
                }
                static_model_items[current_model] =
                    &current_section->static_model_items()[i];
                current_model++;
            }
        }
    }

    return current_model;
}

int stage_section_renderer::manage_section_render(
    stage_section_list_ptr sections, size_t sections_count,
    fr::camera_3d &camera, const fr::model_3d_item **static_model_items)
{

    // <-- Let's not pass this entire camera, just its Y position
    const bn::fixed camera_position = camera.position().y();
    // BN_LOG("CAMERA POS: " + bn::to_string<32>(camera_position));

    return stage_section_renderer::render_sections(
        camera_position, sections, sections_count, static_model_items);
}