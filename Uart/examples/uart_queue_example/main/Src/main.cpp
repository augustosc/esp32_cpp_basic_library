#include "main.h"
#include <string.h>


/***************
 * CHOOSE TEST
****************/
// #define TEST_1         ///< Uart 0 - read from monitor
#define TEST_2       ///< Uart 1/2 - echo


#define LOG_TAG "MAIN"

static Main my_main;

/*****************************************
 * app_main:
 * Create my_main.setup and my_main.loop.
******************************************/
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


/*************************************
 *              Main:setup:
**************************************/
esp_err_t Main::setup(void)
{
    esp_err_t status {ESP_OK};

    ESP_LOGI(LOG_TAG, "Setup!");
    

    vTaskDelay(500/portTICK_PERIOD_MS);



#ifdef TEST_1
    ESP_LOGI(LOG_TAG, "TEST_1: digite uma mensagem ....");
    vTaskDelay(500/portTICK_PERIOD_MS);

    U0.begin(115200,1,3);

    U0.create_uart_event_task("uart_event",10,2048);

    //xTaskCreate(write_task,"write_task",2048,(void*)&U0,5,NULL);

    xTaskCreate(read_task,"read_task",2048,(void*)&U0,5,NULL);

#endif

#ifdef TEST_2
/**************************************
 * U1(txPin=22,rxPin=23)
 * U2(txPin=17,rxPin=16)
 * Conect TX2 (17) to RX1 (23)
 * Conect RX2 (16) to TX  (22)
***************************************/
    // Initialise Uart1
    ESP_LOGI(LOG_TAG, "TEST_2: conecte U1 e U2 ....");
    vTaskDelay(500/portTICK_PERIOD_MS);
    if(ESP_OK == status)
        U1.begin(115200,22,23);

    // Initialise Uart1
    if(ESP_OK == status)
        U2.begin(115200,17,16);


    // Create Uart1 event task
    if(ESP_OK == status)
        U1.create_uart_event_task("uart_event",10,2048);

    // Create Uart2 event task
    if(ESP_OK == status)
        U2.create_uart_event_task("uart_event",10,2048);

        

    /******************************************************
     * Observe xTaskCreate returns pdPASS as successful.
     * As pdPASS == !ESP_OK, we need to invert xTaskCreate 
     * return data.
    *******************************************************/

    // Create Uart2 read task
    if(ESP_OK == status)
        status = !xTaskCreate(read_task,"read_task_U2",2048,(void*)&U2,5,NULL);

    // Create Uart1 read task
    if(ESP_OK == status)
        status = !xTaskCreate(read_task,"read_task_U1",2048,(void*)&U1,5,NULL);


    // Create Uart1 write Task
    if(ESP_OK == status)
        status = !xTaskCreate(write_task,"write_task_U1",2048,(void*)&U1,5,NULL);

    // Create Uart2 write Task
    if(ESP_OK == status)
        status= !xTaskCreate(write_task,"write_task_U2",2048,(void*)&U2,5,NULL);
#endif
    
    return status;
}



/*************************************
 *              Main:loop
**************************************/
void Main::loop(void)
{
    vTaskDelay(1000/portTICK_PERIOD_MS);

} 







#ifdef TEST_1
void Main::write_task(void* parm)
{
    ESP_LOGI(LOG_TAG,"write_task running");

    UART::Uart uart_obj = *(static_cast<UART::Uart*>(parm));

    int len{};
    int cnt{};
    char buff[20];

    for(;;)
    {
        cnt++;
        sprintf(buff, "hello #%d\n",cnt);

        len = uart_obj.write(buff,strlen(buff));

       vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);

}



void Main::read_task(void* parm)
{
    ESP_LOGI(LOG_TAG,"read_task running");

    UART::Uart uart_obj = *(static_cast<UART::Uart*>(parm));

    int len{};
    const int buf_size{200};
    uint8_t *data = (uint8_t *) malloc(buf_size);

    for(;;)
    {
        xSemaphoreTake(uart_obj.uart_data_received, portMAX_DELAY);

        len = uart_obj.read(data,buf_size,5);

        if (len >0)
        {
            len = uart_obj.write(data,len);
        }

        vTaskDelay(1); // prevent watchdog timeout
    }
    
    free(data);
    vTaskDelete(NULL);

}
#endif


#ifdef TEST_2
/**************************************
 * U1(txPin=22,rxPin=23)
 * U2(txPin=17,rxPin=16)
 * Conect TX2 (17) to RX1 (23)
 * Conect RX2 (16) to TX  (22)
***************************************/
void Main::write_task(void* parm)
{
    /*****************************
     * Send a message each second
    *****************************/

    // get UART object
    UART::Uart uart_obj = *(static_cast<UART::Uart*>(parm));

    // get Uart num
    int uart_num = uart_obj._uart_num;

    ESP_LOGI(LOG_TAG,"write_task running in Uart[%d]",uart_num);

    const int buf_size{100};
    char *buff = (char *) malloc(buf_size);
    int cnt{};
    char msg[20]{};

    if(uart_num == 1){
        strcpy(msg,"Hello");
    }
    if(uart_num == 2){
        strcpy(msg,"World");
    }


    for(;;)
    {
        cnt++;
        sprintf(buff, "%s #%d",msg,cnt);

        uart_obj.write(buff,strlen(buff));

        printf("Uart[%d] send    : \"%s\"\n",uart_num,buff);

       vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    free(buff);
    vTaskDelete(NULL);

}



/**************************************
 * U1(txPin=22,rxPin=23)
 * U2(txPin=17,rxPin=16)
 * Conect TX2 (17) to RX1 (23)
 * Conect RX2 (16) to TX  (22)
***************************************/
void Main::read_task(void* parm)
{
/*****************************************
 * Wait SemaphoreGive from Uart event task,
 * read RX buffer and printf data.
******************************************/

    UART::Uart uart_obj = *(static_cast<UART::Uart*>(parm));

    ESP_LOGI(LOG_TAG,"read_task running in Uart[%d]",uart_obj._uart_num);

    int len{};
    const int buf_size{100};
    uint8_t *data = (uint8_t *) malloc(buf_size);


    for(;;)
    {
        xSemaphoreTake(uart_obj.uart_data_received, portMAX_DELAY);

        len = uart_obj.read(data,buf_size,5);

        if (len >0)
        {
            printf("Uart[%d] received: \"%s\"\n",uart_obj._uart_num,data);
        }

        vTaskDelay(1); // prevent watchdog timeout
    }
    
    free(data);
    vTaskDelete(NULL);

}
#endif



