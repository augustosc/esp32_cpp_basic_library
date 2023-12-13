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

    /**********************************************
     * TEST: Connect ESP32 as station to your AP
    ***********************************************/
    
    // init wifi
    status = wifi.init();

    // connect as Station
    if(ESP_OK == status)
        {status = wifi.wifi_sta_connect();}

    // wifi begin
    if(ESP_OK == status)
        {status = wifi.begin(MY_SSID,MY_PASS);}
    
    if(ESP_OK == status)
        ESP_LOGW(LOG_TAG, "******* WIFI STA CONNECTED!");

    
    return status;
}

void Main::loop(void)
{


    vTaskDelay(pdSECOND);

}