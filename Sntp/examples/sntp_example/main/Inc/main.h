#pragma once

// base cpp esp-idf includes
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "nvs_flash.h"


// class includes
#include "Wifi.h"
#include "SntpTime.h"

#define LOG_LEVEL_LOCAL ESP_LOG_INFO

#define pdSECOND pdMS_TO_TICKS(1000)
#define pdMINUTE pdMS_TO_TICKS(60*1000);

constexpr uint32_t one_second{1000};
constexpr uint32_t one_minute{60*1000};


class Main final
{
public:
    esp_err_t setup(void);
    void loop(void);

/*************************************************
 * create objects here as class Main members.
 * use them inside Main::setup and/or Main::loop
**************************************************/
    WIFI::Wifi wifi;
    
    //singleton instance
    SNTP::Sntp& sntp { SNTP::Sntp::get_instance() };

    

};