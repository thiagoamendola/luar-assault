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
    
    // Grid-based jittering: divide screen into cells, place one star per cell
    // Calculate grid size based on NUM_STARS (try to keep cells roughly square)
    constexpr int GRID_COLS = 8;
    constexpr int GRID_ROWS = (NUM_STARS + GRID_COLS - 1) / GRID_COLS;  // Ceiling division
    constexpr int CELL_WIDTH = 240 / GRID_COLS;
    constexpr int CELL_HEIGHT = 160 / GRID_ROWS;
    
    int stars_created = 0;
    for (int row = 0; row < GRID_ROWS && stars_created < NUM_STARS; ++row)
    {
        for (int col = 0; col < GRID_COLS && stars_created < NUM_STARS; ++col)
        {
            star s;
            // Random position within this cell
            s.x = col * CELL_WIDTH + random.get_int(CELL_WIDTH);
            s.y = row * CELL_HEIGHT + random.get_int(CELL_HEIGHT);
            _stars.push_back(s);
            ++stars_created;
        }
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
