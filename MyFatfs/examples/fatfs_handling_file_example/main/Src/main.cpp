#include "main.h"
#include "string.h"

#define LOG_LEVEL_LOCAL ESP_LOG_INFO

#define LOG_TAG "MAIN"


// #define TEST_1         ///< lookup() - file found
// #define TEST_2         ///< lookup() - file not found
 #define TEST_3         ///< write, size, info, read, rename, delete file

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
 *         TEST_1: 
 * ***********************************************
 * 1. mount fatfs
 * 2. lookup file --> found
 * 3. unmount fatfs
**************************************************/
#ifdef TEST_1
    #define BASE_PATH "/fatfs"
    const char* PARTITION_LABEL{"storage"};
    const int MAX_FILES = 20;
    const char* lookup_file = "file11.txt";
    char buf[100] = {0};

    esp_err_t status = ESP_OK;

    // mount fatfs
    ESP_LOGW(LOG_TAG,"STEP 1: mount fatfs");

    ////////////////////////////////////////
    status = mydisk.mount(BASE_PATH,PARTITION_LABEL,MAX_FILES);

    if(ESP_OK != status){
        ESP_LOGE(LOG_TAG,"Error mounting fatfs\n");
        return 0;
    }
    ////////////////////////////////////////


    // lookup file
    ESP_LOGW(LOG_TAG,"STEP 2: look up %s in %s",lookup_file,BASE_PATH);

    ///////////////////////
    status = mydisk.lookup(BASE_PATH,lookup_file,buf);
    if(ESP_OK == status)
        ESP_LOGW(LOG_TAG,"file \"%s\" found at \"%s\"",lookup_file,buf);
    else {
        ESP_LOGE(LOG_TAG,"file \"%s\" not found",lookup_file);
    }
    ///////////////////////

    // unmount fatfs
    ESP_LOGW(LOG_TAG,"STEP 3: unmount fatfs");

    ////////////////////////////////////////
    status = mydisk.unmount();

    if(ESP_OK != status){
        ESP_LOGE(LOG_TAG,"Error unmounting fatfs");
        return 0;
    }
    ////////////////////////////////////////


#endif

/******************************************************** 
 *                TEST_2: 
 *              try to find a non existing file
 * 1. mount sdcard
 * 2. find(f_name) --> not found
 * 3. unmount sdcard
*********************************************************/
#ifdef TEST_2
    #define BASE_PATH "/fatfs"
    const char* PARTITION_LABEL{"storage"};
    const int MAX_FILES = 20;
    const char* lookup_file = "file11.txt";
    char buf[100] = {0};

    esp_err_t status = ESP_OK;

    // mount fatfs
    ESP_LOGW(LOG_TAG,"STEP 1: mount fatfs");

    ///////////////////////
    mydisk.mount(BASE_PATH,PARTITION_LABEL,MAX_FILES);

    if(status != ESP_OK)
    {
        ESP_LOGE(LOG_TAG,"Error mounting sdcard");
        return 0;
    }
    ///////////////////////

    // compose path
    char dir_name[100];
    sprintf(dir_name,"%s/dir2",BASE_PATH);


    ESP_LOGW(LOG_TAG,"STEP 2: look up %s in %s",lookup_file,dir_name);

    ///////////////////////
    status = mydisk.lookup(dir_name,lookup_file,buf);
    if(ESP_OK == status)
        ESP_LOGW(LOG_TAG,"file \"%s\" found : \"%s\"",lookup_file,buf);
    else {
        ESP_LOGE(LOG_TAG,"file \"%s\" not found",lookup_file);
    }
    ///////////////////////

    // unmount fatfs
    ESP_LOGW(LOG_TAG,"STEP 3: unmount fatfs");

    ///////////////////////
    status = mydisk.unmount();
    if(status != ESP_OK)
    {
        ESP_LOGE(LOG_TAG,"Error unmounting fatfs");
        return 0;
    }
    ///////////////////////

#endif


