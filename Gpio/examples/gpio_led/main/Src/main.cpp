#include "main.h"

#define LOG_LEVEL_LOCAL ESP_LOG_INFO

#define LOG_TAG "MAIN"



/***********************************************
 * TESTS:
 * Led_2 is the bultin led.
 * Connect a Led on pin 18.
 * Connect a push button to ground on pin 4.
************************************************/
// #define TEST_0           ///< set()
// #define TEST_1             ///< on()/off()
// #define TEST_2           ///< toggle()
// #define TEST_3           ///< toggle() led_18
 #define TEST_4           ///< blink()


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

    ESP_LOGI(LOG_TAG, "Setup!!!!!");
    
    status = led_2.init();

    if(ESP_OK == status)
    {status = led_18.init();}

    if(ESP_OK == status)
    {status = butt_4.init();}
    
    return status;
}

/***********************************************
 * TESTS:
 * Led_2 is the bultin led.
 * Connect a Led on pin 18.
 * Connect a push button to ground on pin 4.
************************************************/

/*****************************************
 * TEST_0:
 * Blink Led_2 with member function set()
******************************************/
void Main::loop(void)
{
    #ifdef TEST_0
    ESP_LOGI(LOG_TAG, "LED2 SET 1");
    led_2.set(1);
    vTaskDelay(pdSECOND);
    ESP_LOGI(LOG_TAG, "LED2 SET 0");
    led_2.set(0);
    vTaskDelay(pdSECOND);
#endif

/**************************************************
 * TEST_1:
 * Blink Led_2 with member functions on() & off()
**************************************************/
#ifdef TEST_1 // ON & OFF
    ESP_LOGI(LOG_TAG, "LED2 ON");
    led_2.on();
    vTaskDelay(pdSECOND);
    ESP_LOGI(LOG_TAG, "LED2 OFF");
    led_2.off();
    vTaskDelay(pdSECOND);
#endif


/*****************************************
 * TEST_2:
 * Blink Led_2 with member function toggle()
******************************************/
#ifdef TEST_2 // TOGGLE
    ESP_LOGI(LOG_TAG, "LED2 TOGGLE");
    led_2.toggle();
    vTaskDelay(pdSECOND);
#endif


/*****************************************************
 * TEST_3:
 * Blink Led_2 & Led_18 with member function toggle()
******************************************************/
#ifdef TEST_3 // TOGGLE LED2 & LED18
    ESP_LOGI(LOG_TAG, "LED2 & LED18 TOGGLE");
    led_2.toggle();
    led_18.toggle();
    vTaskDelay(pdSECOND);
#endif


/*****************************************************
 * TEST_4:
 * Blink Led_2 & Led_18 with member function blink().
 * Using differents delays.
******************************************************/
#ifdef TEST_4 // BLINK
    led_2.blink(500);
    led_18.blink(2000);
    vTaskDelay(1);
#endif





}