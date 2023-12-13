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
    unsigned int size(const char* f_name);
    
    
    //*********************************************** 
    /// @param f_name file name
    /// @param content data to be written
    /// @param f_size data size
    /// @return bytes written
    unsigned int write(const char* f_name,
                        const char* content,unsigned int f_size);


    //***********************************************  
    /// @param f_name file name
    /// @param buf buffer to save read bytes
    /// @param size_buf buffer size
    /// @return bytes read
    unsigned int read(const char* f_name, char* buf, unsigned int size_buf);


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

    esp_err_t find(const char* dirname,const char* filename, char* buf);
    
    void lstree (const char* dirname);

    void ls (const char* dirname);

    void println (const char* f_name, int n_lines);

    void rmtree(const char* path);

    void readnln(const char* f_name, int n_lines);

    void copynln(const char* fw_name, const char* fr_name, const int n_lines);

    void info( const char *filename);

protected:
    void mode_to_letters( int mode, char str[] );


};




}
        
        
        