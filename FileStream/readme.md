# CPP FileStream driver
This folder contains an ESP-IDF driver for FileStream written in C/C++.

## Member Functions
- info(filename) - print file info

- size(f_name) - get file size

- write(f_name,content, f_size) - write new file

- read(f_name, &buf) - read file

- writeln(f, line) - writeline to file

- readln(f, max_line_size,line, err) - read next line from a file

- printnln (f_name, n_lines) - print "n" lines of a file

- copynln(fw_name,fr_name,n_lines) - copy  "n" lines fromfr_name to fw_name 

- lookup(dirname, filename, buf) - lookup file in dir

- exists(filename) - check file exists

- ren(old_name,new_name) - rename file

- rmfile(filename) - remove file

- rmtree(path) - remove directory tree from path

- ls(dirname) - list directory

- lstree (dirname) - list directory tree

- cp(f_src,f_dst) - copy file

- cptree(spath,dpath,mode=0777) - copy directory tree




## Using the component
The component is used as parent class for others file systems components, as mySpiffs, MyFatfs or Sdcard. Se readme.md in respective library.


## Example
The examples are in specific file systems components. See specific library mySpiffs, MyFatfs or Sdcard.








