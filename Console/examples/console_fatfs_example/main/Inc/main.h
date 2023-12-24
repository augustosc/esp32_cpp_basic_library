#pragma once

// base cpp esp-idf includes
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "MyFatfs.h"
#include "Uart.h"
#include "Console.h"



// class includes


#define LOG_LEVEL_LOCAL ESP_LOG_INFO

#define pdSECOND pdMS_TO_TICKS(1000)






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

    UART::Uart U0{0};

    MYFATFS::Fatfs mydisk;

    /**
     * Note that we are passing a child object (a Fatfs object pointer: mydisk) 
     *  to a parent object pointer in Console (FileStream object pointer: _disk).
     * This strategy permits the same Console class to be used by a Fatfs and Sdcard object.
     * In the case of Sdcard, we should pass a Sdcard object pointer to the 
     * FileStream object pointer in Console.
     * Example:
     *  creating onject: SDCARD::Sdcard sd {PIN_CS, PIN_MOSI, PIN_CLK, PIN_MISO};
     * Passing to Console: CONSOLE::Console sh{&U0,&sd};
    */

    CONSOLE::Console sh{&U0,&mydisk};


};