#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <sys/dirent.h>
#include <errno.h>
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include  "FileStream.h"

namespace FILESTREAM
{
    //***********************************************

    unsigned int FileStream::write(const char* f_name,
                            const char* content, unsigned int f_size)
    {
        FILE* f = fopen(f_name, "wb");
        if (!f)
        {
            ESP_LOGE(_log_tag,"FILE OPEN ERROR");
            return ESP_FAIL;
        }

        unsigned int n = fwrite(content,sizeof(char),f_size,f);

        fclose(f);
        
        return n;
    }


    //***********************************************

    unsigned int FileStream::read(const char* f_name, char* buf, unsigned int size_buf)
    {
        //esp_err_t status{ESP_OK};
        unsigned int f_size{};
        const unsigned int chunk_size{1024};
        unsigned int total_read{};
        unsigned int count{};

        f_size = size(f_name);
        //ESP_LOGI(_log_tag,"FILE SIZE = %u",f_size);
        //ESP_LOGI(_log_tag,"BUFFER SIZE = %u",size_buf);

        // check buffer size sufficiency
        if(size_buf < f_size )
        {
            ESP_LOGE(_log_tag,"INSUFFICIENT BUFFER SIZE: file size = %u bytes",f_size);
            return ESP_FAIL;
        }

        // file open in binary
        FILE* f = fopen(f_name, "rb");
        if (!f)
        {
            ESP_LOGE(_log_tag,"FILE OPEN ERROR");
            return ESP_FAIL;
        }

        ////////////////////////////////////
        // read long files in chunks to prevent WDT trigger
        if (f_size > chunk_size)
        {
            ESP_LOGI(_log_tag,"Running chunk read");
            
            while(total_read < f_size)
            {
                count = fread(&buf[total_read],1,chunk_size,f);
                total_read += count;
                //ESP_LOGI(_log_tag,"COUNT = %ld", count);
                //ESP_LOGI(_log_tag,"TOTAL_READ = %ld",total_read );
                vTaskDelay(1);
            }
        ////////////////////////////////////

        }else {
        total_read = fread(buf,1,f_size,f);
        }

        buf[f_size] = '\0';

        ESP_LOGI(_log_tag,"read bytes = %u",total_read);


        fclose(f);

        return total_read;

    }


    //***********************************************

    esp_err_t FileStream::writeln(FILE* f, char * line)
    {
        errno=0;

        clearerr(f);

        int result = fputs(line,f);

        if(result<0) ESP_LOGE(_log_tag, "Error writing file %s", strerror(errno));

        return (result>0)? ESP_OK : ESP_FAIL;
    }


    //***********************************************
    
    esp_err_t FileStream::readln(FILE* f,size_t max_line_size,char* line, int err)
    {
        errno=0;

        clearerr(f);

        line = fgets(line,max_line_size,f);

        err = errno;

        if(err) ESP_LOGE(_log_tag, "Error reading file : %s", strerror(errno));

        return ((line != NULL))? ESP_OK : ESP_FAIL;

    }

    //***********************************************

    unsigned int FileStream::size(const char* f_name)
    {
        struct stat entryStat;
        errno =0;

        // stat returns 0 on succsess
        int res = stat(f_name, &entryStat);

        if (res!=0)
            ESP_LOGE(_log_tag,"FILE SIZE ERROR: %s",strerror(errno));

        return entryStat.st_size;

    }



    //***********************************************

