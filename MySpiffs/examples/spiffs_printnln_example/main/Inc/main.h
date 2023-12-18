#pragma once

// base cpp esp-idf includes
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mySpiffs.h"



#define pdSECOND pdMS_TO_TICKS(1000)



class Main final
{
public:

    
    
    esp_err_t setup(void);
    void loop(void);

    //void readbyline(const char* f_name, int n_lines);


    SPIFFS::Spiffs spf;
    

};