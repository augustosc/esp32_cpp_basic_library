#pragma once

#include "Gpio.h"

namespace BUTTON {

    class Button : public Gpio::GpioInput
    {
    private:
        const int64_t debounce_delay{100};
        int64_t m_timestamp{};
        bool ris_edge{};
        bool fal_edge{};

    public:
        Button(const gpio_num_t pin, Gpio::gpio_pull_e pull_en,const bool invert = false)
        : GpioInput(pin, pull_en,invert)
        {}

        /********************************************
         * Note read_state() an state() are handled
         * by GpioInput class.
        *********************************************/

        //***********************************************
        /// @brief is button state changed?
        /// @return true if changed, false otherwise
        bool isChange();

        //***********************************************
        /// @brief has button rising edge?
        /// @return true if rising, false otherwise
        bool isRisingEdge(){return ris_edge;};

        //***********************************************
        /// @brief has button falling edge?
        /// @return true if falling, false otherwise
        bool isFallingEdge(){return fal_edge;};

        
        


    };



} // namespace BUTTON