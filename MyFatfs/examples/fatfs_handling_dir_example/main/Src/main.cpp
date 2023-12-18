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

/******************************************************** 
 *               TEST: COPY DIRECTORY TREE
 * 1. mount sdcard
 * 2. list directory tree from BASE_PATH
 * 3. copy directory tree from dir2 to dir20
 * 4. list directory tree from BASE_PATH
 * 5. remove directory tree from dir20
 * 6. list directory tree from BASE_PATH
 * 7. unmount sdcard
*********************************************************/
    #define BASE_PATH "/fatfs"
    const char* PARTITION_LABEL{"storage"};
    const int MAX_FILES = 20;
    const char* PATH = BASE_PATH"/dir2";
    const char* DST_PATH = BASE_PATH"/dir20";

    esp_err_t status {ESP_OK};


    // mount fatfs

    ESP_LOGW(LOG_TAG,"STEP 1: mount fatfs");

    /////////////////////////////////
    status = mydisk.mount(BASE_PATH,PARTITION_LABEL,MAX_FILES);

    if(status != ESP_OK)
    {
        ESP_LOGE(LOG_TAG,"Error mounting fatfs");
        return 0;
    }
    /////////////////////////////////


    // list directory tree from BASE_PATH
    ESP_LOGW(LOG_TAG,"STEP 2: list directory tree from %s",BASE_PATH);

    ///////////////////////
    mydisk.lstree(BASE_PATH);
    ///////////////////////

    vTaskDelay(100);        ///< wait printf conclude

    // copy directory tree to BASE_PATH/dir20
    ESP_LOGW(LOG_TAG,"STEP 3: copy directory tree from %s to %s",PATH,DST_PATH);

    ///////////////////////
    mydisk.cptree(PATH,DST_PATH,0777);
    ///////////////////////

    vTaskDelay(100);        ///< wait printf conclude

    // list directory tree from BASE_PATH
    ESP_LOGW(LOG_TAG,"STEP 4: list directory tree from %s",BASE_PATH);

    ///////////////////////
    mydisk.lstree(BASE_PATH);
    ///////////////////////


    ESP_LOGW(LOG_TAG,"STEP 5: remove directory tree from %s",DST_PATH);

    ///////////////////////
    int ret = mydisk.rmtree(DST_PATH);

    if(ret==-1)
    {
        ESP_LOGE(LOG_TAG,"Error removing dir tree from %s",DST_PATH);
        return 0;
    }
    ///////////////////////

    // list directory tree from BASE_PATH
    ESP_LOGW(LOG_TAG,"STEP 6: list directory tree from %s",BASE_PATH);

    ///////////////////////
    mydisk.lstree(BASE_PATH);
    ///////////////////////

    // unmount fatfs
    ESP_LOGW(LOG_TAG,"STEP 7: unmount fatfs");

    ///////////////////////
    status = mydisk.unmount();

    if(status != ESP_OK)
    {
        ESP_LOGE(LOG_TAG,"Error unmounting fatfs");
        return 0;
    }
    ///////////////////////



    while(1) vTaskDelay(1);
    
    return status;
}

void Main::loop(void)
{


}