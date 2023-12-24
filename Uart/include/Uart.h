#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "driver/uart.h"


#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define UART_EVENT_TASK_STACK_SIZE (2048)
#define UART_EVENT_TASK_PRIORITY (5)
#define UART_EVENT_QUEUE_SIZE (30)
#define UART_PATTERN_QUEUE_SIZE (20)
#define UART_TX_BUFFER_SIZE (512)
#define UART_RX_BUFFER_SIZE (1024)

#define ESP_MODEM_LINE_BUFFER_SIZE (UART_RX_BUFFER_SIZE / 2)
#define ESP_MODEM_EVENT_QUEUE_SIZE (16)

namespace UART {
    class Uart {
    private:
        int _uart_num{};
        const char* _log_tag{"UART"};
        TaskHandle_t handle{NULL};

    private:
        //***********************************************
        /// @brief the event uart task
        void uart_event_task();

        //***********************************************
        /// @brief static wrap function to xTaskCreate
        static void taskfun(void* parm);

        //***********************************************
        /// @brief get line from console
        /// @param line_size line size
        /// @return the line read from console
        char* get_line(int line_size);

        //***********************************************
        /// @brief parse line read from console
        /// @param line line to be parsed 
        /// @return line tokens 
        char** parse_line(char* line);

    public:
        QueueHandle_t event_queue;
        SemaphoreHandle_t uart_data_received;
        uint8_t *buffer;

    public:
        Uart(int uart_num);

        ~Uart(){
            free(buffer);
        }

        //***********************************************
        /// @brief configure UART
        /// @param baud baud rate
        /// @param tx_pin tx pin
        /// @param rx_pin rx pin
        /// @param rx_buf_size rx buffer size
        /// @param tx_buf_size tx buffer size
        /// @return ESP_OK on success
        esp_err_t begin(int baud, int tx_pin, int rx_pin, int rx_buf_size = 1024, int tx_buf_size =1024);

        //***********************************************
        /// @brief write data to UART tx buffer
        /// @param data data to be written
        /// @param len data len
        /// @return number of bytes written
        int write(const void* data, size_t len);

        //***********************************************
        /// @brief read data from UART rx buffer
        /// @param data buffer to receive read data
        /// @param len buffer len
        /// @param timeout time to wait for data
        /// @return number of read bytes
        int read(uint8_t* data, uint32_t len,uint32_t timeout = 1);

        //***********************************************
        /// @brief flush rx buffer
        /// @return ESP_OK on success
        esp_err_t flush();

        //***********************************************
        /// @brief is uart data available to read
        /// @return number of bytes available
        size_t available();

        void handle_uart_data();

        void handle_uart_pattern();

        //***********************************************
        /// @brief create UART event task for each instance
        /// @param name task name
        /// @param priority task priority
        /// @param stackDepth stack size
        /// @return ESP_OK on success
        esp_err_t create_uart_event_task(const char* name, UBaseType_t priority,uint32_t stackDepth=configMINIMAL_STACK_SIZE);

        //***********************************************
        /// @brief get commands from console
        /// @param prompt the console prompt
        /// @return cmd and parameters to be run
        char** console(char* prompt);

        //***********************************************
        /// @brief print data to console
        /// @param data dta to be printed
        void print(uint8_t* data);


        //***********************************************
        /// @brief get uart_num
        /// @return uart_num
        int get_uart_num(){
            return _uart_num;
        }

    }; // class Uart


} //namespace UART
