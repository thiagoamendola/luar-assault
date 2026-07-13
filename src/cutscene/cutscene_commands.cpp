#include "cutscene/cutscene_commands.h"

#include "bn_blending.h"
#include "bn_log.h"
#include "bn_utility.h"

// ---------------------------------------------------------------------------
// move_model_cmd
// ---------------------------------------------------------------------------

move_model_cmd::move_model_cmd(fr::model_3d& m, fr::point_3d s, fr::point_3d e,
                               int start, int dur, easing easing_type) :
    timeline_command(start, dur),
    model(m), start_pos(s), end_pos(e), ease(easing_type) {}

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
                                                     easing easing_type) :
    timeline_command(start, dur),
    model(m), start_rot(s), end_rot(e), ease(easing_type) {}

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
                                 int start, int dur, easing easing_type) :
    timeline_command(start, dur),
    camera(cam), start_pos(s), end_pos(e), ease(easing_type) {}

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
// rotate_camera_cmd
// ---------------------------------------------------------------------------

rotate_camera_cmd::rotate_camera_cmd(fr::camera_3d& cam,
                                     model_rotation s,
                                     model_rotation e,
                                     int start, int dur,
                                     easing easing_type) :
    timeline_command(start, dur),
    camera(cam), start_rot(s), end_rot(e), ease(easing_type) {}

void rotate_camera_cmd::_apply(const model_rotation& r)
{
    camera.set_phi(r.phi);
    camera.set_theta(r.theta);
    camera.set_psi(r.psi);
}

void rotate_camera_cmd::start()
{
    _apply(start_rot);
}

void rotate_camera_cmd::update(int local_frame)
{
    bn::fixed t = apply_easing(bn::fixed(local_frame) / duration, ease);
    model_rotation r;
    r.phi   = start_rot.phi   + (end_rot.phi   - start_rot.phi)   * t;
    r.theta = start_rot.theta + (end_rot.theta - start_rot.theta) * t;
    r.psi   = start_rot.psi   + (end_rot.psi   - start_rot.psi)   * t;
    _apply(r);
}

void rotate_camera_cmd::end()
{
    _apply(end_rot);
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
// move_sprite_cmd
// ---------------------------------------------------------------------------

move_sprite_cmd::move_sprite_cmd(bn::optional<bn::sprite_ptr>& spr, bn::fixed_point start,
                                 bn::fixed_point end, int start_time, int dur, easing easing_type) :
    timeline_command(start_time, dur),
    sprite(spr), start_pos(start), end_pos(end), ease(easing_type),
    use_current_start(false) {}

move_sprite_cmd::move_sprite_cmd(bn::optional<bn::sprite_ptr>& spr, bn::fixed_point end,
                                 int start_time, int dur, easing easing_type) :
    timeline_command(start_time, dur),
    sprite(spr), start_pos(end), end_pos(end), ease(easing_type),
    use_current_start(true) {}

bn::sprite_ptr* move_sprite_cmd::_target_sprite()
{
    if(sprite.has_value())
    {
        return &sprite.value();
    }

    return nullptr;
}

void move_sprite_cmd::start()
{
    bn::sprite_ptr* target = _target_sprite();
    if(!target)
    {
        return;
    }

    if(use_current_start)
    {
        start_pos = target->position();
    }

    target->set_position(start_pos);
}

void move_sprite_cmd::update(int local_frame)
{
    bn::sprite_ptr* target = _target_sprite();
    if(!target)
    {
        return;
    }

    if (duration <= 0)
    {
        target->set_position(end_pos);
        return;
    }
    bn::fixed t = apply_easing(bn::fixed(local_frame) / duration, ease);
    target->set_position(start_pos + (end_pos - start_pos) * t);
}

void move_sprite_cmd::end()
{
    if(bn::sprite_ptr* target = _target_sprite())
    {
        target->set_position(end_pos);
    }
}

// ---------------------------------------------------------------------------
// sprite_fade_cmd
// ---------------------------------------------------------------------------

sprite_fade_cmd::sprite_fade_cmd(bn::optional<bn::sprite_ptr>& spr, bn::fixed alpha_start,
                                 bn::fixed alpha_end, int start, int dur) :
    timeline_command(start, dur),
    sprite(spr), start_alpha(alpha_start), end_alpha(alpha_end) {}

bn::sprite_ptr* sprite_fade_cmd::_target_sprite()
{
    if(sprite.has_value())
    {
        return &sprite.value();
    }

    return nullptr;
}

void sprite_fade_cmd::start()
{
    bn::sprite_ptr* target = _target_sprite();
    if(!target)
    {
        return;
    }

    target->set_visible(true);
    target->set_blending_enabled(true);
    bn::blending::set_transparency_alpha(start_alpha);
}

void sprite_fade_cmd::update(int local_frame)
{
    if(!_target_sprite())
    {
        return;
    }

    bn::fixed t = bn::fixed(local_frame) / duration;
    bn::blending::set_transparency_alpha(start_alpha + (end_alpha - start_alpha) * t);
}

void sprite_fade_cmd::end()
{
    if(_target_sprite())
    {
        bn::blending::set_transparency_alpha(end_alpha);
    }
}

// ---------------------------------------------------------------------------
// subtitle_cmd
// ---------------------------------------------------------------------------

int subtitle_cmd::_last_subtitle_start_time = -1;

subtitle_cmd::subtitle_cmd(bn::vector<bn::sprite_ptr, 40>& sprites,
                           bn::sprite_text_generator& generator,
                           bn::string_view subtitle_text,
                           int start, int dur) :
    timeline_command(start, dur),
    subtitle_sprites(sprites), text_generator(generator), text(subtitle_text)
{
}

void subtitle_cmd::start()
{
    subtitle_sprites.clear();
    text_generator.generate(SUBTITLE_X, SUBTITLE_Y, text, subtitle_sprites);
    _last_subtitle_start_time = start_frame;
}

void subtitle_cmd::end()
{
    if(start_frame == _last_subtitle_start_time)
    {
        subtitle_sprites.clear();
    }
}

// ---------------------------------------------------------------------------
// play_sound_cmd
// ---------------------------------------------------------------------------

play_sound_cmd::play_sound_cmd(bn::sound_item it, bn::fixed vol, bn::fixed spd,
                               bn::fixed pan, int start) :
    timeline_command(start, 1),
    item(it), volume(vol), speed(spd), panning(pan) {}

play_sound_cmd::play_sound_cmd(bn::sound_item it, bn::fixed vol, int start) :
    timeline_command(start, 1),
    item(it), volume(vol), speed(1), panning(0) {}

void play_sound_cmd::start()
{
    item.play(volume, speed, panning);
}
