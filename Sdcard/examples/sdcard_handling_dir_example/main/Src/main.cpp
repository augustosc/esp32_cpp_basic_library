#include "main.h"

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
    esp_err_t status {ESP_OK};

    
/******************************************************** 
 *               TEST: COPY DIRECTORY TREE
 * 1. mount sdcard
 * 2. print sdcard info
 * 3. list directory tree from BASE_PATH
 * 4. copy directory tree from dir2 to dir20
 * 5. list directory tree from BASE_PATH
 * 6. remove directory tree from dir20
 * 7. list directory tree from BASE_PATH
 * 8. unmount sdcard
*********************************************************/
    #define BASE_PATH "/store"
    const int MAX_FILES = 20;
    const char* PATH = BASE_PATH"/dir1/dir2";
    const char* DST_PATH = BASE_PATH"/dir1/dir20";


    // mount fatfs

    ESP_LOGW(LOG_TAG,"STEP 1: mount sdcard");

    /////////////////////////////////
    status = sd.mount(BASE_PATH,MAX_FILES);

    if(status != ESP_OK)
    {
        ESP_LOGE(LOG_TAG,"Error mounting sdcard");
        return 0;
    }
    /////////////////////////////////

    ESP_LOGW(LOG_TAG, "STEP2: Sdcard info:");

    /////////////////////////////////
    sd.print_info();
    /////////////////////////////////

    // list directory tree from BASE_PATH
    ESP_LOGW(LOG_TAG,"STEP 3: list directory tree from %s",BASE_PATH);

    ///////////////////////
    sd.lstree(BASE_PATH);
    ///////////////////////

    vTaskDelay(100);        ///< wait printf conclude

    // copy directory tree to BASE_PATH/dir20
    ESP_LOGW(LOG_TAG,"STEP 4: copy directory tree from %s to %s",PATH,DST_PATH);

    ///////////////////////
    sd.cptree(PATH,DST_PATH,0777);
    ///////////////////////

    vTaskDelay(100);        ///< wait printf conclude

    // list directory tree from BASE_PATH
    ESP_LOGW(LOG_TAG,"STEP 5: list directory tree from %s",BASE_PATH);

    ///////////////////////
    sd.lstree(BASE_PATH);
    ///////////////////////


    ESP_LOGW(LOG_TAG,"STEP 6: remove directory tree from %s",DST_PATH);

    ///////////////////////
    int ret = sd.rmtree(DST_PATH);

    if(ret==-1)
    {
        ESP_LOGE(LOG_TAG,"Error removing dir tree from %s",DST_PATH);
        return 0;
    }
    ///////////////////////

    // list directory tree from BASE_PATH
    ESP_LOGW(LOG_TAG,"STEP 7: list directory tree from %s",BASE_PATH);

    ///////////////////////
    sd.lstree(BASE_PATH);
    ///////////////////////

    // unmount fatfs
    ESP_LOGW(LOG_TAG,"STEP 8: unmount sdcard");

    ///////////////////////
    status = sd.unmount();

    if(status != ESP_OK)
    {
        ESP_LOGE(LOG_TAG,"Error unmounting sdcard");
        return 0;
    }
    ///////////////////////



    while(1) vTaskDelay(1);
    
    return status;
}

void Main::loop(void)
{


}

