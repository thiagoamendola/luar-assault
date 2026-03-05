#include "cutscene/cutscene_commands.h"

#include "bn_utility.h"

// ---------------------------------------------------------------------------
// move_model_cmd
// ---------------------------------------------------------------------------

move_model_cmd::move_model_cmd(fr::model_3d& m, fr::point_3d s, fr::point_3d e,
                               int start, int dur, easing ease) :
    timeline_command(start, dur),
    model(m), start_pos(s), end_pos(e), ease(ease) {}

void move_model_cmd::start()
{
    model.set_position(start_pos);
}

void move_model_cmd::update(int local_frame)
{
    bn::fixed t = apply_easing(bn::fixed(local_frame) / duration, ease);
    model.set_position(start_pos + (end_pos - start_pos) * t);
}

void move_model_cmd::end()
{
    model.set_position(end_pos);
}

// ---------------------------------------------------------------------------
// rotate_model_combined_cmd
// ---------------------------------------------------------------------------

rotate_model_combined_cmd::rotate_model_combined_cmd(fr::model_3d& m,
                                                     model_rotation s,
                                                     model_rotation e,
                                                     int start, int dur,
                                                     easing ease) :
    timeline_command(start, dur),
    model(m), start_rot(s), end_rot(e), ease(ease) {}

void rotate_model_combined_cmd::_apply(const model_rotation& r)
{
    model.set_phi(r.phi);
    model.set_theta(r.theta);
    model.set_psi(r.psi);
}

void rotate_model_combined_cmd::start()
{
    _apply(start_rot);
}

void rotate_model_combined_cmd::update(int local_frame)
{
    bn::fixed t = apply_easing(bn::fixed(local_frame) / duration, ease);
    model_rotation r;
    r.phi   = start_rot.phi   + (end_rot.phi   - start_rot.phi)   * t;
    r.theta = start_rot.theta + (end_rot.theta - start_rot.theta) * t;
    r.psi   = start_rot.psi   + (end_rot.psi   - start_rot.psi)   * t;
    _apply(r);
}

void rotate_model_combined_cmd::end()
{
    _apply(end_rot);
}

// ---------------------------------------------------------------------------
// move_camera_cmd
// ---------------------------------------------------------------------------

move_camera_cmd::move_camera_cmd(fr::camera_3d& cam, fr::point_3d s, fr::point_3d e,
                                 int start, int dur, easing ease) :
    timeline_command(start, dur),
    camera(cam), start_pos(s), end_pos(e), ease(ease) {}

void move_camera_cmd::start()
{
    camera.set_position(start_pos);
}

void move_camera_cmd::update(int local_frame)
{
    bn::fixed t = apply_easing(bn::fixed(local_frame) / duration, ease);
    camera.set_position(start_pos + (end_pos - start_pos) * t);
}

void move_camera_cmd::end()
{
    camera.set_position(end_pos);
}

// ---------------------------------------------------------------------------
// sprite_anim_cmd
// ---------------------------------------------------------------------------

sprite_anim_cmd::sprite_anim_cmd(bn::sprite_animate_action<16>&& act,
                                 int start, int dur) :
    timeline_command(start, dur),
    action(bn::move(act)) {}

void sprite_anim_cmd::update(int /*local_frame*/)
{
    if(!action.done())
        action.update();
}

// ---------------------------------------------------------------------------
// play_sound_cmd
// ---------------------------------------------------------------------------

play_sound_cmd::play_sound_cmd(bn::sound_item it, bn::fixed vol, bn::fixed spd,
                               bn::fixed pan, int start_frame) :
    timeline_command(start_frame, 1),
    item(it), volume(vol), speed(spd), panning(pan) {}

play_sound_cmd::play_sound_cmd(bn::sound_item it, bn::fixed vol, int start_frame) :
    timeline_command(start_frame, 1),
    item(it), volume(vol), speed(1), panning(0) {}

void play_sound_cmd::start()
{
    item.play(volume, speed, panning);
}
