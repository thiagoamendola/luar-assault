#ifndef CUTSCENE_COMMANDS_H
#define CUTSCENE_COMMANDS_H

#include "bn_fixed.h"
#include "bn_sound_item.h"
#include "bn_sprite_animate_actions.h"

#include "fr_model_3d.h"
#include "fr_camera_3d.h"
#include "fr_point_3d.h"

#include "cutscene/timeline_command.h"
#include "cutscene/easing.h"

struct model_rotation
{
    bn::fixed phi   = 0;
    bn::fixed theta = 0;
    bn::fixed psi   = 0;
};

/**
 * Interpolates an fr::model_3d position over time.
 */
class move_model_cmd : public timeline_command
{
public:
    fr::model_3d& model;
    fr::point_3d  start_pos;
    fr::point_3d  end_pos;
    easing        ease;

    move_model_cmd(fr::model_3d& m, fr::point_3d s, fr::point_3d e,
                   int start, int dur, easing ease = easing::LINEAR);

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
    fr::model_3d&  model;
    model_rotation start_rot;
    model_rotation end_rot;
    easing         ease;

    rotate_model_combined_cmd(fr::model_3d& m,
                              model_rotation s, model_rotation e,
                              int start, int dur, easing ease = easing::LINEAR);

    void start() override;
    void update(int local_frame) override;
    void end() override;

private:
    void _apply(const model_rotation& r);
};

/**
 * Interpolates an fr::camera_3d position over time.
 */
class move_camera_cmd : public timeline_command
{
public:
    fr::camera_3d& camera;
    fr::point_3d   start_pos;
    fr::point_3d   end_pos;
    easing         ease;

    move_camera_cmd(fr::camera_3d& cam, fr::point_3d s, fr::point_3d e,
                    int start, int dur, easing ease = easing::LINEAR);

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
    fr::camera_3d& camera;
    model_rotation start_rot;
    model_rotation end_rot;
    easing         ease;

    rotate_camera_cmd(fr::camera_3d& cam,
                      model_rotation s, model_rotation e,
                      int start, int dur, easing ease = easing::LINEAR);

    void start() override;
    void update(int local_frame) override;
    void end() override;

private:
    void _apply(const model_rotation& r);
};

/**
 * Plays a bn::sound_item once on its start frame.
 * Volume in [0..1], speed in [0..64], panning in [-1..1].
 */
class play_sound_cmd : public timeline_command
{
public:
    bn::sound_item item;
    bn::fixed      volume;
    bn::fixed      speed;
    bn::fixed      panning;

    /// Play with full control over volume, speed and panning.
    play_sound_cmd(bn::sound_item it, bn::fixed vol, bn::fixed spd, bn::fixed pan,
                   int start_frame);

    /// Convenience constructor: volume only, speed=1, panning=0.
    play_sound_cmd(bn::sound_item it, bn::fixed vol, int start_frame);

    void start() override;
    void update(int local_frame) override {}
};

/**
 * Drives a bn::sprite_animate_action for the duration of the command.
 */
class sprite_anim_cmd : public timeline_command
{
public:
    bn::sprite_animate_action<16> action;

    sprite_anim_cmd(bn::sprite_animate_action<16>&& act, int start, int dur);

    void update(int local_frame) override;
};

#endif // CUTSCENE_COMMANDS_H
