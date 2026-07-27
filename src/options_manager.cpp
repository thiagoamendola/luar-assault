#include "options_manager.h"

#include "bn_keypad.h"
#include "bn_string.h"

#include "controller.h"
#include "k8x8_sprite_font.h"

options_manager::options_manager(controller *controller) :
    _controller(controller),
    _text_generator(k8x8_sprite_font)
{
    _text_generator.set_bg_priority(2);
    _text_generator.set_center_alignment();
}

void options_manager::show_menu()
{
    _is_open = true;
    _current_selection = 0;
    render_menu();
}

void options_manager::close_menu()
{
    hide_menu();
}

void options_manager::menu_update()
{
    if (!_is_open)
    {
        return;
    }

    if (bn::keypad::b_pressed())
    {
        hide_menu();
        return;
    }

    if (bn::keypad::up_pressed())
    {
        _current_selection--;
        if (_current_selection < 0)
        {
            _current_selection = MENU_OPTIONS.size() - 1;
        }
    }
    else if (bn::keypad::down_pressed())
    {
        _current_selection++;
        if (_current_selection >= MENU_OPTIONS.size())
        {
            _current_selection = 0;
        }
    }
    else if (bn::keypad::left_pressed() || bn::keypad::right_pressed())
    {
        if (_current_selection == INVERT_Y_AXIS_OPTION)
        {
            toggle_invert_y_axis();
        }
    }
    else if (bn::keypad::a_pressed())
    {
        if (_current_selection == INVERT_Y_AXIS_OPTION)
        {
            toggle_invert_y_axis();
        }
        else if (_current_selection == BACK_OPTION)
        {
            hide_menu();
            return;
        }
    }

    render_menu();
}

void options_manager::hide_menu()
{
    _is_open = false;
    _text_sprites.clear();
}

void options_manager::render_menu()
{
    _text_sprites.clear();

    _text_generator.generate(0, -40, "OPTIONS", _text_sprites);
    auto STARTING_Y = -10;
    auto OFFSET_Y = 15;

    for (int i = 0; i < MENU_OPTIONS.size(); ++i)
    {
        bn::string<32> option_name = MENU_OPTIONS[i];
        // Option updates
        if (i == INVERT_Y_AXIS_OPTION)
        {
            option_name = _controller->get_invert_y_axis() ? "Invert Y axis: ON" : "Invert Y axis: OFF";
        }
        // Update selection.
        if (i == _current_selection)
        {
            option_name = "> " + option_name + " <";
        }
        _text_generator.generate(0, STARTING_Y + i * OFFSET_Y, option_name,
                                _text_sprites);
    }
}

void options_manager::toggle_invert_y_axis()
{
    _controller->set_invert_y_axis(!_controller->get_invert_y_axis());
}