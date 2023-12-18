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
 *         TEST: Read and print file line by line
**************************************************/
    esp_err_t status = ESP_OK;

    const char *BASE_PATH = "/store";
    const char *FOLDER = "sdcard_dir/sub/data.txt";
    const int MAX_FILES = 10;

    char fr_name[256]{};
    sprintf(fr_name,"%s/%s",BASE_PATH,FOLDER);

    const int n_lines = 100;
    

    // mount sdcard
    ESP_LOGW(LOG_TAG,"STEP 1: mount sdcard");

    //////////////////////////////
    status = sd.mount(BASE_PATH,MAX_FILES);
    if (ESP_OK != status){
        ESP_LOGE(LOG_TAG, "Error mounting sdcard");
        return 0;
    }
    //////////////////////////////
    

    ESP_LOGW(LOG_TAG,"STEP 2: print sdcard info");

    //////////////////////////////
    sd.print_info();
    //////////////////////////////

    ESP_LOGW(LOG_TAG,"STEP 3: print n lines from %s",fr_name);

    //////////////////////////////
    int ret = sd.printnln(fr_name,n_lines);
    if (ret == -1){
        ESP_LOGE(LOG_TAG, "Error printnln from %s",fr_name);
        status = sd.unmount();
        return 0;
    }
    //////////////////////////////
    

    ESP_LOGW(LOG_TAG,"STEP 4: unmount sdcard");

    //////////////////////////////
    status = sd.unmount();

    if(ESP_OK != status)
    {
        ESP_LOGE(LOG_TAG, "Error unmounting sdcard");
        return 0;
    }
    //////////////////////////////

    return ESP_OK;
}


void Main::loop(void)
{
   vTaskDelay(5);
}


