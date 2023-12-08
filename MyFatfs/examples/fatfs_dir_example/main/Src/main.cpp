#include "main.h"

#define LOG_LEVEL_LOCAL ESP_LOG_INFO

#define LOG_TAG "MAIN"

 #define TEST_1         ///< list_all_entries()
// #define TEST_2         ///< look_up_file() file found
// #define TEST_3         ///< look_up_file() file not found




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

    static const char *BASE_PATH = "/myfatfs";
    static const char *PARTITION_LABEL = "storage";
    

    ESP_LOGI(LOG_TAG, "Setup!");

    mydisk.init(BASE_PATH,PARTITION_LABEL,NULL);

#ifdef TEST_1
    mydisk.list_all_entries(BASE_PATH);
#endif

#ifdef TEST_2
    const char* lookup_file = "file11.txt";
    char buf[100] = {0};

    status = mydisk.look_up_file(BASE_PATH,lookup_file,buf);
    if(ESP_OK == status)
        ESP_LOGW(LOG_TAG,"file \"%s\" found : \"%s\"",lookup_file,buf);
    else {
        ESP_LOGE(LOG_TAG,"file \"%s\" not found",lookup_file);
    }
#endif

#ifdef TEST_3
    const char* lookup_file = "file11.txt";
    char buf[100] = {0};

    char dir_name[100];
    sprintf(dir_name,"%s/dir2",BASE_PATH);

    status = mydisk.look_up_file(dir_name,lookup_file,buf);
    if(ESP_OK == status)
        ESP_LOGW(LOG_TAG,"file \"%s\" found : \"%s\"",lookup_file,buf);
    else {
        ESP_LOGE(LOG_TAG,"file \"%s\" not found",lookup_file);
    }
#endif


    while(1) vTaskDelay(1);
    
    return status;
}

void Main::loop(void)
{


}