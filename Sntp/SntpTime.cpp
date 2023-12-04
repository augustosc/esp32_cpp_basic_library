#include "SntpTime.h"

namespace SNTP {
    SemaphoreHandle_t got_time_semaphore;
    time_t Sntp::last_time_update{};



//***********************************************

void Sntp::init(uint32_t sync_interval)
{
    got_time_semaphore = xSemaphoreCreateBinary();

    ///////////////////////////////////////////////////////////////////
    // GET POSIX TZ STRINGS ON:
    // https://community.progress.com/s/article/P129473
    // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
    // https://www.timeanddate.com/time/zones/
    ///////////////////////////////////////////////////////////////////
    
    // setenv("TZ", "BRST+3BRDT+2,M10.3.0,M2.3.0", 1);
    setenv("TZ", "UTC+3", 1);
    tzset();

    ESP_LOGI(TAG, "%s:%d Calling sntp_set_sync_mode",__func__, __LINE__);
    sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);

    ESP_LOGI(TAG, "%s:%d Calling esp_sntp_setservername",__func__, __LINE__);
    sntp_setservername(0, "time.google.com");
    sntp_setservername(1, "pool.ntp.org");

    ESP_LOGI(TAG, "%s:%d set sntp call_back",__func__, __LINE__);
    sntp_set_time_sync_notification_cb(on_got_time);

    ESP_LOGI(TAG, "%s:%d set sntp set_sync_interval",__func__, __LINE__);
    sntp_set_sync_interval(sync_interval);

    ESP_LOGI(TAG, "%s:%d Calling sntp_init.",__func__, __LINE__);
    esp_sntp_init();

    ESP_LOGI(TAG, "%s:%d Waiting for first time from SNTP",__func__, __LINE__);
    xSemaphoreTake(got_time_semaphore, portMAX_DELAY);
}



//***********************************************

time_t Sntp::get_time_now(){
    return time(NULL);
}



//***********************************************

void Sntp::print_time(time_t t)
{
    // time_t now = 0;
    // time(&now);
    struct tm *time_info = localtime(&t);

    char time_buffer[50];
    strftime(time_buffer, sizeof(time_buffer), "%c", time_info);
    ESP_LOGI(TAG, "************ %s ***********", time_buffer);
}



//***********************************************

void Sntp::on_got_time(struct timeval *tv)
{
    ESP_LOGI(TAG, "Got time from SNTP: %lld", tv->tv_sec);

    print_time(tv->tv_sec);
    last_time_update = tv->tv_sec;
    
    xSemaphoreGive(got_time_semaphore);
}


//***********************************************

time_t Sntp::get_time_from_last_update(){

    return  get_time_now() - last_time_update;

}


//***********************************************

void Sntp::print_time_from_last_update(){

    ESP_LOGI(TAG, "Time from last SNTP update:");
    print_elapsed_thms (get_time_from_last_update());
}


//***********************************************

void Sntp::print_elapsed_thms(time_t t){

    Time_HMS& thms = get_elapsed_thms(t);
    ESP_LOGI(TAG, "*********** %dh%dm%ds ***********", thms.hours, thms.minutes, thms.seconds);

}


//***********************************************

Time_HMS& Sntp::get_elapsed_thms(time_t t){

    static Time_HMS time_hms{};
    int remainder = t % 3600;
    time_hms.hours = t/3600;
    time_hms.minutes = remainder / 60;
    time_hms.seconds = remainder % 60;
    return time_hms;
    
}


}// namespace SNTP
