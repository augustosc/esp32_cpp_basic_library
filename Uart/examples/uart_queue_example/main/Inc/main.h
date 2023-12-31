#pragma once

// base cpp esp-idf includes
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "nvs_flash.h"



// gpio class includes
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

    /*******************************************************
     * create objects, functions and tasks here 
     * as class Main members, and use them inside Main::setup
     * and/or Main::loop.
    ********************************************************/


    UART::Uart U0{0};
    UART::Uart U1{1};
    UART::Uart U2{2};

    static void write_task(void* parm);
    static void read_task(void* parm);




    

};