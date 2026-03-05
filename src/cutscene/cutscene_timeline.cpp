#include "cutscene/cutscene_timeline.h"

#include "bn_assert.h"
#include "bn_algorithm.h"

void cutscene_timeline::add(timeline_command* cmd)
{
    BN_ASSERT(_cmd_count < MAX_CMDS, "cutscene_timeline: too many commands");
    _cmds[_cmd_count++] = cmd;
}

void cutscene_timeline::start()
{
    _frame = 0;
    _running = true;
}

bool cutscene_timeline::update()
{
    if(!_running)
    {
        return false;
    }

    for(int i = 0; i < _cmd_count; ++i)
    {
        timeline_command* cmd = _cmds[i];

        const int cmd_start = cmd->start_frame;
        const int cmd_end   = cmd->start_frame + cmd->duration;

        if(_frame < cmd_start || _frame > cmd_end)
        {
            continue; // not active
        }

        if(_frame == cmd_start)
        {
            cmd->start();
        }

        if(_frame >= cmd_start && _frame < cmd_end)
        {
            cmd->update(_frame - cmd_start);
        }

        if(_frame == cmd_end)
        {
            cmd->end();
        }
    }

    _frame++;

    if(_frame > _last_frame())
    {
        _running = false;
    }

    return _running;
}

bool cutscene_timeline::is_running() const
{
    return _running;
}

int cutscene_timeline::current_frame() const
{
    return _frame;
}

void cutscene_timeline::clear()
{
    for(int i = 0; i < _cmd_count; ++i)
    {
        if(_cmds[i] != nullptr)
        {
            delete _cmds[i];
            _cmds[i] = nullptr;
        }
    }
    _cmd_count = 0;
    _frame = 0;
    _running = false;
}

int cutscene_timeline::_last_frame() const
{
    int last = 0;
    for(int i = 0; i < _cmd_count; ++i)
    {
        last = bn::max(last, _cmds[i]->start_frame + _cmds[i]->duration);
    }
    return last;
}