    esp_err_t FileStream::find(const char* dirname,const char* filename, char* buf)
    {
#ifdef DEBUG
        ESP_LOGI(_log_tag,"calling look_up_file: %s",dirname);
#endif
        

        errno = 0;
        DIR* dir = NULL;
        struct dirent *entry;
        esp_err_t ret = ESP_FAIL;

        char* dir_name = (char*)calloc(300,1); ///< prevent stack overflow

        if(dir_name == NULL)
        {
            ESP_LOGE(_log_tag, "Error allocating dir_name buffer");
            goto finish_1;
        }

        if((dir = opendir(dirname)) == NULL)
        {
            ESP_LOGE(_log_tag, "Error openning dir \"%s\": \"%s\"",dirname,strerror(errno));
            goto finish;
        }

        while ((entry = readdir(dir))!= NULL)
        {
            if (entry->d_type == DT_REG)
            {
#ifdef DEBUG
                ESP_LOGI(_log_tag,"filename: %s; d->name:  %s",filename,entry->d_name);
#endif
                /*********************************************************************
                 * FAT32 is a case-insensitive file system.
                 * We need to compare with strcasecmp().
                 * 
                 * Section 6.1 of the official Microsoft spec:
                 * "Short file names passed to the file system are "always" converted 
                 * to "upper case" and their original case value is lost."
                 * 
                 * raddir() normally returns d->name in upper case. 
                 * But sometimes in lower case??????
                **********************************************************************/
                if ((strcasecmp(entry->d_name,filename))== 0)
                {
#ifdef DEBUG
                    ESP_LOGI(_log_tag,"file match");
#endif
                    sprintf(buf,"%s/%s",dirname, filename);
                    ret = ESP_OK;
                    break;
                }
            }
            else if (entry->d_type == DT_DIR)
            {
                sprintf(dir_name,"%s/%s",dirname,entry->d_name);
                ret = find(dir_name,filename,buf);
                if (ESP_OK == ret) 
                {
                    break;
                }
                
            }
            
        }

        if(entry == NULL && errno != 0)
        {
            ESP_LOGE(_log_tag, "Error reading \"%s\": \"%s\"",dirname,strerror(errno));
        }

        if (closedir(dir) == -1)
        {
            ESP_LOGE(_log_tag, "Error closing \"%s\": \"%s\"",dirname,strerror(errno));
        }

    finish:
        free(dir_name);

    finish_1:
#ifdef DEBUG
        ESP_LOGI(_log_tag,"returning look_up_file: %s ret = %d",dirname,ret);
#endif

        return ret;
}




//***********************************************

    void FileStream::ls (const char* dirname)
    {
        //ESP_LOGI(_log_tag,"calling list_all_entries: %s",dirname);
        errno = 0;
        DIR* dir;
        struct dirent *entry;
        //esp_err_t ret = ESP_FAIL;

        char* dir_name = (char*)calloc(300,1); ///< prevent stack overflow

        if(dir_name == NULL)
        {
            ESP_LOGE(_log_tag, "Error allocating dir_name buffer");
            return;
        }

        if((dir = opendir(dirname)) == NULL)
        {
            ESP_LOGE(_log_tag, "Error openning dir \"%s\": \"%s\"",dirname,strerror(errno));
            goto finish_1;
        }

        //ESP_LOGI(_log_tag,"dirname : %s",dirname);
        info(dirname);

        while ((entry = readdir(dir))!= NULL)
        {
            sprintf(dir_name,"%s/%s",dirname,entry->d_name);
            info(dir_name);
        }

        if(entry == NULL && errno != 0)
        {
            ESP_LOGE(_log_tag, "Error reading \"%s\": \"%s\"",dirname,strerror(errno));
            goto finish;
        }

    finish:

        if (closedir(dir) == -1)
        {
            ESP_LOGE(_log_tag, "Error closing \"%s\": \"%s\"",dirname,strerror(errno));
        }

    finish_1:
        free(dir_name);

        }






    //***********************************************

