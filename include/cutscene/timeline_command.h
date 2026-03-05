#ifndef TIMELINE_COMMAND_H
#define TIMELINE_COMMAND_H

// <-- Clean comments
/**
 * Base class for all cutscene timeline commands.
 *
 * A TimelineCommand describes a single timed action within a CutsceneTimeline.
 * Commands run in parallel: multiple commands may be active on the same frame,
 * each spanning their own [start_frame, start_frame + duration) window.
 *
 * Subclasses must implement update(int local_frame).
 * Optionally override start() and end() for setup/teardown logic.
 *
 * local_frame = current_frame - start_frame, ranging from 0 to duration - 1.
 */
class timeline_command
{
public:
    int start_frame;
    int duration;

    timeline_command(int start, int dur) :
        start_frame(start), duration(dur)
    {

    };

    virtual ~timeline_command() = default;

    /// Called once on the frame this command begins.
    virtual void start() {};

    /// Called every frame while the command is active.
    /// @param local_frame  Frames elapsed since start_frame (0 … duration-1).
    virtual void update(int local_frame) = 0;

    /// Called once on the frame this command ends.
    virtual void end() {};
};

#endif // TIMELINE_COMMAND_H
