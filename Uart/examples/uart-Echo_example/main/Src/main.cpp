#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"


#define LOG_TAG "MAIN"

 #define TEST_1         ///< Uart 0 - read from monitor
// #define TEST_2       ///< Uart 1/2 - echo


static Main my_main;


extern "C" void app_main(void)
{
    ESP_LOGI(LOG_TAG, "Creating default event loop");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(LOG_TAG, "Initialising NVS");
    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_ERROR_CHECK(my_main.setup());

    while (true)
    {
        my_main.loop();
    }
}


esp_err_t Main::setup(void)
{
    esp_err_t status {ESP_OK};

    ESP_LOGI(LOG_TAG, "Setup!");

    vTaskDelay(500/portTICK_PERIOD_MS);

#ifdef TEST_1
    if (ESP_OK == status)
    {U0.begin(115200,1,3);}
    if (ESP_OK == status)
    {U0.flush();}
#endif

#ifdef TEST_2
    if (ESP_OK == status)
    {U1.begin(115200,22,23);}

    if (ESP_OK == status)
    {U2.begin(115200,17,16);}

    if (ESP_OK == status)
    {U1.flush(); U2.flush();}
#endif


#ifdef TEST_1
    ESP_LOGI(LOG_TAG, "TEST_1: digite uma mensagem ...");
    int len{};
    const int buf_size{1024};
    //uint8_t data[buf_size]{};
    uint8_t *data = (uint8_t *) malloc(buf_size);

    for(;;)
    {
        len = U0.read(data,buf_size,5);

        if (len >0)
        {
            len = U0.write(data,len);
        }

        vTaskDelay(1); // prevent watchdog timeout
    }
#endif

#ifdef TEST_2
/**************************************
 * Conect pin 17 (TX2) to pin 23 (RX1)
 * Conect pin 16 (RX2) to pin 22 (TX1)
***************************************/
    
    int len{};
    const int buf_size{100};

    //char incoming_msg[buf_size];
    char *incoming_msg = (char *) malloc(buf_size);

    const char msg_from_U1[] = "hello from U1";
    const char msg_from_U2[] = "world from U2";

    
    for(;;)
    {
        ESP_LOGI(LOG_TAG, "TEST_2!");
        // write U1 
        len = U1.write(msg_from_U1,sizeof(msg_from_U1));
        
        if (len>0)
        {
            // read U2
            len = U2.read((uint8_t*)incoming_msg,buf_size,500);

            // print result
            if (len>0)
            {printf("U2 received: %s\n", incoming_msg);}
        }
        

        // write U2
        len = U2.write(msg_from_U2,sizeof(msg_from_U2));

        
        if (len>0)
        {
            // read U1
            len = U1.read((uint8_t*)incoming_msg,buf_size,500);

            // print result
            if (len>0)
            {printf("U1 received: %s\n\n", incoming_msg);}
        }

        vTaskDelay(pdSECOND); 
    }

#endif

    
    return status;
}

void Main::loop(void)
{
    

}

