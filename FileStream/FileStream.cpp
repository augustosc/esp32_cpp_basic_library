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

    unsigned int FileStream::write_new_file(const char* f_name,
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

    unsigned int FileStream::read_file(const char* f_name, char* buf, unsigned int size_buf)
    {
        //esp_err_t status{ESP_OK};
        unsigned int f_size{};
        const unsigned int chunk_size{1024};
        unsigned int total_read{};
        unsigned int count{};

        f_size = get_file_size(f_name);
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

    unsigned int FileStream::get_file_size(const char* f_name)
    {
        struct stat entryStat;

        // stat returns 0 on succsess
        int res = stat(f_name, &entryStat);

        if (res!=0)
            ESP_LOGE(_log_tag,"FILE SIZE ERROR");

        return entryStat.st_size;

    }



    //***********************************************

    esp_err_t FileStream::look_up_file(const char* dirname,const char* filename, char* buf)
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
                ret = look_up_file(dir_name,filename,buf);
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

    void FileStream::list_all_entries (const char* dirname)
    {
        ESP_LOGI(_log_tag,"calling list_all_entries: %s",dirname);

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

    ESP_LOGI(_log_tag,"dirname : %s",dirname);

    while ((entry = readdir(dir))!= NULL)
    {
        if (entry->d_type == DT_REG)
        {
            ESP_LOGI(_log_tag,"filename: %s/%s",dirname,entry->d_name);
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
            list_all_entries(dir_name);
            
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



}
