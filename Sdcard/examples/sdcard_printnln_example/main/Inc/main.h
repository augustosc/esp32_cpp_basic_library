#pragma once

// base cpp esp-idf includes
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Sdcard.h"



#define pdSECOND pdMS_TO_TICKS(1000)

#define PIN_CS 4
#define PIN_MOSI 5
#define PIN_CLK 18
#define PIN_MISO 19



class Main final
{
public:

    
    
    esp_err_t setup(void);
    void loop(void);



    SDCARD::Sdcard sd {PIN_CS, PIN_MOSI, PIN_CLK, PIN_MISO};
    

};