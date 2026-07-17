#ifndef CUTSCENE_COMMANDS_H
#define CUTSCENE_COMMANDS_H

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_sound_item.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string_view.h"
#include "bn_utility.h"
#include "bn_vector.h"

#include "fr_model_3d.h"
#include "fr_camera_3d.h"
#include "fr_point_3d.h"

#include "cutscene/timeline_command.h"
#include "cutscene/easing.h"

struct model_rotation
{
    bn::fixed phi = 0;
    bn::fixed theta = 0;
    bn::fixed psi = 0;
};

/**
 * Interpolates an fr::model_3d position over time.
 */
class move_model_cmd : public timeline_command
{
public:
    fr::model_3d &model;
    fr::point_3d start_pos;
    fr::point_3d end_pos;
    easing ease;
    bool use_current_start;

    move_model_cmd(fr::model_3d &m, fr::point_3d s, fr::point_3d e,
                   int start, int dur, easing easing_type = easing::LINEAR);
    move_model_cmd(fr::model_3d &m, fr::point_3d e,
                   int start, int dur, easing easing_type = easing::LINEAR);

    void start() override;
    void update(int local_frame) override;
    void end() override;
};

/**
 * Interpolates phi, theta AND psi together in a single command.
 *
 * All three axes are written atomically every frame, so their values are
 * always consistent with each other. This avoids the unpredictable
 * interaction that occurs when parallel single-axis commands compose
 * independently through the phi→theta→psi Euler chain.
 */
class rotate_model_combined_cmd : public timeline_command
{
public:
    fr::model_3d &model;
    model_rotation start_rot;
    model_rotation end_rot;
    easing ease;
    bool use_current_start;

    rotate_model_combined_cmd(fr::model_3d &m,
                              model_rotation s, model_rotation e,
                              int start, int dur, easing easing_type = easing::LINEAR);
    rotate_model_combined_cmd(fr::model_3d &m,
                              model_rotation e,
                              int start, int dur, easing easing_type = easing::LINEAR);

    void start() override;
    void update(int local_frame) override;
    void end() override;

private:
    void _apply(const model_rotation &r);
};

/**
 * Interpolates an fr::camera_3d position over time.
 */
class move_camera_cmd : public timeline_command
{
public:
    fr::camera_3d &camera;
    fr::point_3d start_pos;
    fr::point_3d end_pos;
    easing ease;
    bool use_current_start;

    move_camera_cmd(fr::camera_3d &cam, fr::point_3d s, fr::point_3d e,
                    int start, int dur, easing easing_type = easing::LINEAR);
    move_camera_cmd(fr::camera_3d &cam, fr::point_3d e,
                    int start, int dur, easing easing_type = easing::LINEAR);

    void start() override;
    void update(int local_frame) override;
    void end() override;
};

/**
 * Interpolates phi, theta AND psi of a camera together in a single command.
 *
 * Uses the same model_rotation struct as rotate_model_combined_cmd.
 */
class rotate_camera_cmd : public timeline_command
{
public:
    fr::camera_3d &camera;
    model_rotation start_rot;
    model_rotation end_rot;
    easing ease;

    rotate_camera_cmd(fr::camera_3d &cam,
                      model_rotation s, model_rotation e,
                      int start, int dur, easing easing_type = easing::LINEAR);

    void start() override;
    void update(int local_frame) override;
    void end() override;

private:
    void _apply(const model_rotation &r);
};

/**
 * Plays a bn::sound_item once on its start frame.
 * Volume in [0..1], speed in [0..64], panning in [-1..1].
 */
class play_sound_cmd : public timeline_command
{
public:
    bn::sound_item item;
    bn::fixed volume;
    bn::fixed speed;
    bn::fixed panning;

    /// Play with full control over volume, speed and panning.
    play_sound_cmd(bn::sound_item it, bn::fixed vol, bn::fixed spd, bn::fixed pan,
                   int start);

    /// Convenience constructor: volume only, speed=1, panning=0.
    play_sound_cmd(bn::sound_item it, bn::fixed vol, int start);

    void start() override;
    void update(int) override {}
};

/**
 * Drives a bn::sprite_animate_action for the duration of the command.
 */
class sprite_anim_cmd : public timeline_command
{
public:
    bn::sprite_animate_action<16> action;

    sprite_anim_cmd(bn::sprite_animate_action<16> &&act, int start, int dur);

    void update(int local_frame) override;
};

/**
 * Interpolates a bn::sprite_ptr position over time.
 */
class move_sprite_cmd : public timeline_command
{
public:
    bn::optional<bn::sprite_ptr> &sprite;
    bn::fixed_point start_pos;
    bn::fixed_point end_pos;
    easing ease;
    bool use_current_start;

    move_sprite_cmd(bn::optional<bn::sprite_ptr> &spr, bn::fixed_point start, bn::fixed_point end,
                    int start_time, int dur, easing easing_type = easing::LINEAR);
    move_sprite_cmd(bn::optional<bn::sprite_ptr> &spr, bn::fixed_point end,
                    int start_time, int dur, easing easing_type = easing::LINEAR);

    void start() override;
    void update(int local_frame) override;
    void end() override;

private:
    bn::sprite_ptr *_target_sprite();
};

/**
 * Fades a sprite using transparency blending.
 */
class sprite_fade_cmd : public timeline_command
{
public:
    bn::optional<bn::sprite_ptr> &sprite;
    bn::fixed start_alpha;
    bn::fixed end_alpha;

    sprite_fade_cmd(bn::optional<bn::sprite_ptr> &spr, bn::fixed alpha_start, bn::fixed alpha_end,
                    int start_frame, int dur);

    void start() override;
    void update(int local_frame) override;
    void end() override;

private:
    bn::sprite_ptr *_target_sprite();
};

/**
 * Updates the current subtitle text for a timed window.
 */
class subtitle_cmd : public timeline_command
{
public:
    bn::vector<bn::sprite_ptr, 40> &subtitle_sprites;
    bn::sprite_text_generator &text_generator;
    bn::string_view text;

    subtitle_cmd(bn::vector<bn::sprite_ptr, 40> &sprites,
                 bn::sprite_text_generator &generator,
                 bn::string_view subtitle_text,
                 int start, int dur);

    void start() override;
    void update(int) override {}
    void end() override;

private:
    static constexpr int SUBTITLE_X = 0;
    static constexpr int SUBTITLE_Y = 42;

    static int _last_subtitle_start_time;
};

/**
 * Runs arbitrary code once on its start frame.
 */
template<typename Function>
class lambda_cmd : public timeline_command
{
public:
    Function function;

    lambda_cmd(int start, Function fn) :
        timeline_command(start, 1),
        function(bn::move(fn))
    {
    }

    void start() override
    {
        function();
    }

    void update(int) override {}
};

template<typename Function>
lambda_cmd(int, Function) -> lambda_cmd<Function>;

#endif // CUTSCENE_COMMANDS_H
