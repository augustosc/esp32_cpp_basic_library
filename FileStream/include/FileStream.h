#pragma once

#include "esp_err.h"

namespace FILESTREAM
{

class FileStream 
{
private:
    const char* _log_tag{"FileStream"};
public:
    //***********************************************
    /// @param f_name file name
    /// @return file size
    unsigned int get_file_size(const char* f_name);
    
    
    //*********************************************** 
    /// @param f_name file name
    /// @param content data to be written
    /// @param f_size data size
    /// @return bytes written
    unsigned int write_new_file(const char* f_name,
                        const char* content,unsigned int f_size);


    //***********************************************  
    /// @param f_name file name
    /// @param buf buffer to save read bytes
    /// @param size_buf buffer size
    /// @return bytes read
    unsigned int read_file(const char* f_name, char* buf, unsigned int size_buf);


    ///*********************************************** 
    /// @brief writeln to a file stream
    /// @param f pointer to file stream
    /// @param line line to be written
    /// @return bytes written
    esp_err_t writeln(FILE* f, char * line);

    //***********************************************  
    /// @param f pointer to file stream
    /// @param max_line_size max line size
    /// @param line buffer to save read line
    /// @param err returns errno
    /// @return ESP_OK on success
    esp_err_t readln(FILE* f,size_t max_line_size,char* line,int err);

    esp_err_t look_up_file(const char* dirname,const char* filename, char* buf);
    
    void list_all_entries (const char* dirname);



};




}
        
        
        