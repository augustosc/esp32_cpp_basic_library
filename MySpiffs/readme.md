# Spiffs class
To create spiffs objects

## Member Functions
mount(const char* p_label,const char* path,size_t max,bool format) - register spiffs

open_file(const char* f_name,const char* mode) - open file

close_file(FILE* f) - close file

get_file_size(const char* f_name) - get file size

write_new_file(const char* f_name,const char* content,unsigned int f_size) - write new file

read_file(const char* f_name, char* buf, unsigned int size_buf) - read file

unmount(void) - unregister spiffs









