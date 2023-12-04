#include "Button.h"

namespace BUTTON {

    /**********************************************
     * isChange debounces button.
     * Only read new state after debounce_delay.
     * Returns true, if old_state != state.
     * Also detects edge, rising or falling.
    ***********************************************/
    bool Button::isChange()
    {
        int64_t now = millis();
        if(now - m_timestamp < debounce_delay) return false;
        m_timestamp = now;
        bool old_state = state();
        get();
        if (state() == old_state) {
            return false;
        }
        else{
            ESP_LOGI(_log_tag, "invert_logic = %d & previous = %d & current = %d!", _inverted_logic, previous_state,current_state);
            if (previous_state == 0 && current_state == 1){
                ris_edge = _inverted_logic ? false : true;
                fal_edge = _inverted_logic ? true  : false;
            }
            if (previous_state == 1 && current_state == 0){
                ris_edge = _inverted_logic ? true : false;
                fal_edge = _inverted_logic ? false : true;
            }
            ESP_LOGI(_log_tag, "rise = %d & fall = %d",ris_edge, fal_edge);
        return true;

        }

    }


} // namespace BUTTON