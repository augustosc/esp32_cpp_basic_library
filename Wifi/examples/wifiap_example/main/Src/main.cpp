#include "main.h"


#define LOG_TAG "MAIN"

/*******************************
 * TESTS
********************************/
// #define TEST_1       ///< connect wifi AP
#define TEST_2          ///< connect alternate wifi AP/STA

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
        status = wifi.init();




    return status;

}

void Main::loop(void)
{
    static esp_err_t status{ESP_OK};

    /********************************
     * 1. Connect as AP
     * 2. Countdown to disconnect
     * 3. Stop AP
    *********************************/

    if(ESP_OK == status)
        status = wifi.wifi_ap_connect("Esp32","password");

    ESP_LOGI(LOG_TAG, "Check Esp32 on wifi-network: pin = password");

    int cnt{10};
    while (cnt > 0){
        ESP_LOGW(LOG_TAG, "AP will disconnect in %ds",cnt--);
        vTaskDelay(pdSECOND);
    }

    if(ESP_OK == status)
        status = wifi.wifi_stop();

    vTaskDelay(pdSECOND/2);


    /********************************
     * 1. Connect as STA
     * 2. Countdown to disconnect
     * 3. Stop STA
    *********************************/
    if(ESP_OK == status)
        status = wifi.wifi_sta_connect();

    if(ESP_OK == status)
        status = wifi.begin("Casa","pIMa7602944");

    cnt=10;
    while (cnt > 0){
        ESP_LOGE(LOG_TAG, "STA will disconnect in %ds",cnt--);
        vTaskDelay(pdSECOND);
    }

    if(ESP_OK == status)
      status = wifi.wifi_stop();

    

}