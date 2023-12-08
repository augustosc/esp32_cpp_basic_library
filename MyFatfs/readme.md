# CPP FatFS driver
This folder contains an ESP-IDF driver for FatFS written in C++.

## Member Functions from MyFatfs
- init(base_path, partition_label, mount_config) - init FatFs

## Member Functions inherited from FileStream
- get_file_size(f_name) - get file size

- write_new_file(f_name,content, f_size) - write new file

- read_file(f_name, buf, size_buf) - read file

- writeln(f, line) - writeline to a file

- readln(f, max_line_size,line, err) - read a line from a file

- list_all_entries (dirname) - list dir all entries

- look_up_file(dirname, filename, buf) - lookup file in dir



## Using the component
- Creating an instance in main.h inside class Main
```bash
MYFATFS::Fatfs mydisk;
``````

- Init Fatfs in main.cpp inside main::setup
```bash
 esp_err_t err = mydisk.init(BASE_PATH,PARTITION_LABEL,NULL);

``````
- Use Fatfs member functions in your code
```bash
mydisk.look_up_file(BASE_PATH,lookup_file,buf);
mydisk.readln(FILE* f, max_line_size,cline,err);
mydisk.write_new_file(f_name, content,f_size);
```


## Example
Download an example code from MyFatfs/examples folder in github.


If you choose the option BUILDING WITH COMPONENTS FROM GITHUB in idf_component.yml file inside main, the component will be automatically downloaded from github to the manage_components local folder.

If you prefer dependencies locally download all MySpiffs component from github, choose the # LOCAL COMPONENT BUILDING  in idf_component.yml file inside main. 

So compile.

In local folder run:
```bash
idf.py build
```

And the project is build.









