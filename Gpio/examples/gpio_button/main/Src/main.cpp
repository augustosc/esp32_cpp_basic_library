#include "main.h"

#define LOG_LEVEL_LOCAL ESP_LOG_INFO

#define LOG_TAG "MAIN"



/***********************************************
 * TESTS:
 * Led_2 is the bultin led.
 * Connect a Led on pin 18.
 * Connect a push button to ground on pin 4.
************************************************/
 #define TEST_5           ///< rising/falling edge led_2
// #define TEST_6           ///< rising/falling edge led_2/18

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
    {status = butt_0.init();}
    
    return status;
}

/***********************************************
 * TESTS:
 * Led_2 is the bultin led.
 * Connect a Led on pin 18.
 * Connect a push button to ground on pin 4.
************************************************/
void Main::loop(void)
{

/*****************************************************
 * TEST_5:
 * Turn on Led_2 on button_4 RISING EDGE.
 * Turn off Le_2 on button_4 FALLING EDGE.
 * So, when you press the button the led turns on
 * and when you release the button the led turns off
******************************************************/
#ifdef TEST_5// BUTTON RISE & FALLING EDGE
    if (butt_0.isChange())
    {
        led_2.set(butt_0.read_state());
        int st=butt_0.read_state();
        ESP_LOGW(LOG_TAG, "new button state = %d",st);
    }
    vTaskDelay(1);
#endif


/*****************************************************
 * TEST_6:
 * Toggle Led_2 on button_4 RISING EDGE.
 * Toggle Le_18 on button_4 FALLING EDGE.
******************************************************/
#ifdef TEST_6// BUTTON RISE EDGE (OFF TO ON)
    if (butt_0.isChange()){
        if (butt_0.isRisingEdge())
        {
            led_2.toggle();
        }
        if (butt_0.isFallingEdge())
        {
            led_18.toggle();
        }

    }
    vTaskDelay(1);
#endif

}