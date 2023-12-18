# CPP Sdcard driver
This folder contains an ESP-IDF driver for SDcard written in C++.

## Member Functions from Sdcard
- constructor Sdcard(PIN_CS, PIN_MOSI, PIN_CLK, PIN_MISO) - creates sdcard object

- mount(BASE_PATH,MAX_FILES) - mount sdcard

- unmount(void) - unmount sdcard

- print_info(void) - print sdcard info


## Member Functions inherited from FileStream
- see readme.md in FileSream library


## Using the component
- Before build, Set "long filename in heap" in menuconfig (Fat File System Support)

- Create an instance in main.h inside class Main
```bash
SDCARD::Sdcard sd {PIN_CS, PIN_MOSI, PIN_CLK, PIN_MISO};
``````

- Mount sdcard in main.cpp inside main::setup
```bash
 status = sd.mount(BASE_PATH,MAX_FILES);
``````
- Use sdcard member functions in your code
```bash
sd.lookup(BASE_PATH,lookup_file,buf);
sd.readln(FILE* f, max_line_size,line,err);
sd.write(f_name, content,f_size);
```

- Unmount sdcard in main.cpp inside main::setup
```bash
 status = sd.unmount();
``````

## Example
Download an example code from Sdcard/examples folder in github.


If you choose the option BUILDING WITH COMPONENTS FROM GITHUB in idf_component.yml file inside main, the component will be automatically downloaded from github to the manage_components local folder.

If you prefer dependencies locally download all MySpiffs component from github, choose the # LOCAL COMPONENT BUILDING  in idf_component.yml file inside main. 

So compile.

In local folder run:
```bash
idf.py build
```

And the project is build.









