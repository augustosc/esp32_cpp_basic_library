# CPP FileStream handler
This folder contains an ESP-IDF handler for FileStream written in C++.

## Member Functions

- get_file_size(f_name) - get file size

- write_new_file(f_name,content, f_size) - write new file

- read_file(f_name, buf, size_buf) - read file

- writeln(f, line) - writeline to a file

- readln(f, max_line_size,line, err) - read a line from a file

- list_all_entries (dirname) - list dir all entries

- look_up_file(dirname, filename, buf) - lookup file in dir

- void print_file (f_name, n_lines) - print "n" lines of a file


## Using the component
The component is used as parent class for specifics file systems components, as mySpiffs and MyFatfs


## Example
The examples are in specific file systems components








