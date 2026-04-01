#ifndef SCENE_COLOR_GENERATOR_H
#define SCENE_COLOR_GENERATOR_H

#include <map>

#include "bn_array.h"
#include "bn_color.h"
#include "bn_log.h"
#include "bn_span.h"
#include "bn_string.h"
#include "bn_unordered_map.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

namespace scene_colors_generator
{

const int MAX_COLORS = 16;
const int MAX_MODELS = 16;

//
struct color_mapping_handler
{
    color_mapping_handler(size_t model_palette_count, size_t scene_palette_size,
                          const bn::span<const bn::color> *raw_scene_colors,
                          const bn::color *scene_colors

                          )
        : _model_palette_count(model_palette_count),
          _scene_palette_size(scene_palette_size),
          _scene_colors(scene_colors)
    {
        for (size_t i = 0; i < model_palette_count; ++i)
        {
            const auto colors_span = raw_scene_colors[i];
            const auto colors_raw = colors_span.data();

            int color_index = 0;

            for (auto color : colors_span)
            {
                // Find color in scene_colors and add index
                for (size_t j = 0; j < scene_palette_size; j++)
                {
                    if (scene_colors[j] == color)
                    {
                        _color_map[colors_raw][color_index] = j;
                        break;
                    }
                }

                color_index++;
            }

            _palette_sizes[i] = (int)colors_span.size();
        }
    }

    int get_index(int index, const bn::color *model_color)
    {
        return _color_map[model_color][index];
    }

    void log_debug()
    {
        BN_LOG("[COLOR_MAPPING_HANDLER] scene_palette_size: ", (int)_scene_palette_size);

        BN_LOG("[COLOR_MAPPING_HANDLER] scene_colors: ", (int)_scene_palette_size);
        bn::string<256> sc_msg;
        for (int i = 0; i < (int)_scene_palette_size; ++i)
        {
            const bn::color& c = _scene_colors[i];
            sc_msg.append("(");
            sc_msg.append(bn::to_string<4>(c.red()));
            sc_msg.append(",");
            sc_msg.append(bn::to_string<4>(c.green()));
            sc_msg.append(",");
            sc_msg.append(bn::to_string<4>(c.blue()));
            sc_msg.append(") ");
        }
        BN_LOG(sc_msg);

        BN_LOG("[COLOR_MAPPING_HANDLER] color_map size: ", (int)_color_map.size());
        BN_LOG("[COLOR_MAPPING_HANDLER] color_map: ");
        int entry = 0;
        for (auto it = _color_map.begin(); it != _color_map.end(); ++it)
        {
            const int pal_size = _palette_sizes[entry];
            bn::string<256> msg;
            msg.append("  [");
            msg.append(bn::to_string<4>(entry));
            msg.append("]: ");
            for (int i = 0; i < pal_size; ++i)
            {
                const bn::color& c = it->first[i];
                msg.append("(");
                msg.append(bn::to_string<4>(c.red()));
                msg.append(",");
                msg.append(bn::to_string<4>(c.green()));
                msg.append(",");
                msg.append(bn::to_string<4>(c.blue()));
                msg.append(")");
            }
            msg.append(" => [");
            for (int i = 0; i < pal_size; ++i)
            {
                msg.append(bn::to_string<4>(it->second[i]));
                msg.append(", ");
            }
            msg.append("]");
            BN_LOG(msg);
            ++entry;
        }
    }

    size_t _model_palette_count;
    size_t _scene_palette_size;
    const bn::color *_scene_colors;

    bn::unordered_map<const bn::color *, bn::array<int, MAX_COLORS>, MAX_MODELS>
        _color_map;
    bn::array<int, MAX_MODELS> _palette_sizes;
};

// Get full size of final scene color array (does not count for repeated)
constexpr size_t calculate_total_size(
    const std::initializer_list<bn::span<const bn::color>> &color_list)
{
    size_t total_size = 0;
    for (const auto &colors_span : color_list)
    {
        total_size += colors_span.size();
    }
    return total_size;
}

//
template <size_t FullSize>
constexpr bn::array<bn::color, FullSize> generate_scene_colors(
    const std::initializer_list<bn::span<const bn::color>> color_list)
{
    bn::array<bn::color, FullSize> color_list_vec;
    int i = 0;

    for (auto colors_span : color_list)
    {
        for (auto color : colors_span)
        {
            bool repeated = false;
            for (auto existing_color : color_list_vec)
            {
                if (existing_color == color)
                {
                    repeated = true;
                    break;
                }
            }
            if (!repeated)
            {
                color_list_vec[i] = color;
                i++;
            }
        }
    }

    return color_list_vec;
}

} // namespace scene_colors_generator

#endif
