# CPP Gpio driver
This folder contains an ESP-IDF driver for GPIO written in C++.

To create and use Led & Buttons instances.

The Button inteface includes debounce.

## Interfaces
There are two interfaces:
- Led
- Button

## Led Member Functions
. on(void) - turn led on

. off(void) - turn led off

. toggle(void) - toggle led

. blink(interval) - blink led on interval

## Button Member Functions
. read_state() - button state

. isChange() - is button state  changed?

. isRisingEdge() - is button 
rising edge?

. isFallingEdge() - is button falling edge?


## Using the component
- Creating an instance in main.h inside class Main
```bash
LED::Led led_2 {GPIO_NUM_2, false};

BUTTON::Button butt_0 {GPIO_NUM_0, Gpio::gpio_pull_e::UP,true};
``````

- Init the instance in main.cpp inside main::setup
```bash
esp_err_t err = led_2.init();
esp_err_t err = butt_4.init();
``````
- Use Gpio member functions in your code
```bash
led_2.on();
led_2.off();
if (butt_0.isChange()){
    ....
}
```

## Example
Download an example code from Gpio/examples folder in github.


If you choose the option BUILDING WITH COMPONENTS FROM GITHUB in idf_component.yml file inside main, the component will be automatically downloaded from github to the manage_components local folder.

If you prefer dependencies locally, first download all MySpiffs component from github, so choose the # LOCAL COMPONENT BUILDING  in idf_component.yml file inside main. 

So compile.

In local folder run:
```bash
idf.py build
```

And the project is build.







