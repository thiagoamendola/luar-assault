#ifndef STAGE_H
#define STAGE_H

#include "static_model_3d_item.h"
#include "enemy_def.h"

class stage_section
{
  public:
    constexpr stage_section(
        const int starting_pos, const int ending_pos,
        const std::initializer_list<fr::model_3d_item> &static_model_items,
        const std::initializer_list<enemy_def> &enemies)
        : _static_model_items(static_model_items.begin()),
          _static_model_count(static_model_items.size()),
          _enemies(enemies.begin()),
          _enemies_count(enemies.size()),
          _starting_pos(starting_pos), _ending_pos(ending_pos)
    {
    }

    constexpr int starting_pos() const
    {
        return _starting_pos;
    }

    constexpr int ending_pos() const
    {
        return _ending_pos;
    }

    constexpr const fr::model_3d_item *static_model_items() const
    {
        return _static_model_items;
    }

    constexpr int static_model_count() const
    {
        return _static_model_count;
    }

    constexpr const enemy_def *enemies() const
    {
      return _enemies;
    }

    constexpr int enemies_count() const
    {
      return _enemies_count;
    }

  private:
    const fr::model_3d_item *_static_model_items;
    const int _static_model_count;
    const enemy_def* _enemies;
    const int _enemies_count;
    int _starting_pos;
    int _ending_pos;
};

typedef const stage_section *const *stage_section_list_ptr;

#endif