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
 *         TEST: Println
 * 1. mount fatfs
 * 2. println n lines of a file
 * 3. unmount fatfs
**************************************************/
    esp_err_t status = ESP_OK;

    #define BASE_PATH "/fatfs"
    const char* PARTITION_LABEL{"storage"};
    const char *FOLDER = "sub/data.txt";
    const int MAX_FILES = 10;

    char fr_name[256]{};
    sprintf(fr_name,"%s/%s",BASE_PATH,FOLDER);

    const int n_lines = 100;
    

    // mount sdcard
    ESP_LOGW(LOG_TAG,"STEP 1: mount fatfs");

    //////////////////////////////
    status = mydisk.mount(BASE_PATH,PARTITION_LABEL,MAX_FILES);
    if (ESP_OK != status){
        ESP_LOGE(LOG_TAG, "Error mounting fatfs");
        return 0;
    }
    //////////////////////////////
    

    ESP_LOGW(LOG_TAG,"STEP 2: print n lines from %s",fr_name);

    //////////////////////////////
    int ret = mydisk.printnln(fr_name,n_lines);
    if (ret == -1){
        ESP_LOGE(LOG_TAG, "Error printnln from %s",fr_name);
        status = mydisk.unmount();
        return 0;
    }
    //////////////////////////////
    

    ESP_LOGW(LOG_TAG,"STEP 3: unmount fatfs");

    //////////////////////////////
    status = mydisk.unmount();

    if(ESP_OK != status)
    {
        ESP_LOGE(LOG_TAG, "Error unmounting fatfs");
        return 0;
    }
    //////////////////////////////

    return ESP_OK;
}


void Main::loop(void)
{
   vTaskDelay(5);
}


