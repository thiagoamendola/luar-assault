#ifndef TEST_3D_SCENE_DEFS_H
#define TEST_3D_SCENE_DEFS_H

#include "player_laser.h"
#include "player_ship.h"
#include "scene_colors_generator.h"
#include "stage_section.h"
#include "static_model_3d_item.h"

#include "bn_color.h"
#include "bn_span.h"

// #include "bn_sprite_items_ninja.h"
#include "models/bush.h"
#include "models/player_ship_02.h"
#include "models/shot.h"
#include "models/asteroid1.h"

using namespace scene_colors_generator;

// --- Sections

// # Section 1

constexpr auto _s1_bush_model =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(30, 750, 40), 16000, fr::model_3d_items::shot_colors);
constexpr auto _s1_bush_model_2 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(30, 650, 40), 16000);
constexpr auto _s1_bush_model_3 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(30, 550, 40), 16000);
constexpr auto _s1_bush_model_4 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(30, 450, 40), 16000);
constexpr auto _s1_bush_model_5 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(30, 350, 40), 16000);
constexpr auto _s1_bush_model_6 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(30, 250, 40), 16000);
constexpr auto _s1_bush_model_7 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(30, 150, 40), 16000);
constexpr auto _s1_bush_model__2 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(-30, 750, 40), 0);
constexpr auto _s1_bush_model_2__2 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(-30, 650, 40), 0);
constexpr auto _s1_bush_model_3__2 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(-30, 550, 40), 0);
constexpr auto _s1_bush_model_4__2 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(-30, 450, 40), 0);
constexpr auto _s1_bush_model_5__2 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(-30, 350, 40), 0);
constexpr auto _s1_bush_model_6__2 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(-30, 250, 40), 0);
constexpr auto _s1_bush_model_7__2 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(-30, 150, 40), 0);

constexpr auto _section_1_static_model_items = {

    _s1_bush_model.item(),
    // _s1_bush_model_3.item(),
    // _s1_bush_model_2.item(),
    // _s1_bush_model_4.item(),
    _s1_bush_model__2.item(),
    // _s1_bush_model_2__2.item(),
    // _s1_bush_model_3__2.item(),
    // _s1_bush_model_4__2.item(),
    _s1_bush_model_5.item(),
    // _s1_bush_model_6.item(),
    // _s1_bush_model_7.item(),
    // _s1_bush_model_5__2.item(),
    _s1_bush_model_6__2.item(),
    // _s1_bush_model_7__2.item()

};

constexpr auto _section_1_enemies = {
    enemy_def(fr::point_3d(-20, 600, 0), 800, enemy_type::ASTEROID),
    enemy_def(fr::point_3d(0, 400, 20), 200, enemy_type::ASTEROID)

};

constexpr int _section_1_start = 1050;
constexpr int _section_1_end = 200;

constexpr stage_section section_1(_section_1_start, _section_1_end,
                                  _section_1_static_model_items, _section_1_enemies);

// # Section 2

constexpr auto _s2_bush_model_1 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(50, 50, 0), 16000, fr::model_3d_items::shot_colors);
constexpr auto _s2_bush_model_1b =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(-10, 50, 0), 16000, fr::model_3d_items::shot_colors);
constexpr auto _s2_bush_model_2 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(50, -50, 0), 16000, fr::model_3d_items::shot_colors);
constexpr auto _s2_bush_model_2b =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(-10, -50, 0), 16000, fr::model_3d_items::shot_colors);
constexpr auto _s2_bush_model_3 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(50, -150, 0), 16000, fr::model_3d_items::shot_colors);
constexpr auto _s2_bush_model_3b =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(-10, -150, 0), 16000, fr::model_3d_items::shot_colors);
constexpr auto _s2_bush_model_4 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(50, -250, 0), 16000);
constexpr auto _s2_bush_model_4b =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(-10, -250, 0), 16000, fr::model_3d_items::shot_colors);

constexpr auto _section_2_static_model_items = {

    _s2_bush_model_1.item(),
    // _s2_bush_model_1b.item(),
    _s2_bush_model_2.item(),
    // _s2_bush_model_2b.item(),
    _s2_bush_model_3.item(),
    // _s2_bush_model_3b.item(),
    _s2_bush_model_4.item(),
    // _s2_bush_model_4b.item()

};

