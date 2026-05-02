#include "hyperlight_background.h"

#include "bn_random.h"

hyperlight_background::hyperlight_background(bn::fixed_point speed, int trail_length) :
    _bg(bn::palette_bitmap_bg_ptr::create(palette_item)),
    _painter(_bg),
    _speed(speed),
    _trail_length(trail_length)
{
    _bg.set_priority(3); // Behind everything
    
    _init_stars();
    
    // Draw stars once (static for now)
    _painter.clear();
    for (const star& s : _stars)
    {
        _painter.plot(s.x.integer(), s.y.integer(), 1);
    }
    _painter.flip_page_later();
}

void hyperlight_background::_init_stars()
{
    bn::random random;
    
    for (int i = 0; i < NUM_STARS; ++i)
    {
        star s;
        s.x = random.get_int(240);  // 0 to 239
        s.y = random.get_int(160);  // 0 to 159
        _stars.push_back(s);
    }
}

void hyperlight_background::update()
{
    // Move all stars
    for (star& s : _stars)
    {
        s.x += _speed.x();
        s.y += _speed.y();
        
        // Wrap around screen edges
        if (s.x >= 240) s.x -= 240;
        if (s.x < 0) s.x += 240;
        if (s.y >= 160) s.y -= 160;
        if (s.y < 0) s.y += 160;
    }
    
    // Redraw stars with trails
    _painter.clear();
    for (const star& s : _stars)
    {
        // Draw trail in opposite direction of movement
        int trail_end_x = s.x.integer() - (_speed.x() * _trail_length).integer();
        int trail_end_y = s.y.integer() - (_speed.y() * _trail_length).integer();
        _painter.line(trail_end_x, trail_end_y, s.x.integer(), s.y.integer(), 2);
        
        // Draw star head
        _painter.plot(s.x.integer(), s.y.integer(), 1);
    }
    _painter.flip_page_later();
}
