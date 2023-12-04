#pragma once

#include "Gpio.h"

namespace LED {
    class Led : public Gpio::GpioOutput
    {
    private:
        int64_t m_timestamp{};
        

    public:
        Led(const gpio_num_t pin, const bool invert = false)
        : GpioOutput(pin,invert)
        {}

        //***********************************************
        /// @brief turn led on
        /// @return ESP_OK on success
        esp_err_t on(void){return set(true);}

        //***********************************************
        /// @brief turn led off
        /// @return ESP_OK on success
        esp_err_t off(void){return set(false);}


        //***********************************************
        /// @brief toggle led
        /// @return ESP_OK on success
        esp_err_t toggle(void){return set(!state());}

        //***********************************************

        /// @brief blink led on interval
        /// @param _delay interval
        void blink(const int64_t _delay);

        
    };




} // namespace LED