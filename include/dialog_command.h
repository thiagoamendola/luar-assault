#ifndef DIALOG_COMMAND_H
#define DIALOG_COMMAND_H

enum class dialog_command_type
{
    SUBTITLE,
    TUTORIAL
};

struct dialog_command
{
    const char* text;
    int start_time;
    int duration;
    dialog_command_type type = dialog_command_type::SUBTITLE;
};

#endif