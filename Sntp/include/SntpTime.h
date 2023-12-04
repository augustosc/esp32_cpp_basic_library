#pragma once

/***************************************************
 * Use SNTP class:
 * 1. create sntp singleton object: 
 *   SNTP::Sntp& sntp { SNTP::Sntp::get_instance() };
 * 2. call sntp init: sntp.init(3*one_minute);
*****************************************************/

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <time.h>
#include "esp_sntp.h"

namespace SNTP {
    
    #define TAG "NTP TIME"

    struct Time_HMS{
        int hours;
        int minutes;
        int seconds;
    };

class Sntp {
private:
    static time_t last_time_update;

    //***********************************************
    /// @brief callback to sntp update
    /// @param tv struct to receive time data
    static void on_got_time(struct timeval *tv);


    //***********************************************
    /// @brief print time elapsed in hh:mm:ss
    /// @param t time
    static void print_elapsed_thms(time_t t);


    //***********************************************
    /// @brief get time elapsed in hh:mm:ss
    /// @param t time
    static Time_HMS& get_elapsed_thms(time_t t);

public:

    //***********************************************
    /// @brief get current time
    /// @return current time
    static time_t get_time_now(void);
    

    //***********************************************
    /// @brief print time
    /// @param t time to be printed
    static void print_time(time_t t);


    //***********************************************
    /// @brief get duration from last sntp update
    /// @return duration
    static time_t get_time_from_last_update(void);


    //***********************************************
    /// @brief print duration from last sntp update
    static void print_time_from_last_update(void);


    //***********************************************
    /// @brief init sntp
    /// @param sync_interval update interval
    void init(uint32_t sync_interval = 60*60*1000);


    /// @brief get sntp singleton instance
    /// @return instance
    static Sntp& get_instance(void)
    {
        static Sntp instance;
        return instance;
    }
};

}
