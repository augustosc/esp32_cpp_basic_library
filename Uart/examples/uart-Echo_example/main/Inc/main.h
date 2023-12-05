#pragma once

// base cpp esp-idf includes
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "nvs_flash.h"



// gpio class includes
#include "Gpio.h"
#include "Led.h"
#include "Uart.h"

#define LOG_LEVEL_LOCAL ESP_LOG_INFO

#define pdSECOND pdMS_TO_TICKS(1000)
#define pdMINUTE pdMS_TO_TICKS(60*1000);

constexpr uint32_t one_second{1000};
constexpr uint32_t one_minute{60*1000};




class Main final
{
public:
    Main(void)
        
    {
    }

    esp_err_t setup(void);
    void loop(void);


    /// create objects here as class Main members
    /// use them inside Main::setup and/or Main::loop

     LED::Led led_2 {GPIO_NUM_2, false};

    UART::Uart U0{0};

    UART::Uart U1{1};

    UART::Uart U2{2};


    

};