    void FileStream::lstree (const char* dirname)
    {
        //ESP_LOGI(_log_tag,"calling list_all_entries: %s",dirname);
        errno = 0;
        DIR* dir;
        struct dirent *entry;
        //esp_err_t ret = ESP_FAIL;

        char* dir_name = (char*)calloc(300,1); ///< prevent stack overflow

        if(dir_name == NULL)
        {
            ESP_LOGE(_log_tag, "Error allocating dir_name buffer");
            return;
        }

        if((dir = opendir(dirname)) == NULL)
        {
            ESP_LOGE(_log_tag, "Error openning dir \"%s\": \"%s\"",dirname,strerror(errno));
            goto finish_1;
        }

        //ESP_LOGI(_log_tag,"dirname : %s",dirname);
        info(dirname);

        while ((entry = readdir(dir))!= NULL)
        {
            if (entry->d_type == DT_REG)
            {
                //ESP_LOGI(_log_tag,"filename: %s/%s",dirname,entry->d_name);
                sprintf(dir_name,"%s/%s",dirname,entry->d_name);
                info(dir_name);
            }
            else if (entry->d_type == DT_DIR)
            {
                continue;
            }
            
        }

        if(entry == NULL && errno != 0)
        {
            ESP_LOGE(_log_tag, "Error reading \"%s\": \"%s\"",dirname,strerror(errno));
            goto finish;
        }


        rewinddir(dir);

        while ((entry = readdir(dir))!= NULL)
        {
            if (entry->d_type == DT_REG)
            {
                continue;
            }
            else if (entry->d_type == DT_DIR)
            {
                sprintf(dir_name,"%s/%s",dirname,entry->d_name);
                lstree(dir_name);
                
            }
            
        }

        if(entry == NULL && errno != 0)
        {
            ESP_LOGE(_log_tag, "Error reading \"%s\": \"%s\"",dirname,strerror(errno));
        }

    finish:

        if (closedir(dir) == -1)
        {
            ESP_LOGE(_log_tag, "Error closing \"%s\": \"%s\"",dirname,strerror(errno));
        }

    finish_1:
        free(dir_name);

        }

    //**********************************************8
    void FileStream::println(const char* f_name, int n_lines)
{
    //const char* PARTITION_LABEL{"storage"};
    const size_t MAX_LINE_SIZE = 256;

    esp_err_t status = ESP_OK;
    errno=0;
    int err =0;
    FILE* f = NULL;

    // create buffer to receive read line
    char* line = (char*)malloc(MAX_LINE_SIZE);

    // open file
    ESP_LOGW(_log_tag,"Openning file %s",f_name);
    f = fopen(f_name,"r");
    if (f == NULL)
    {
        ESP_LOGE(_log_tag,"Error opening file %s: %s",f_name, strerror(errno));
        goto l_free;
    }

    ESP_LOGW(_log_tag,"Reading %d lines of file %s",n_lines,f_name);
    // read lines
    for(int i=0;i<n_lines;i++)
    {
        status = readln(f,MAX_LINE_SIZE,line,err);

        if(err)
        {
            ESP_LOGE(_log_tag,"Error reading line %d",i+1);
            break;
        }

        if(ESP_OK==status)
        {
            printf("[%d] %s",i+1,line);
        } else {
            break;
        }
    }
    
    fclose(f);

l_free:
    free(line);
}

//***********************************************

    void FileStream::rmtree(const char path[])
    {
        size_t path_len;
        char *full_path;
        DIR *dir;
        struct stat stat_path, stat_entry;
        struct dirent *entry;

        // stat for the path
        stat(path, &stat_path);

        // if path does not exists or is not dir - exit with status -1
        if (S_ISDIR(stat_path.st_mode) == 0) {
            fprintf(stderr, "%s: %s\n", "Is not directory", path);
            exit(-1);
        }

        // if not possible to read the directory for this user
        if ((dir = opendir(path)) == NULL) {
            fprintf(stderr, "%s: %s\n", "Can`t open directory", path);
            exit(-1);
        }

        // the length of the path
        path_len = strlen(path);

        // iteration through entries in the directory
        while ((entry = readdir(dir)) != NULL) {

            // skip entries "." and ".."
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
                continue;

            // determinate a full path of an entry
            full_path = (char*)calloc(path_len + 1 + strlen(entry->d_name) + 1, sizeof(char));
            strcpy(full_path, path);
            strcat(full_path, "/");
            strcat(full_path, entry->d_name);

            // stat for the entry
            stat(full_path, &stat_entry);

            // recursively remove a nested directory
            if (S_ISDIR(stat_entry.st_mode) != 0) {
                rmtree(full_path);
                free(full_path);
                continue;
            }

            // remove a file object
            if (unlink(full_path) == 0)
                printf("Removed a file:\t%s\n", full_path);
            else
                printf("Can't remove a file:\t%s\n", full_path);
            free(full_path);
        }

        // remove the devastated directory and close the object of it
        if (rmdir(path) == 0)
            printf("Removed a directory:\t%s\n", path);
        else
            printf("Can't remove a directory:\t%s\n", path);

        closedir(dir);
    }


