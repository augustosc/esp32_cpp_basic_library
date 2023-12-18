# CPP FatFS driver
This folder contains an ESP-IDF driver for FatFS written in C++.

## Member Functions from MyFatfs
- mount(base_path, partition_label, MAX_FILES,mount_config = NULL) - mount fatfs

- unmount() - unmount fatfs

## Member Functions inherited from FileStream
- see readme.md in FileStream


## Using the component
- Before build, Set "long filename in heap" in menuconfig (Fat File System Support)

- Create an instance in main.h inside class Main
```bash
MYFATFS::Fatfs mydisk;
``````

- Mount Fatfs in main.cpp inside main::setup
```bash
 status = mydisk.mount(BASE_PATH,PARTITION_LABEL,MAX_FILES);

``````
- Use Fatfs member functions in your code
```bash
mydisk.lookup(BASE_PATH,lookup_file,buf);
mydisk.readln(FILE* f, max_line_size,cline,err);
mydisk.write(f_name, content,f_size);
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









