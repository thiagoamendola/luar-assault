#ifndef CUTSCENE_TIMELINE_H
#define CUTSCENE_TIMELINE_H

#include "bn_array.h"

#include "cutscene/timeline_command.h"

// <-- Clean comments
/**
 * Parallel-friendly cutscene player.
 *
 * Commands are added up-front via add(), then the timeline is started with
 * start(). Call update() once per frame; it ticks every command whose window
 * overlaps the current frame and returns false when the cutscene is finished.
 *
 * All commands run independently — multiple commands can be active on the
 * same frame with no ordering dependencies.
 *
 */
class cutscene_timeline
{
public:
    static constexpr int MAX_CMDS = 64;

    /// Register a command with this timeline.
    /// Commands must outlive the timeline (or be owned externally).
    void add(timeline_command* cmd);

    /// Reset the frame counter and begin playback.
    void start();

    /// Advance playback by one frame.
    /// return true while the cutscene is still running, false when it ends.
    bool update();

    bool is_running() const;
    int  current_frame() const;

    /// Remove all commands and reset state (does NOT delete command objects).
    void clear();

private:
    bn::array<timeline_command*, MAX_CMDS> _cmds = {};
    int  _cmd_count = 0;
    int  _frame = 0;
    bool _running = false;

    int _last_frame() const;
};

#endif // CUTSCENE_TIMELINE_H
