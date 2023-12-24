# CPP very simple uart shell
This folder contains an ESP-IDF very simple shell written in C/C++.

## Member Functions
- console (prompt) - start shell


## Using the component
- Before build, Set "long filename in heap" in menuconfig (Fat File System Support)

- Create an instance of uart in main.h inside class Main
```bash
UART::Uart U0{0};
```

- Create an instance of fatfs or sdcard stream in main.h inside class Main
```bash
MYFATFS::Fatfs mydisk;

SDCARD::Sdcard sd {PIN_CS, PIN_MOSI, PIN_CLK, PIN_MISO};
```

- Create an instance of shell in main.h inside class Main
```bash
CONSOLE::Console sh{ &U0,&mydisk };
``````

- Initialize uart in main.cpp inside main::setup
```bash
 U0.begin(115200,1,3);
``````

- Mount Fatfs in main.cpp inside main::setup
```bash
 status = mydisk.mount(BASE_PATH,PARTITION_LABEL,MAX_FILES);
``````

- Call console() in main.cpp inside main::setup
```bash
sh.console(prompt);
``````

- Try Commands in shell
```bash
ls /store
lstree /store
rm /store/file11.txt
cptree /store/dir20/store/dir2
rmtree /store/dir2
info /store/file12.txt
cat /store/file13.txt
cp /store/file15.txt /store/file12.txt
ren /store/file15.txt /store/file12.txt
```
- Try "exit" to finish shell
```bash
exit
```

- Unount Fatfs in main.cpp inside main::setup
```bash
status = mydisk.unmount();
``````


## Example
Download an example code from Console/examples folder in github.


If you choose the option BUILDING WITH COMPONENTS FROM GITHUB in idf_component.yml file inside main, the component will be automatically downloaded from github to the manage_components local folder.

If you prefer dependencies locally download all Console component from github, choose the # LOCAL COMPONENT BUILDING  in idf_component.yml file inside main. 

So compile.

In local folder run:
```bash
idf.py build
```

And the project is build.









