#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"


#define LOG_TAG "MAIN"

static Main my_main;




extern "C" void app_main(void)
{
    ESP_LOGI(LOG_TAG, "Creating default event loop");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(my_main.setup());

    while (true)
    {
        my_main.loop();
    }
}


esp_err_t Main::setup(void)
{
    #define BASE_PATH "/store"
    const char* PARTITION_LABEL{"storage"};
    const int MAX_FILES = 20;
    esp_err_t status {ESP_OK};
    const char* prompt = "\n>> ";



    if (ESP_OK == status)
    {U0.begin(115200,1,3);}
    if (ESP_OK == status)
    {U0.flush();}

    /////////////////////////////////
    status = mydisk.mount(BASE_PATH,PARTITION_LABEL,MAX_FILES);

    if(status != ESP_OK)
    {
        ESP_LOGE(LOG_TAG,"Error mounting fatfs");
        return 0;
    }
    /////////////////////////////////

    sh.console(prompt);

    ///////////////////////
    status = mydisk.unmount();

    if(status != ESP_OK)
    {
        ESP_LOGE(LOG_TAG,"Error unmounting fatfs");
        return 0;
    }
    ///////////////////////

    return ESP_OK;
}

void Main::loop(void)
{

  vTaskDelay(1);  

}

