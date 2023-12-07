#include "main.h"
#include "string.h"

#define LOG_LEVEL_LOCAL ESP_LOG_INFO

#define LOG_TAG "MAIN"

//#define TEST_1            ///< Running spiffs_check
                            ///<Disable Watch CPU0 Idle Task in MENUCONFIG
//#define TEST_2            ///< Running wite & read spiffs file
//#define TEST_3            ///< Try to read file longer than the buffer
#define TEST_4            ///< Try to read & log a 12k file in chunks
//#define TEST_5            ///< Get Spiffs info (total & used)

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

    

    const char* partition_label{"storage"};

    status = spf.mount(partition_label,"/spiffs",5,"true");

    if (ESP_OK == status)
        ESP_LOGI(LOG_TAG, "Spiff Mounted!");


#ifdef TEST_1
/*************************************************
 *              Running spiffs_check:
 * Disable Watch CPU0 Idle Task in MENUCONFIG to
 * prevent WDT trigger.
 * 
**************************************************/

   ESP_LOGI(LOG_TAG, "esp_spiffs_check init");

    if (ESP_OK == status)
        status =spf.spiffs_check(partition_label);

   ESP_LOGI(LOG_TAG, "esp_spiffs_check end");
#endif

#ifdef TEST_2
/*************************************************
 *         Running wite & read spiffs file:
**************************************************/

    const char* f_name{"/spiffs/hello.txt"};
    const char content[]={"Hello Sherman"};
    unsigned int content_size = sizeof(content);
    unsigned int got_f_size{};
    unsigned int total_written{};
    unsigned int total_read{};
    char* data{};

    
    ESP_LOGI(LOG_TAG,"CONTENT SIZE = %u bytes",content_size);

    // write file
    total_written = spf.write_new_file(f_name,content,content_size);
    ESP_LOGI(LOG_TAG,"successful file written: TOTAL WRITEN = %u bytes",total_written);

        


    
    if(total_written == content_size)
    {
        // get file size
        got_f_size = spf.get_file_size(f_name);
        ESP_LOGI(LOG_TAG,"successful got file size: FILE SIZE = %u bytes",got_f_size);

    }

    
    if(got_f_size == total_written)
    {
        // create read buffer
        data = (char*)malloc(got_f_size);

        // read file
        total_read = spf.read_file(f_name,data,got_f_size);
    }

    if (total_read == got_f_size)
    {
        // log read file
        ESP_LOGI(LOG_TAG,"succssesful read file: TOTAL READ = %u bytes",total_read);
        ESP_LOGI(LOG_TAG,"data = %s",data);
    }
    free(data);
    
        
    
#endif
#ifdef TEST_3
/********************************************************
 *         Try to read file longer than the buffer
 *             Returns INSUFFICIENT BUFFER SIZE
*********************************************************/
    const char* ff_name{"/spiffs/sub/data.txt"};

    char data[256];
    unsigned int f_size = spf.get_file_size(ff_name);

    ESP_LOGI(LOG_TAG,"FILE SIZE = %u bytes",f_size);

    if (ESP_OK == status)
        unsigned int ret = spf.read_file(ff_name,data,sizeof(data));
#endif

#ifdef TEST_4
/*************************************************
 *         Try to read & log a 12k file in chunks
**************************************************/
    const char* ff_name{"/spiffs/sub/data1.txt"};
    
    unsigned int f_size = spf.get_file_size(ff_name);
    unsigned int ret{};

    ESP_LOGI(LOG_TAG,"FILE SIZE = %u bytes",f_size);

    char* data2 = (char*)malloc(f_size+1);

    if (ESP_OK == status)
        ret=spf.read_file(ff_name,data2,f_size+1);

    ESP_LOGI(LOG_TAG," data len = %u",ret);
        ESP_LOGI(LOG_TAG,"data = %s",data2);
    
    free(data2);
#endif

#ifdef TEST_5
/***********************************************
 *      Get Spiffs info (total & used)
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