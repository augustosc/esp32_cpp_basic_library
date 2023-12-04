#include "main.h"


#define LOG_TAG "MAIN"

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


    if(ESP_OK == status)
    {status = wifi.init();}

    if(ESP_OK == status)
    {status = wifi.wifi_sta_connect();}

    /****************************************************
     * TEST
     * get SSID & PASSWORD from smartconfig/ESPTOUCH App
    *****************************************************/
   
    /////////////////////////////////////////////
    ESP_LOGI(LOG_TAG, "Calling smartconfig!");
    status = smartconfig.init();
    /////////////////////////////////////////////


    if(ESP_OK == status)
    {   
        SMARTCFG::Credentials credential{0};
        credential = smartconfig.get_credentials();

        ESP_LOGW(LOG_TAG, "ssid = %s\t password = %s",credential.ssid, credential.password);

        wifi.begin((char*)credential.ssid,(char*)credential.password);

    }

    ESP_LOGI(LOG_TAG, "Time before SNTP update:");
    sntp.print_time(sntp.get_time_now());

    sntp.init(3*one_minute);

    
    return status;
}

void Main::loop(void)
{
    
    sntp.print_time_from_last_update();

    vTaskDelay(pdSECOND);

}