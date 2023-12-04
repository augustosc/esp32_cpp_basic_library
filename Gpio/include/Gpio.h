#pragma once

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"

namespace Gpio
{
    
enum class gpio_pull_e{
    UP,
    DOWN,
    NONE
};

class GpioBase
{ 
protected:
    constexpr static const char* _log_tag{"GPIO"};
    const gpio_num_t _pin;
    const bool _inverted_logic = false;
    const gpio_config_t _cfg;

    [[nodiscard]] esp_err_t init(void);

    static int64_t millis()
    {
        return (esp_timer_get_time()/1000);
    }

public:
    constexpr GpioBase(const gpio_num_t pin,
                       const gpio_config_t& config, 
                       const bool invert_logic = false) :
        _pin{pin},
        _inverted_logic{invert_logic},
        _cfg{config}
    {}

    virtual bool state(void) =0;

    
}; // class GpioBase



class GpioOutput : public GpioBase
{
private:
    bool current_state = false;

public:
    constexpr GpioOutput(const gpio_num_t pin, const bool invert = false) :
        GpioBase{pin, 
                    gpio_config_t{
                        .pin_bit_mask   = static_cast<uint64_t>(1) << pin,
                        .mode           = GPIO_MODE_OUTPUT,
                        .pull_up_en     = GPIO_PULLUP_DISABLE,
                        .pull_down_en   = GPIO_PULLDOWN_ENABLE,
                        .intr_type      = GPIO_INTR_DISABLE
                    }, 
                    invert}
    { }

    [[nodiscard]] esp_err_t init(void);
    esp_err_t set(const bool state);
    bool state(void) { return current_state; }

}; // class GpioOutput



class GpioInput : public GpioBase
{
protected:
    bool current_state = false;
    bool previous_state = false;
public:
    GpioInput(const gpio_num_t pin, gpio_pull_e pull_en,const bool invert = false)
    : GpioBase{pin
              ,gpio_config_t{
                        .pin_bit_mask   = static_cast<uint64_t>(1) << pin,
                        .mode           = GPIO_MODE_INPUT,
                        .pull_up_en     = (pull_en == gpio_pull_e::UP) 
                                            ? GPIO_PULLUP_ENABLE
                                            : GPIO_PULLUP_DISABLE,
                        .pull_down_en   = (pull_en == gpio_pull_e::DOWN)
                                            ? GPIO_PULLDOWN_ENABLE
                                            : GPIO_PULLDOWN_DISABLE,
                        .intr_type      = GPIO_INTR_DISABLE
                    }
               ,invert}
{}
    [[nodiscard]] esp_err_t init(void);
    bool get();
    bool state(void) { return current_state; };
    bool read_state(void)
    {
        return _inverted_logic  ? 
                    !current_state :
                    current_state;
    }

}; // class GpioInput

} // namespace GPIO 