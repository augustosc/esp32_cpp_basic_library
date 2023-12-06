# Uart Class
This folder contains an ESP-IDF driver for UART written in C++.

You could create and use uart instances to communicate.

## Interfaces
There are two interfaces:
- pooling uart
- queue (event) uart


## Member Functions all Uart
- esp_err_t begin(int baud, int tx_pin, int rx_pin, int rx_buf_size, int tx_buf_size) - configure UART

- int write(const void* data, size_t len) - write data to UART tx buffer

- int read(uint8_t* data, uint32_t len,uint32_t timeout) - read data from UART rx buffer
- esp_err_t flush(); - flush rx buffer
- size_t available(); - is uart data available to read

## Member Functions only event Uart
- esp_err_t create_uart_event_task(const char* name, UBaseType_t priority,uint32_t stackDepth=configMINIMAL_STACK_SIZE) - create UART event task for the instance

- Att: event data is flagged by semaphore uart_data_received.





