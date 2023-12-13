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
 *         Write file line by line
 * 1. copy fr_name to fw_name
 * 2. read and print fw_name
**************************************************/
    esp_err_t status = ESP_OK;
    #define BASE_PATH "/fatfs"
    const char* PARTITION_LABEL{"storage"};
    const int MAX_FILES = 10;
    const char* fr_name{BASE_PATH"/sub/data.txt"};
    const char* fw_name{BASE_PATH"/sub/write_data.txt"};
    const int n_lines = 100;

    // mount fatfs with MAX_FILES files
    status = mydisk.mount(BASE_PATH,PARTITION_LABEL,MAX_FILES);
    if (ESP_OK != status)
    {
        ESP_LOGE(LOG_TAG,"Error mounting fatfs  %s",PARTITION_LABEL);
        return ESP_OK;
    }

    ESP_LOGI(LOG_TAG, "Fatfs Mounted!");

    ESP_LOGW(LOG_TAG,"Copying by line: %s ---> %s",fr_name, fw_name);
    mydisk.copynln(fw_name,fr_name,n_lines);

    ESP_LOGW(LOG_TAG,"Reading and printing by line file %s", fw_name);
    mydisk.readnln(fw_name, n_lines);

    mydisk.unmount();

    ESP_LOGI(LOG_TAG, "Fatfs Unmount!");

    return ESP_OK;
}


void Main::loop(void)
{
   vTaskDelay(5);
}

