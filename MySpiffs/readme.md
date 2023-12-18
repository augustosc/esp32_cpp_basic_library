# CPP Spiffs driver
This folder contains an ESP-IDF driver for Spiffs written in C++.

## Member Functions from Spiffs
- mount(p_label, path, max_files,format) - register spiffs

- spiffs_check(p_label) - check spifss consistency 

- get_spiffs_info(void) - get total and use bytes

- unmount(void) - unregister spiffs

## Member Functions inherited from FileStream
- see readme.md in FileStream library

## Using the component
- Creating an instance in main.h inside class Main
```bash
SPIFFS::Spiffs spf;
``````

- Mount Spiffs in main.cpp inside main::setup
```bash
status = spf.mount(PARTITION_TABLE,PATH,MAX_FILES,"true");

``````
- Use Spiffs member functions in your code
```bash
spf.printnln(fw_name, n_lines);

spf.write(f_name, content,f_size);
```

- So, unmount Spiffs in main.cpp
```bash
status = spf.unmount();
```

## Example
Download an example code from MySpiffs/examples folder in github.


If you choose the option BUILDING WITH COMPONENTS FROM GITHUB in idf_component.yml file inside main, the component will be automatically downloaded from github to the manage_components local folder.

If you prefer dependencies locally download all MySpiffs component from github, choose the # LOCAL COMPONENT BUILDING  in idf_component.yml file inside main. 

So compile.

In local folder run:
```bash
idf.py build
```

And the project is build.









