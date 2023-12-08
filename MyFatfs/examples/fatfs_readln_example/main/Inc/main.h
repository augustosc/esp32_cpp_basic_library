#pragma once

// base cpp esp-idf includes
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "MyFatfs.h"



#define pdSECOND pdMS_TO_TICKS(1000)



class Main final
{
public:

    
    
    esp_err_t setup(void);
    void loop(void);

    void read_by_line(const char* base_path,const char* f_name, int n_lines);


    MYFATFS::Fatfs mydisk;
    

};