#ifndef SRAM_DATA_H
#define SRAM_DATA_H

#include "bn_sram.h"

struct sram_data
{
    // Magic value to check if SRAM has been initialized
    int integrity_check_value = 0;
    int high_score = 0;

    static constexpr int EXPECTED_CHECK_VALUE = 0x4C554152; // "LUAR"

    bool is_valid() const
    {
        return integrity_check_value == EXPECTED_CHECK_VALUE;
    }

    static sram_data load()
    {
        sram_data data;
        bn::sram::read(data);
        if (!data.is_valid())
        {
            data.integrity_check_value = EXPECTED_CHECK_VALUE;
            data.high_score = 0;
            bn::sram::write(data);
        }
        return data;
    }

    void save()
    {
        bn::sram::write(*this);
    }

    // Updates high score if new score is higher. Returns true if it was a new record.
    bool try_update_high_score(int score)
    {
        if (score > high_score)
        {
            high_score = score;
            save();
            return true;
        }
        return false;
    }
};

#endif
