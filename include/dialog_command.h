#ifndef DIALOG_COMMAND_H
#define DIALOG_COMMAND_H

enum class dialog_command_type
{
    SUBTITLE,
    TUTORIAL
};

enum class dialog_character
{
    TEST,
    MC
};

struct dialog_command
{
    const char* text;
    int start_time;
    int duration;
    dialog_command_type type = dialog_command_type::SUBTITLE;
    dialog_character character = dialog_character::TEST;
};

#endif