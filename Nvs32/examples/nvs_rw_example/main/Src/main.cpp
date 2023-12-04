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

    /**********************************************
     * TEST: NVS read & write:
     * 1. open nvs namespace
     * 2. write ssid and password to nvs
     * 3. read ssid and password from nvs
     * 4. close nvs namespace
     * 5. log result 
    ***********************************************/

    const char* nspace{"ESP32testrw"};

    const char* key_ssid{"ssid"};
    const char* key_pass{"password"};

    const char ssid[32] = {"1234"};
    const char pass[64] = {"pass1234"};
    
    char output_ssid[32]{0};
    char output_pass[64]{0};
    size_t len;

    // open nvs namespace
    ESP_LOGW(LOG_TAG, "openning nvs namespace!");
    status = nvs.init(nspace);

    // write ssid to nvs
    ESP_LOGW(LOG_TAG, "writing ssid to nvs!");
    if(ESP_OK == status)
        status = nvs.write(key_ssid,ssid,sizeof(ssid));

    // write password to nvs
    ESP_LOGW(LOG_TAG, "writing password to nvs!");
    if(ESP_OK == status)
        status = nvs.write(key_pass,pass,sizeof(pass));

    // read ssid from nvs
    ESP_LOGW(LOG_TAG, "reading ssid from nvs!");
    len=sizeof(ssid);
    if(ESP_OK == status)
        status = nvs.read(key_ssid,output_ssid,len);

    // read password from nvs
    ESP_LOGW(LOG_TAG, "reading password from nvs!");
    len=sizeof(pass);
    if(ESP_OK == status)
        status = nvs.read(key_pass,output_pass,len);

    // close nvs namespace
    ESP_LOGW(LOG_TAG, "closing nvs namespace!");
    nvs.close();

    // log SSID & PASSWORd
    ESP_LOGW(LOG_TAG," SSID = %s\tPASSWORD = %s", output_ssid,output_pass);

    
    return status;
}

void Main::loop(void)
{
    

    vTaskDelay(pdSECOND);

}