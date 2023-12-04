#pragma once

// base cpp esp-idf includes
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


// gpio class includes
#include "Gpio.h"
#include "Led.h"
#include "Button.h"

#define pdSECOND pdMS_TO_TICKS(1000)




class Main final
{
public:
    esp_err_t setup(void);
    void loop(void);


    
    LED::Led led_2 {GPIO_NUM_2, false};
    LED::Led led_18 {GPIO_NUM_18, false};
    BUTTON::Button butt_0 {GPIO_NUM_0, Gpio::gpio_pull_e::UP,true};
    

};