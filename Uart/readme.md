# CPP Uart driver
This folder contains an ESP-IDF driver for UART written in C++.

You could create and use uart instances to communicate.

## Interfaces
There are two interfaces:
- pooling uart
- queue (event) uart


## Member Functions all Uart
- begin(baud, tx_pin, rx_pin, rx_buf_size, tx_buf_size) - configure UART

- write(data, len) - write data to UART tx buffer

- read(data, len, timeout) - read data from UART rx buffer

- flush(void); - flush rx buffer

- size_t available(void) - is uart data available to read

- char** console(prompt) - get commands from console

- void print(uint8_t* data) - print data to console

## Member Functions only event Uart
- create_uart_event_task(name, priority,stackDepth) - create UART event task for the instance

- Att: event data is flagged by semaphore uart_data_received.

## Using the component
- Creating an instance in main.h inside class Main
```bash
UART::Uart U0{0};
``````

- Init the instance in main.cpp inside main::setup
```bash
U0.begin(115200,1,3);
``````
- Use Gpio member functions in your code
```bash
 len = U0.read(data,buf_size,5);
 len = U0.write(data,len);
```

## Example
Download an example code from Uart/examples folder in github.

Connect pins: 
<br>RX2(16)-->TX1(22)
<br>TX2(17)-->RX1(23)

If you choose the option BUILDING WITH COMPONENTS FROM GITHUB in idf_component.yml file inside main, the component will be automatically downloaded from github to the manage_components local folder.

If you prefer dependencies locally, first download all MySpiffs component from github, so choose the # LOCAL COMPONENT BUILDING  in idf_component.yml file inside main. 

So compile.

In local folder run:
```bash
idf.py build
```

And the project is build.