constexpr auto _section_2_enemies = {
    enemy_def(fr::point_3d(-35, 0, 0), 800, enemy_type::ASTEROID),
    enemy_def(fr::point_3d(20, 400, 0), 600, enemy_type::ASTEROID),
    // enemy_def(fr::point_3d(-20, 200, 0), 400, enemy_type::ASTEROID),
    enemy_def(fr::point_3d(30, 100, 20), 200, enemy_type::ASTEROID)

};

constexpr int _section_2_start = 750;
constexpr int _section_2_end = -150;

constexpr stage_section section_2(_section_2_start, _section_2_end,
                                  _section_2_static_model_items, _section_2_enemies);

// # Section 3

constexpr auto _s3_bush_model_1 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(30, -350, -15), 16000);
constexpr auto _s3_bush_model_1b =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(-30, -350, -15), 16000);
constexpr auto _s3_bush_model_2 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(30, -450, -15), 16000);
constexpr auto _s3_bush_model_2b =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(-30, -450, -15), 16000);
constexpr auto _s3_bush_model_3 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(30, -550, -15), 16000);
constexpr auto _s3_bush_model_3b =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(-30, -550, -15), 16000);
constexpr auto _s3_bush_model_4 =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(30, -650, -15), 16000);
constexpr auto _s3_bush_model_4b =
    static_model_3d_item<fr::model_3d_items::bush_full>(
        fr::point_3d(-30, -650, -15), 16000);

constexpr auto _section_3_static_model_items = {

    // _s3_bush_model_1.item(),
    _s3_bush_model_1b.item(),
    // _s3_bush_model_2.item(),
    _s3_bush_model_2b.item(),
    // _s3_bush_model_3.item(),
    _s3_bush_model_3b.item(),
    // _s3_bush_model_4.item(),
    _s3_bush_model_4b.item()

};

constexpr auto _section_3_enemies = {
    enemy_def(fr::point_3d(-35, -100, 0), 800, enemy_type::ASTEROID),
    enemy_def(fr::point_3d(20, -400, 0), 600, enemy_type::ASTEROID),
    // enemy_def(fr::point_3d(-20, 200, 0), 400, enemy_type::ASTEROID),
    enemy_def(fr::point_3d(30, -500, 20), 200, enemy_type::ASTEROID)

};

constexpr int _section_3_start = 350;
constexpr int _section_3_end = -700;

constexpr stage_section section_3(_section_3_start, _section_3_end,
                                  _section_3_static_model_items, _section_3_enemies);

// # Sections List

// Define sections here:
constexpr const auto sections_full = {
    &section_1,
    &section_2,
    &section_3,
};

// Import these in your scene
constexpr stage_section_list_ptr sections = sections_full.begin();
constexpr size_t sections_count = sections_full.size();

// --- Colors

// Add model materials here
constexpr const auto raw_scene_colors = {
    bn::span<const bn::color>(fr::model_3d_items::debug_collider_colors),
    bn::span<const bn::color>(fr::model_3d_items::laser_colors),
    bn::span<const bn::color>(fr::model_3d_items::shot_colors),
    bn::span<const bn::color>(fr::model_3d_items::hurt_colors),
    bn::span<const bn::color>(fr::model_3d_items::player_ship_02_colors),
    bn::span<const bn::color>(fr::model_3d_items::bush_colors),
    bn::span<const bn::color>(fr::model_3d_items::asteroid1_colors),
};

constexpr size_t model_palette_count = raw_scene_colors.size();
constexpr size_t scene_palette_size = calculate_total_size(raw_scene_colors);
constexpr const bn::span<const bn::color> *raw_scene_color_ptr =
    raw_scene_colors.begin();

// Load these:
constexpr bn::array<bn::color, scene_palette_size> scene_colors =
    generate_scene_colors<scene_palette_size>(raw_scene_colors);

inline color_mapping_handler *get_scene_color_mapping()
{
    return new color_mapping_handler(model_palette_count, scene_palette_size,
                                     raw_scene_color_ptr, scene_colors.data());
};

#endif
