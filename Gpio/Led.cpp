#include "Led.h"

namespace LED
{

    void Led::blink(const int64_t _delay)
    {
        int64_t now {millis()};
        if(now - m_timestamp >= _delay )
        {
            toggle();
            m_timestamp = now;
        }

    }


}