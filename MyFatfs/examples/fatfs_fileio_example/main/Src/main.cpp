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
    #define BASE_PATH "/fatfs"
    const char* PARTITION_LABEL{"storage"};
    const int MAX_FILES = 20;
    char f_name[40];
    

    esp_err_t status = ESP_OK;

    // mount fatfs
    status = mydisk.mount(BASE_PATH,PARTITION_LABEL,MAX_FILES);

    ///////////////////////////////

    sprintf(f_name,BASE_PATH"/sub1/data.txt");
    ESP_LOGW(LOG_TAG,"Info file %s",f_name);

    mydisk.info( f_name );

    printf("\n");

    sprintf(f_name,BASE_PATH"/sub1");
    ESP_LOGW(LOG_TAG,"ls dir %s",f_name);
    
    mydisk.ls(f_name);

    printf("\n");

    ESP_LOGW(LOG_TAG,"ls dir %s",BASE_PATH);

    mydisk.lstree(BASE_PATH);
    
    printf("\n");

    ///////////////////////////////

    mydisk.unmount();

    ESP_LOGI(LOG_TAG, "Fatfs Unmount!");

    return ESP_OK;
}


void Main::loop(void)
{
   vTaskDelay(5);
}