    /*************************************************
     *         Read and print file line by line
    **************************************************/
    void FileStream::readnln(const char* f_name, int n_lines)
    {
        const size_t MAX_LINE_SIZE = 256;

        esp_err_t status = ESP_OK;
        int err =0;
        FILE* f = NULL;

        // create buffer to receive read line
        char* line = (char*)malloc(MAX_LINE_SIZE);

        // open file
        f = fopen(f_name,"r");
        if (f == NULL)
        {
            ESP_LOGE(_log_tag,"Error opening file %s",f_name);
            goto l_end;
        }
        // read lines
        for(int i=0;i<n_lines;i++)
        {
            status = readln(f,MAX_LINE_SIZE,line,err);

            if(err)
            {
                ESP_LOGE(_log_tag,"Error reading line %d",i+1);
                break;
            }

            if(ESP_OK==status)
            {
                printf("[%d] %s",i+1,line);
            } else {
                break;
            }
        }
        
        fclose(f);

    l_end:
        free(line);
        
    }

    /*************************************************
     *            Copy files line by line
    **************************************************/
    void FileStream::copynln(const char* fw_name, const char* fr_name, const int n_lines)
    {
        const int MAX_LINE_SIZE=256;

        esp_err_t status = ESP_OK;
        char* line = (char*)malloc(MAX_LINE_SIZE);
        int err=0;
        FILE* fr = NULL;
        FILE* fw = NULL;


        // open file to read
        fr = fopen(fr_name,"r");
        if (fr == NULL) {
            ESP_LOGE(_log_tag, "Error opening %s", fr_name);
            return;
        }
        // open file to write
        fw = fopen(fw_name,"a+");
        if (fw == NULL) {
            ESP_LOGE(_log_tag, "Error opening %s", fw_name);
            return;
        }

        // copy n_lines
        for (int i=0; i < n_lines; i++)
        {
            if(ESP_OK == status)
                readln(fr,MAX_LINE_SIZE,line,err);

            if(ESP_OK == status)
                writeln(fw, line);
        }

        // close files
        fclose(fr);
        fclose(fw);

    }

    void FileStream::mode_to_letters( int mode, char str[] )
    {
        strcpy( str, "----------" );           /* default=no perms */

        if ( S_ISDIR(mode) )  str[0] = 'd';    /* directory?       */
        //if ( S_ISREG(mode) )  str[0] = 'f';    /* file?       */
        if ( S_ISCHR(mode) )  str[0] = 'c';    /* char devices     */
        if ( S_ISBLK(mode) )  str[0] = 'b';    /* block device     */

        if ( mode & S_IRUSR ) str[1] = 'r';    /* 3 bits for user  */
        if ( mode & S_IWUSR ) str[2] = 'w';
        if ( mode & S_IXUSR ) str[3] = 'x';

        if ( mode & S_IRGRP ) str[4] = 'r';    /* 3 bits for group */
        if ( mode & S_IWGRP ) str[5] = 'w';
        if ( mode & S_IXGRP ) str[6] = 'x';

        if ( mode & S_IROTH ) str[7] = 'r';    /* 3 bits for other */
        if ( mode & S_IWOTH ) str[8] = 'w';
        if ( mode & S_IXOTH ) str[9] = 'x';
    }


    void FileStream::info( const char *filename )
    /*
    * display the info about 'filename'.  The info is stored in struct at *info_p
    */
    {
        struct stat finfo;
        char    modestr[11];
        errno=0;
        int ret = stat(filename,&finfo);
        if (ret == -1)
        {
            ESP_LOGE(_log_tag,"Error openning file %s: %s",filename,strerror(errno));
            return;
        }

        mode_to_letters( finfo.st_mode, modestr );

        printf( "%s"    , modestr );
        printf( "%4d "  , (int) finfo.st_nlink);	
        //printf( "%-8s " , uid_to_name(info_p->st_uid) );
        //printf( "%-8s " , gid_to_name(info_p->st_gid) );
        printf( "%8ld " , (long)finfo.st_size);
        printf( "%.12s ", 4+ctime(&finfo.st_mtime));
        printf( "%s\n"  , filename );

    }

}
