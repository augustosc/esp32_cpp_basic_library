#include "main.h"
#include "string.h"

#define LOG_LEVEL_LOCAL ESP_LOG_INFO

#define LOG_TAG "MAIN"


static Main my_main;


extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(my_main.setup());

    while (true)
    {
        my_main.loop();
    }
}

esp_err_t Main::setup(void)
{

/*************************************************
 *         TEST: Read file line by line
**************************************************/
    #define BASE_PATH "/spiffs"
    const char* PARTITION_LABEL{"storage"};
    const int MAX_FILES = 10;
    const char* fr_name{BASE_PATH"/sub/data.txt"};
    const int n_lines = 100;

    esp_err_t status = ESP_OK;

    
    ESP_LOGW(LOG_TAG,"Reading %d lines of %s",n_lines,fr_name);

    // mount spiff
    status = spf.mount(PARTITION_LABEL,BASE_PATH,MAX_FILES,"true");

    if (ESP_OK != status)
    {
        ESP_LOGE(LOG_TAG,"Error mounting spiffs  %s",PARTITION_LABEL);
        return ESP_OK;
    }

    ESP_LOGI(LOG_TAG, "Spiff Mounted!");
        
    spf.readnln(fr_name,n_lines);

    spf.unmount();

    ESP_LOGI(LOG_TAG, "Spiff Unmount!");

    
    return ESP_OK;
}


void Main::loop(void)
{
   vTaskDelay(5);
}

