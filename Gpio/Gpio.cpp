#include "Gpio.h"

namespace Gpio
{

[[nodiscard]] esp_err_t GpioBase::init(void)
{
    esp_err_t status{ESP_OK};

    ESP_LOGI(_log_tag, "Init Gpio object");

    status |= gpio_config(&_cfg);

    return status;
}

[[nodiscard]] esp_err_t GpioOutput::init(void)
{
    esp_err_t status{GpioBase::init()};

    if (ESP_OK == status)
    {
        // set off state == _inverted_logic
        status |= set(_inverted_logic);
    }

    return status;
}

esp_err_t GpioOutput::set(const bool state)
{
    current_state = state;

    //ESP_LOGI(_log_tag, "Calling gpio_set_level = %d", current_state);

    return gpio_set_level(_pin,
                          _inverted_logic ? 
                                !state : 
                                state);
}


[[nodiscard]] esp_err_t GpioInput::init(void)
{
    esp_err_t status{GpioBase::init()};

    if (ESP_OK == status)
    {
        get();
    }

    return status;
}

bool GpioInput::get()
{
    previous_state = current_state;
    current_state =  gpio_get_level(_pin);
    return _inverted_logic  ? 
                    !current_state :
                    current_state;
}


} // namespace Gpio