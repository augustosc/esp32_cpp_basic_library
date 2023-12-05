# Uart Class
To create Uart objects

## Member Functions
begin(int baud, int tx_pin, int rx_pin, int rx_buf_size, int tx_buf_size) - configure UART

write(const void* data, size_t len) - write data to UART tx buffer

read(uint8_t* data, uint32_t len,uint32_t timeout) - read data from UART rx buffer






