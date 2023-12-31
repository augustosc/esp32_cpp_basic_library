#include "Uart.h"
#include "esp_log.h"
#include <string.h>


namespace UART
{
    //***********************************************Uart constructor

    Uart::Uart(int uart_num )
    : _uart_num {uart_num}
    {}
    

    

    //******************************************************begin
    
    esp_err_t Uart::begin(int baud, int tx_pin, int rx_pin, int rx_buf_size, int tx_buf_size)
    {
        buffer = (uint8_t*)calloc(1, UART_RX_BUFFER_SIZE);
        uart_config_t uart_config = {
		.baud_rate = baud,
		.data_bits = UART_DATA_8_BITS,
		.parity    = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_DEFAULT,
	    };
        esp_err_t status = ESP_OK;
        if (ESP_OK == status)
        {uart_param_config(_uart_num, &uart_config);}
        if (ESP_OK == status)
        {uart_set_pin(_uart_num, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);}
        if (ESP_OK == status)
        {uart_driver_install(_uart_num, rx_buf_size, tx_buf_size, 20, &event_queue, 0);}
        if (ESP_OK == status)
        {uart_flush(_uart_num);}

        return status;

    } //end begin


    //***********************************************write

    int Uart::write (const void* data, size_t len)
    {
        return uart_write_bytes(_uart_num, data, len);
       
    } // end write



    //***********************************************read

    int Uart::read (uint8_t* data, uint32_t len,uint32_t timeout)
    {   
        memset(data, 0 , len);

        return uart_read_bytes(_uart_num, data, len, timeout/portTICK_PERIOD_MS);
    } // end read



    //***********************************************flush

    esp_err_t Uart::flush()
    {
        return uart_flush(_uart_num);
    } // end flush


    //***********************************************available

    size_t Uart::available()
    {
        size_t size{};

        esp_err_t status = uart_get_buffered_data_len(_uart_num, &size);

        return (ESP_OK == status) ? size : -1;
    } // end available



    //***********************************************handle_uart_data

    void Uart::handle_uart_data()
    {
        size_t len = 0;
        uart_get_buffered_data_len(_uart_num, &len);
        //len = MIN(ESP_MODEM_LINE_BUFFER_SIZE, len);
        len = uart_read_bytes(_uart_num, buffer, len, portMAX_DELAY);
        /* pass the input data to configured callback */
        if (len) {
            //printf("%.*s",len,(char*)buffer);
            xSemaphoreGive(uart_data_received);
        }
    } // end handle_uart_data


    //***********************************************handle_uart_pattern

    void Uart::handle_uart_pattern()
    {}


    /********************************************************
     * uart_event_task:
     * Give a Semaphore after receiving data.
    *********************************************************/
    void Uart::uart_event_task()
    {
        ESP_LOGI(_log_tag,"running uart event task");
        //int len;
        const int buffer_size{1024};
        uint8_t* buffer = (uint8_t*) malloc(buffer_size);
        
        uart_event_t event;
        while (1) 
        {
            if (xQueueReceive(event_queue, &event, pdMS_TO_TICKS(100))) {
                memset(buffer,0,buffer_size);
                switch (event.type) 
                {
                case UART_DATA:
                    xSemaphoreGive(uart_data_received);
                    //handle_uart_data(); // could handle data internally
                    break;
                case UART_FIFO_OVF:
                    ESP_LOGW(_log_tag, "HW FIFO Overflow");
                    uart_flush_input(_uart_num);
                    xQueueReset(event_queue);
                    break;
                case UART_BUFFER_FULL:
                    ESP_LOGW(_log_tag, "Ring Buffer Full");
                    uart_flush_input(_uart_num);
                    xQueueReset(event_queue);
                    break;
                case UART_BREAK:
                    ESP_LOGW(_log_tag, "Rx Break");
                    break;
                case UART_PARITY_ERR:
                    ESP_LOGE(_log_tag, "Parity Error");
                    break;
                case UART_FRAME_ERR:
                    ESP_LOGE(_log_tag, "Frame Error");
                    break;
                case UART_PATTERN_DET:
                    ESP_LOGI(_log_tag, "Pattern Detect");
                    //handle_uart_pattern();
                    break;
                default:
                    ESP_LOGW(_log_tag, "unknown uart event type: %d", event.type);
                    break;
                }
            }
        }
        vTaskDelete(NULL);
    } // end uart_event_task


/*************************************************
 * create_uart_event_task:
 * Use taskfun static member function 
 * as wrap for uart_event_task.
 * The wrap is necessary because xTaskCreate demands 
 * a static function (no objects), but we need a 
 * uart_event_task for each object.
 * The object itself is passed by task parameter.
**************************************************/

esp_err_t Uart::create_uart_event_task(const char* name, UBaseType_t priority,uint32_t stackDepth)
{
    ESP_LOGI(_log_tag,"Creating uart event task");

    esp_err_t status;


    uart_data_received = xSemaphoreCreateBinary();

    /******************************************************
     * Observe xTaskCreate returns pdPASS as successful.
     * As pdPASS == !ESP_OK, we need to invert xTaskCreate 
     * return data.
    *******************************************************/
    status = !xTaskCreate(&taskfun, (const char*)name, stackDepth, this, priority, &handle);

    return status;
}

/******************************************************
 * taskfun:
 * a wrap to uart_event_task.
 * The wrap is necessary because xTaskCreate demands 
 * a static function (no objects), but we need a 
 * uart_event_task for each object.
 * The object itself is passed by task parameter.
*******************************************************/

void Uart::taskfun(void* parm) 
  {
        ESP_LOGI("UART","starting uart event task");
        
        /****************************************
         * Call uart_event_task for the instance
        *****************************************/
        static_cast<Uart *>(parm)->uart_event_task();

        //((TaskClass*)(parm))->task();

        //TaskClass* obj = static_cast<TaskClass*>(parm);
        //obj->task();

        vTaskDelete(NULL);
  }

  
    //****************************************

    void Uart::print (uint8_t* data)
    {
        int len;
        len = write(data,strlen((char*)data)+1);
    }




} // namespace UART