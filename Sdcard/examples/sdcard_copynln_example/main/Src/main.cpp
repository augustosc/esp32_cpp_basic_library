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
 *         TEST: 
 * 1. mount sdcard
 * 2. copy n lines from src file to destination file
 * 3. print n lines of destination file
 * 4. delete destination file
 * 5. check destination file was deleted
 * 6. unmount sdcard
**************************************************/
    esp_err_t status = ESP_OK;
    #define BASE_PATH "/store"
    const int MAX_FILES = 20;

    const char* fr_name{BASE_PATH"/sdcard_dir/sub/data.txt"};
    const char* fw_name{BASE_PATH"/sdcard_dir/sub/write_data.txt"};
    const int n_lines = 100;

    // mount sdcard
    ESP_LOGW(LOG_TAG,"STEP 1: mount sdcard");

    //////////////////////////////
    status = sd.mount(BASE_PATH,MAX_FILES);
    if (ESP_OK != status)
    {
        ESP_LOGE(LOG_TAG,"Error mounting sdcard  %s",BASE_PATH);
        return 0;
    }
    //////////////////////////////



    ESP_LOGW(LOG_TAG,"STEP 2: Copying by line: %s ---> %s",fr_name, fw_name);

    ////////////////////////////
    int ret = sd.copynln(fw_name,fr_name,n_lines);

    if (ret == -1){
        ESP_LOGE(LOG_TAG,"Error copying %s\n",fw_name);
         return 0;
    }
    ////////////////////////////

    ESP_LOGW(LOG_TAG,"STEP 3: Printing %d lines of file %s", n_lines,fw_name);

    ////////////////////////////
    sd.printnln(fw_name, n_lines);

    if (ret == -1){
        ESP_LOGE(LOG_TAG,"Error printing %s\n",fw_name);
         return 0;
    }
    ////////////////////////////

    ESP_LOGW(LOG_TAG,"STEP 4: Deleting \"%s\"",fw_name);

    ////////////////////////////////////////
    ret = sd.rmfile(fw_name);

    if (ret == -1){
        ESP_LOGE(LOG_TAG,"Error deleting %s\n",fw_name);
        return 0;
    }
    ////////////////////////////////////////

    // check file was deleted
    ESP_LOGW(LOG_TAG,"STEP 5: Checking \"%s\" was deleted",fw_name);
    ////////////////////////////////////////
    if(sd.exists(fw_name) != 0)
    {
        printf("file \"%s\" was deleted\n\n",fw_name);
    }
    else {
        ESP_LOGE(LOG_TAG,"Error deleting %s\n",fw_name);
        return 0;
    }
    ////////////////////////////////////////


    ESP_LOGW(LOG_TAG,"STEP 6: unmount sdcard");

    ////////////////////////////
    status = sd.unmount();
    if (ESP_OK != status)
    {
        ESP_LOGE(LOG_TAG,"Error unmounting sdcard  %s",BASE_PATH);
        return 0;
    }
    ////////////////////////////


    return ESP_OK;
}


void Main::loop(void)
{
   vTaskDelay(5);
}

