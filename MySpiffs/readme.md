# CPP Spiffs driver
This folder contains an ESP-IDF driver for Spiffs written in C++.

## Member Functions
- mount(p_label, path, max_files,format) - register spiffs

- get_file_size(f_name) - get file size

- write_new_file(f_name,content, f_size) - write new file

- read_file(f_name, buf, size_buf) - read file

- writeln(f, line) - writeline to a file

- readln(f, max_line_size,line, err) - read a line from a file


- unmount(void) - unregister spiffs


## Using the component
- Creating an instance in main.h inside class Main
```bash
SPIFFS::Spiffs spf;
``````

- Mount Spiffs in main.cpp inside main::setup
```bash
esp_err_t status = spf.mount(PARTITION_TABLE,PATH,MAX_FILES,"true");

``````
- Use Spiffs member functions in your code
```bash
readln(FILE* f, max_line_size,cline,err);

write_new_file(f_name, content,f_size);
```

- So, unmount Spiffs in main.cpp
```bash
esp_err_t status = spf.unmount();
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









