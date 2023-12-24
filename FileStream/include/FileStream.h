#pragma once

#include <stdio.h>
#include <sys/dirent.h>
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_err.h"

namespace FILESTREAM
{

class FileStream 
{
private:
    const char* _log_tag{"FileStream"};

public:
    //***********************************************
    /// @brief get file size
    /// @param f_name file name
    /// @return file size on success otherwise -1 
    long size(const char* f_name);
    
    
    //*********************************************** 
    /// @brief write data to a file sream
    /// @param f_name file name
    /// @param content data to be written
    /// @param f_size data size
    /// @return number of written bytes otherwise -1
    long long write(const char* f_name,
                        const char* content,unsigned int f_size);


    //***********************************************  
    /// @brief read data from a file sream on heap buffer
    /// @param f_name file name
    /// @param content buffer to save read bytes
    /// @return number of read bytes otherwise -1
    long long read(const char* f_name, char** content);


    ///*********************************************** 
    /// @brief write next line to a file stream
    /// @param f pointer to file stream
    /// @param line line to be written
    /// @return bytes written
    esp_err_t writeln(FILE* f, char * line);

    //***********************************************  
    /// @brief read next line from a file stream
    /// @param f pointer to file stream
    /// @param max_line_size max line size
    /// @param line buffer to save line
    /// @param err returns errno
    /// @return ESP_OK on success
    esp_err_t readln(FILE* f,size_t max_line_size,char* line,int* err);

    //***********************************************  
    /// @brief look up a file in a directory
    /// @param dirname directory name
    /// @param filename file name
    /// @param buf returns the path if the file is found
    /// @return ESP_OK on success
    esp_err_t lookup(const char* dirname,const char* filename, char* buf);


    //***********************************************
    /// @brief list all directory tree 
    /// @param dirname directory name
    int lstree (const char* dirname);

    //***********************************************
    /// @brief list directory 
    /// @param dirname directory name
    int ls (const char* dirname);


     /// @brief print n lines from a file. Open file first.
     /// @param f_name file name
     /// @param n_lines number of lines
     /// @return returns 0 on success otherwise -1
   int printnln (const char* f_name, int n_lines);


    //***********************************************  
    /// @brief read n lines from a file. Open file first.
    /// @param f_name file name
    /// @param n_lines number of lines
    /// @return returns 0 on success otherwise -1
    int readnln(const char* f_name, int n_lines);


    //***********************************************  
    /// @brief copy n lines from file1 to file2
    /// @param fr_name file to be read
    /// @param fw_name file to be written
    /// @param n_lines number of lines
    /// @return returns 0 on success otherwise -1
    int copynln(const char* fw_name, const char* fr_name, const int n_lines);


    //***********************************************
    /// @brief remove all directory tree recursevely
    /// @param path directory name
    /// @return returns 0 on success otherwise -1
    int rmtree(const char* path);


    //***********************************************
    /// @brief print file info 
    /// @param filename file name
    int info( const char *filename);


    //***********************************************
    /// @brief check file exists
    /// @param filename file name
    /// @return returns 0 if exists otherwise -1
    int exists(const char* filename);


    //***********************************************
    /// @brief rename file
    /// @param new_name new name
    /// @param old_name old name
    /// @return returns 0 if success otherwise -1
    int ren(const char* new_name, const char* old_name);

    //***********************************************
    /// @brief remove file
    /// @param filename file name
    /// @return returns 0 if success otherwise -1
    int rm(const char* filename);

    //***********************************************
    /// @brief copy file
    /// @param fw_name destination file
    /// @param fr_name source file
    /// @return returns 0 if success otherwise -1
    int cp(const char* fw_name,const char* fr_name);


    //***********************************************
    /// @brief copy directory tree
    /// @param spath source path
    /// @param dpath destination path
    /// @param mod file permissions
    int cptree(const char *dpath, const char *spath, mode_t mod = 0777);

    /// @brief print all lines from a file. Open file first.
    /// @param f_name file name
    /// @return returns 0 on success otherwise -1
    int cat(const char* fname);

    int makdir(const char* dirname, mode_t mode =0777);

    



protected:
    void mode_to_letters( int mode, char str[] );
    DIR * xopendir(const char *);
    FILE * xopenfile(const char *path, const char* mode);
    int xclosefile(FILE *f, const char *path);
    int xclosedir(DIR *, const char *);
    void xstat(const char *, struct stat *);
    void xrealpath(const char *s, char *resolved);
    void pathcat(char *p, const char *n);


};




}
        
        
        