/*************************************************
 *         TEST_3: 
 * ***********************************************
 * 1. mount sdcard
 * 2. write file
 * 3. get file size
 * 4. get file info
 * 5. read file
 * 6. rename file
 * 7. delete file
 * 8. read renamed file
 * 9. unmount sdcard
**************************************************/
#ifdef TEST_3
    #define BASE_PATH "/fatfs"
    const char* PARTITION_LABEL{"storage"};
    const int MAX_FILES = 20;
    const char* fw_name = BASE_PATH"/hello.txt";
    const char* new_name = BASE_PATH"/new_hello.txt";
    const char* content ="Hello esp32 dev kit";
    const unsigned int content_size = strlen(content);
    unsigned int got_f_size{};
    unsigned int total_written{};
    unsigned int total_read{};
    char* data{};
    
    esp_err_t status = ESP_OK;

    // mount fatfs
    ////////////////////////////////////////
    ESP_LOGW(LOG_TAG,"STEP 1: mount fatfs");
    status = mydisk.mount(BASE_PATH,PARTITION_LABEL,MAX_FILES);

    if(ESP_OK != status){
        ESP_LOGE(LOG_TAG,"Error mounting fatfs\n");
        return 1;
    }
    ////////////////////////////////////////


    // write file
    ESP_LOGW(LOG_TAG,"STEP 2: WRITE TEST: -------------------------------------------");

    ////////////////////////////////////////
    total_written = mydisk.write(fw_name,content,content_size);

    if(total_written<content_size){
        ESP_LOGE(LOG_TAG,"Error writing %s\n",fw_name);
        return 1;
    }
    ////////////////////////////////////////

    ESP_LOGW(LOG_TAG,"write file \"%s\": %u bytes",fw_name,total_written);
    printf("\n");


    // get file size
    ESP_LOGW(LOG_TAG,"STEP 3: SIZE TEST: -------------------------------------------");

    ////////////////////////////////////////
    got_f_size = mydisk.size(fw_name);

    if(got_f_size == -1){
         ESP_LOGE(LOG_TAG,"Error getting size of %s\n", fw_name);
         return 1;
    }
    ////////////////////////////////////////

    ESP_LOGW(LOG_TAG,"size of \"%s\": %u bytes",fw_name,got_f_size);
    printf("\n");



    // get file info
    ESP_LOGW(LOG_TAG,"STEP 4: INFO TEST: -------------------------------------------");
    ESP_LOGW(LOG_TAG,"info \"%s\"",fw_name);

    ////////////////////////////////////////
    mydisk.info( fw_name );
    ////////////////////////////////////////

    printf("\n");




    // read file
    ESP_LOGW(LOG_TAG,"STEP 5: READ TEST: -------------------------------------------");
    ESP_LOGW(LOG_TAG,"read \"%s\"",fw_name);

    ////////////////////////////////////////
    total_read = mydisk.read(fw_name,&data);

    if(total_read == -1){
         ESP_LOGE(LOG_TAG,"Error reading %s\n", fw_name);
         return 1;
    }
    ////////////////////////////////////////

    // print read file
    printf("%s contains \"%s\"\n",fw_name,data);
    printf("\n");



    // rename file
    ESP_LOGW(LOG_TAG,"STEP 6: RENAME TEST: -------------------------------------------");
    ESP_LOGW(LOG_TAG,"rename \"%s\" to \"%s\"",fw_name, new_name);

    ////////////////////////////////////////
    int ret = mydisk.ren(fw_name,new_name);

    if (ret == -1){
        ESP_LOGE(LOG_TAG,"Error renaming %s to %s\n", fw_name,new_name);
         return 1;
    }
    ////////////////////////////////////////
    

    printf("\n");



    // read new file
    ESP_LOGW(LOG_TAG,"STEP 7: READ RENAMED TEST: -------------------------------------------");
    ESP_LOGW(LOG_TAG,"read \"%s\"",new_name);
    printf("\n");

    ////////////////////////////////////////
    total_read = mydisk.read(new_name,&data);

    if(total_read == -1){
         ESP_LOGE(LOG_TAG,"Error reading %s\n", fw_name);
         return 1;
    }
    ////////////////////////////////////////

    // print read file
    printf("%s contains \"%s\"\n",new_name,data);
    printf("\n");


    // delete file
    ESP_LOGW(LOG_TAG,"STEP 8: DELETE TEST: -------------------------------------------");
    ESP_LOGW(LOG_TAG,"delete \"%s\"",new_name);

    ////////////////////////////////////////
    ret = mydisk.rmfile(new_name);

    if (ret == -1){
        ESP_LOGE(LOG_TAG,"Error deleting %s\n",new_name);
         return 1;
    }
    ////////////////////////////////////////

    // check file was deleted
    ////////////////////////////////////////
    if(mydisk.exists(new_name) != 0)
    {
        printf("file \"%s\" was deleted\n\n",new_name);
    }
    ////////////////////////////////////////


    free(data);

    ESP_LOGI(LOG_TAG,"-------------------------------------------");
   ////////////////////////////////////////
    ESP_LOGW(LOG_TAG,"STEP 9: unmount fatfs");
    status = mydisk.unmount();

    if(ESP_OK != status){
        ESP_LOGE(LOG_TAG,"Error unmounting fatfs");
    }
    ////////////////////////////////////////


#endif

return ESP_OK;
}


void Main::loop(void)
{
   vTaskDelay(5);
}



