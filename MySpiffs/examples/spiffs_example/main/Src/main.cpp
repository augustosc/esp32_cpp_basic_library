#include "main.h"
#include "string.h"

#define LOG_LEVEL_LOCAL ESP_LOG_INFO

#define LOG_TAG "MAIN"

/************************************************
 *           CHOOSE A TEST:
*************************************************/
#define TEST_1            ///< Running spiffs_check
                            ///<Disable Watch CPU0 Idle Task in MENUCONFIG
//#define TEST_2            ///< Running wite & read spiffs file
//#define TEST_3            ///< Try to read & print a 12k file in chunks
//#define TEST_4            ///< Get Spiffs info (total & used)


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

    

    const char* PARTITION_TABLE{"storage"};
    const char* PATH {"/spiffs"};
    const int MAX_FILES = 10;

    status = spf.mount(PARTITION_TABLE,PATH,MAX_FILES,"true");

    if (ESP_OK == status)
        ESP_LOGI(LOG_TAG, "Spiff Mounted!");


#ifdef TEST_1
/*************************************************
 *              TEST_1: Running spiffs_check:
 * Disable Watch CPU0 Idle Task in MENUCONFIG to
 * prevent WDT trigger.
 * 
**************************************************/

   ESP_LOGI(LOG_TAG, "esp_spiffs_check init");

    if (ESP_OK == status)
        status =spf.spiffs_check(PARTITION_TABLE);

   ESP_LOGI(LOG_TAG, "esp_spiffs_check end");
#endif

#ifdef TEST_2
/*************************************************
 *         TEST_2: Running wite & read spiffs file:
**************************************************/

    const char* f_name{"/spiffs/hello.txt"};
    const char content[]={"Hello World!"};
    const unsigned int content_size = sizeof(content);
    unsigned int got_f_size{};
    unsigned int total_written{};
    unsigned int total_read{};
    char* data{};

    
    ESP_LOGW(LOG_TAG,"content size = %u bytes",content_size);

    // write file

    ///////////////////////////
    total_written = spf.write(f_name,content,content_size);
    ///////////////////////////

    if (total_written != content_size)
    {
        ESP_LOGE(LOG_TAG,"Error writing file %s",f_name);
        return 0;
    }

    ESP_LOGW(LOG_TAG,"successful file %s written: %u bytes",f_name,total_written);
   
    // get file size
    ///////////////////////////
    got_f_size = spf.size(f_name);
    ///////////////////////////

    if (got_f_size != total_written)
    {
        ESP_LOGE(LOG_TAG,"Error getting file %s size",f_name);
            return 0;
    }

    ESP_LOGW(LOG_TAG,"successful got file %s size: %u bytes",f_name,got_f_size);


    // read file
    ///////////////////////////
    total_read = spf.read(f_name,&data);
    ///////////////////////////

    if (total_read != got_f_size)
    {
        ESP_LOGE(LOG_TAG,"Error reading file %s",f_name);
        return 0;
    }

    // log read file
    ESP_LOGW(LOG_TAG,"succssesful read file %s : %u bytes",f_name,total_read);
    ESP_LOGI(LOG_TAG,"file content = %s",data);

    free(data);
    
#endif

#ifdef TEST_3
/*************************************************
 *      TEST: Try to read & print a 12k file in chunks
**************************************************/
    unsigned int len{};
    char* buffer;
    const char* f_name{"/spiffs/sub/data1.txt"};
    
    unsigned int f_size = spf.size(f_name);
    
    ESP_LOGW(LOG_TAG,"file %s size = %u bytes",f_name,f_size);

    if (ESP_OK == status)
        len=spf.read(f_name,&buffer);

    ESP_LOGI(LOG_TAG," reading %s: %u bytes",f_name,len);
    
    printf("%s",buffer);
    
    free(buffer);

#endif

#ifdef TEST_4
/***********************************************
 *      TEST: Get Spiffs info (total & used)
************************************************/
    struct SPIFFS::Spiffs_info info;
    info = spf.get_spiffs_info();
    ESP_LOGI(LOG_TAG,"TOTAL = %u",info.total);
    ESP_LOGI(LOG_TAG,"USED = %u",info.used);

#endif
    spf.unmount();

    ESP_LOGI(LOG_TAG, "Spiff Unmount!");

    
    return status;
}


void Main::loop(void)
{
   vTaskDelay(5);
}