#include "main.h"


#define LOG_TAG "MAIN"
#define MY_SSID      CONFIG_ESP_WIFI_SSID
#define MY_PASS      CONFIG_ESP_WIFI_PASSWORD

static Main my_main;


extern "C" void app_main(void)
{
    ESP_LOGI(LOG_TAG, "Creating default event loop");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(LOG_TAG, "Initialising NVS");
    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_ERROR_CHECK(my_main.setup());

    while (true)
    {
        my_main.loop();
    }
}

esp_err_t Main::setup(void)
{
    esp_err_t status {ESP_OK};

    ESP_LOGI(LOG_TAG, "Setup!");
    
    status = wifi.init();

    if(ESP_OK == status)
        {status = wifi.wifi_sta_connect();}

    if(ESP_OK == status)
        {status = wifi.begin(MY_SSID,MY_PASS);}
    

    /********************************
     * TEST:
     * get SNTP time evey 3 minutes
     * print time from last update every second
    *********************************/

    ESP_LOGI(LOG_TAG, "Time registered before SNTP start:");
    sntp.print_time(sntp.get_time_now());

    sntp.init(3*one_minute);
    
    return status;
}

void Main::loop(void)
{
    
    sntp.print_time_from_last_update();

    vTaskDelay(pdSECOND);

}