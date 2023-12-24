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

    long long FileStream::write(const char* f_name,const char* content, unsigned int f_size)
    {
        errno =0;
        FILE* f = xopenfile(f_name, "wb");
        if (!f) return -1;

        unsigned int n = fwrite(content,sizeof(char),f_size,f);
        
        if(n<f_size)
        {
            if (feof(f)){
                ESP_LOGE(_log_tag,"Error writing to %s:%s", f_name,strerror(errno));
            } else {
                ESP_LOGE(_log_tag,"Error writing to %s ; bytes written less than file size", f_name);
            }
            xclosefile(f,f_name);
            return -1;
        }

        int ret = xclosefile(f,f_name);
        if (ret == -1) return -1;
        
        return n;
    }



    long long FileStream::read(const char* f_name, char** content)
    {
        const int chunk_size = 1024;
        long long total_read = {0};
        unsigned int count = {0};
        errno = 0;


        // file open in binary
        FILE* f = xopenfile(f_name, "rb");
        if (!f) {
            return -1;
        }

        // allocate initial memory
        char* buf = (char*)malloc(chunk_size*sizeof(char));
        if (buf == NULL){
            ESP_LOGE(_log_tag,"Error allocating memory\n");
            xclosefile(f,f_name);
            return -1;
        }

        
        ////////////////////////////////////
        // read long files in chunks to prevent WDT trigger
        while(!feof(f))
        {   // read a chunk
            count = fread(&buf[total_read],sizeof(char),chunk_size,f);

            // realloc memory to one chunk more
            if (count == chunk_size)
            {
                total_read += count;

                buf = (char*)realloc(buf,(total_read+chunk_size)*sizeof(char));

                if (buf==NULL)
                {
                    ESP_LOGE(_log_tag,"Error realloccating buffer\n");
                    free(buf);
                    xclosefile(f,f_name);
                    return -1;
                }
                
            }
            // return if reading error
            else if (count < chunk_size)
            {
                if(ferror(f)){
                    ESP_LOGE(_log_tag,"Error reading %s at byte %lld: %s\n",f_name,total_read,strerror(errno));
                    xclosefile(f,f_name);
                    return -1;
                }
               
            }
            vTaskDelay(1);

        }

        // expecting EOF here
        // realloc memory to necessary size
        if(feof(f)) {
            total_read += count;

            buf = (char*)realloc(buf,total_read*sizeof(char));
            if (buf==NULL)
            {
                printf("Error realloccating buffer\n");
                free(buf);
                xclosefile(f,f_name);
                return -1;
            }

        }
        // if !EOF log error
        else {
            printf("Error EOF not reached\n");
            free(buf);
            xclosefile(f,f_name);
            return -1;

        }

        xclosefile(f,f_name);
        if(!f) return -1;

        printf("bytes read = %lld\n",total_read);

        // return data in content 
        *content = buf;

        // return total read
        return total_read;

    }


    //***********************************************

    esp_err_t FileStream::writeln(FILE* f, char * line)
    {
        errno=0;

        clearerr(f);

        int result = fputs(line,f);

        if(result<0) ESP_LOGE(_log_tag, "Error writing file %s", strerror(errno));

        return (result>=0)? ESP_OK : ESP_FAIL;
    }


    //***********************************************
    
    esp_err_t FileStream::readln(FILE* f,size_t max_line_size,char* line, int* err)
    {
        errno=0;

        clearerr(f);

        line = fgets(line,max_line_size,f);

        *err = errno;

        if(errno) ESP_LOGE(_log_tag, "Error reading file : %s", strerror(errno));
        
        return ((line != NULL))? ESP_OK : ESP_FAIL;

    }

    //***********************************************

    long FileStream::size(const char* f_name)
    {
        struct stat entryStat;
        errno =0;

        // stat returns 0 on succsess
        int res = stat(f_name, &entryStat);

        if (res!=0)
        {
            ESP_LOGE(_log_tag,"FILE SIZE ERROR: %s",strerror(errno));
            return -1;
        }
            

        return entryStat.st_size;

    }



    //***********************************************

    esp_err_t FileStream::lookup(const char* dirname,const char* filename, char* buf)
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
                ret = lookup(dir_name,filename,buf);
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

    int FileStream::ls (const char* dirname)
    {
        errno = 0;
        DIR* dir;
        struct dirent *entry;

        char* full_path = (char*)calloc(300,1); ///< prevent stack overflow

        if(full_path == NULL)
        {
            ESP_LOGE(_log_tag, "Error allocating dir_name buffer");
            return -1;
        }

        if((dir = opendir(dirname)) == NULL)
        {
            ESP_LOGE(_log_tag, "Error openning dir \"%s\": \"%s\"",dirname,strerror(errno));
            free(full_path);
            return -1;
        }

        info(dirname);

        while ((entry = readdir(dir))!= NULL)
        {
            sprintf(full_path,"%s/%s",dirname,entry->d_name);
            info(full_path);
        }

        if(entry == NULL && errno != 0)
        {
            ESP_LOGE(_log_tag, "Error reading \"%s\": \"%s\"",dirname,strerror(errno));
            closedir(dir);
            free(full_path);
            return -1;
        }

        if (closedir(dir) == -1)
        {
            ESP_LOGE(_log_tag, "Error closing \"%s\": \"%s\"",dirname,strerror(errno));
            free(full_path);
            return -1;
        }

        free(full_path);

        return 0;

    }


    //***********************************************

    int FileStream::lstree (const char* dirname)
    {
        errno = 0;
        DIR* dir;
        struct dirent *entry;

        char* full_path = (char*)calloc(300,1); ///< prevent stack overflow

        if(full_path == NULL)
        {
            ESP_LOGE(_log_tag, "Error allocating dir_name buffer");
            return -1;
        }

        if((dir = opendir(dirname)) == NULL)
        {
            ESP_LOGE(_log_tag, "Error openning dir \"%s\": \"%s\"",dirname,strerror(errno));
            free(full_path);
            return -1;
            
        }

        info(dirname);

        while ((entry = readdir(dir))!= NULL)
        {
            if (entry->d_type == DT_REG)
            {
                sprintf(full_path,"%s/%s",dirname,entry->d_name);
                info(full_path);
            }
            else if (entry->d_type == DT_DIR)
            {
                continue;
            }
            
        }

        if(entry == NULL && errno != 0)
        {
            ESP_LOGE(_log_tag, "Error reading \"%s\": \"%s\"",dirname,strerror(errno));
            closedir(dir);
            free(full_path);
            return -1;
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
                sprintf(full_path,"%s/%s",dirname,entry->d_name);
                lstree(full_path);
                
            }
            
        }

        if(entry == NULL && errno != 0)
        {
            ESP_LOGE(_log_tag, "Error reading \"%s\": \"%s\"",dirname,strerror(errno));
            closedir(dir);
            free(full_path);
            return -1;
        }

        if (closedir(dir) == -1)
        {
            ESP_LOGE(_log_tag, "Error closing \"%s\": \"%s\"",dirname,strerror(errno));
            free(full_path);
            return -1;
        }

        free(full_path);

        return 0;

        }

    //**********************************************

    int FileStream::printnln(const char* f_name, int n_lines)
    {
        const size_t MAX_LINE_SIZE = 256;

        esp_err_t status = ESP_OK;
        errno=0;
        int err =0;
        FILE* f = NULL;

        // check number of lines to print
        if (n_lines <=0)
        {
            ESP_LOGE(_log_tag,"number of lines must be > 0");
            return -1;
        }

        // create buffer to receive read line
        char* line = (char*)malloc(MAX_LINE_SIZE);

        f = xopenfile(f_name,"r");
        if (f == NULL)
        {
            free(line);
            return -1;
        }

        // read lines
        for(int i=0;i<n_lines;i++)
        {
            status = readln(f,MAX_LINE_SIZE,line,&err);

            if(ESP_OK==status)
            {
                printf("[%d] %s",i+1,line);
            } else {
                if(err) ESP_LOGE(_log_tag,"Error reading line %d",i+1);
                fclose(f);
                free(line);
                return -1;
            }
        }
        
        int ret = xclosefile(f,f_name);
        if(ret==-1){
            free(line);
            return -1;
        }
        free(line);
        return 0;
    }


    //**********************************************

    int FileStream::cat(const char* f_name)
    {
        const size_t MAX_LINE_SIZE = 256;

        esp_err_t status = ESP_OK;
        errno=0;
        int err =0;
        FILE* f = NULL;

        // create buffer to receive read line
        char* line = (char*)malloc(MAX_LINE_SIZE);

        f = xopenfile(f_name,"r");
        if (f == NULL)
        {
            free(line);
            return -1;
        }

        
        // read all lines
        int i=1;
        while ((status = readln(f,MAX_LINE_SIZE,line,&err)) == ESP_OK)
        {
            printf("[%d] %s\n",i++,line);
        }
        fclose(f);
        free(line);
        if(err) {
            ESP_LOGE(_log_tag,"Error reading line %d",i+1);
            return -1;
        }
        return 0;

        
    }



//***********************************************

int FileStream::rmtree(const char *const path) {

    //printf("openning %s\n",path);

    DIR *const directory = xopendir(path);
    if (directory) {
        struct dirent *entry;
        while ((entry = readdir(directory))) {

            // ignore "." and ".."
            if (!strcmp(".", entry->d_name) || !strcmp("..", entry->d_name)) {
                printf("%s\n",entry->d_name);
                continue;
            }

            // compose entryname
            char entryname[strlen(path) + strlen(entry->d_name) + 2];
            sprintf(entryname, "%s/%s", path, entry->d_name);


            // if entry == dir then call rmtree recursevely
            if(entry->d_type == DT_DIR){
                if (rmtree(entryname)) {
                printf(">>>ERRO [%s]\n",entryname);
                closedir(directory);
                return -1;
            }
            // if entry == file then remove file
            } else {
                if (remove(entryname)) {
                printf(">>>ERRO [%s]\n",entryname);
                closedir(directory);
                return -1;
                } else {
                    printf(">>>REMOVING [%s]\n",entryname);
                }
            }
        }
        // close current dir
        int ret = xclosedir(directory,path);
        if (ret == -1) return -1;
        
    }
    // remove current dir
    printf(">>>REMOVING [%s]\n",path);
    return remove(path);
}


    
   //***********************************************

    int FileStream::readnln(const char* f_name, int n_lines)
    {
        const size_t MAX_LINE_SIZE = 256;

        esp_err_t status = ESP_OK;
        int err =0;
        FILE* f = NULL;

        // create buffer to receive read line
        char* line = (char*)malloc(MAX_LINE_SIZE);

        // open file
        f = xopenfile(f_name,"r");
        if (f == NULL)
        {
            free(line);
            return -1;
        }
        // read lines
        for(int i=0;i<n_lines;i++)
        {
            status = readln(f,MAX_LINE_SIZE,line,&err);

            if(ESP_OK==status)
            {
                printf("[%d] %s",i+1,line);
            } else {
                if(err) ESP_LOGE(_log_tag,"Error reading line %d",i+1);
                fclose(f);
                free(line);
                return -1;
            }
        }
        
        int ret = xclosefile(f,f_name);
        if(ret==-1){
            free(line);
            return -1;
        }

        free(line);
        return 0;

    }

    //***********************************************
   
    int FileStream::copynln(const char* fw_name, const char* fr_name, const int n_lines)
    {
        const int MAX_LINE_SIZE=256;

        esp_err_t status = ESP_OK;
        char* line = (char*)malloc(MAX_LINE_SIZE);
        int err=0;
        FILE* fr = NULL;
        FILE* fw = NULL;


        // open file to read
        fr = xopenfile(fr_name,"r");
        if (fr == NULL) {
            return -1;
        }
        // open file to write
        fw = xopenfile(fw_name,"a+");
        if (fw == NULL) {
            return -1;
        }


        // copy n_lines
        for (int i=0; i < n_lines; i++)
        {
            if(ESP_OK == status){
                status = readln(fr,MAX_LINE_SIZE,line,&err);
            }
            else {
                if(err) ESP_LOGE(_log_tag,"Error reading line %d",i+1);
                fclose(fr);
                fclose(fw);
                return -1;
            }


            if(ESP_OK == status){
                status = writeln(fw, line);
            }
            else {
                ESP_LOGE(_log_tag,"Error writing line %d",i+1);
                fclose(fr);
                fclose(fw);
                return -1;
            }
                
        }

        // close files
        int ret = xclosefile(fr,fr_name);
        if(ret==-1) return-1;

        ret = xclosefile(fw,fw_name);
        if(ret==-1) return-1;

        return 0;

    }

    //***********************************************
    
    int FileStream::info( const char *filename )
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
            return -1;
        }

        mode_to_letters( finfo.st_mode, modestr );

        printf( "%s"    , modestr );
        printf( "%4d "  , (int) finfo.st_nlink);	
        //printf( "%-8s " , uid_to_name(info_p->st_uid) );
        //printf( "%-8s " , gid_to_name(info_p->st_gid) );
        printf( "%8ld " , (long)finfo.st_size);
        printf( "%.12s ", 4+ctime(&finfo.st_mtime));
        printf( "%s\n"  , filename );

        return 0;

    }

    int FileStream::exists(const char* filename)
    {
        struct stat st;
        if(stat(filename, &st)==0){
            printf("... file \"%s\" exists\n",filename);
            return 0;
        }
        printf("... file \"%s\" does not exists\n",filename);
        return -1;
    }


    int FileStream::ren(const char* new_name, const char* old_name)
    {
        errno=0;

        if (exists(new_name)==0){
            ESP_LOGE(_log_tag,"Error renaming %s: File already exist",new_name);
            return -1;
        }

        if (rename(old_name,new_name)!=0)
        {
            ESP_LOGE(_log_tag,"Error renaming %s: %s",old_name,strerror(errno));
                return -1;
        }
        printf("... file renamed to \"%s\"\n",new_name);
        return 0;

    }


    int FileStream::rm(const char* filename)
    {
        errno=0;
        if (exists(filename)!=0){
            ESP_LOGE(_log_tag,"Error deleting file %s: File does not exist",filename);
            return -1;
        }

        if (remove(filename)!=0){
            ESP_LOGE(_log_tag,"Error deleting file %s: %s",filename,strerror(errno));
            return -1;
        }

        ESP_LOGI(_log_tag,"file %s deleted", filename);
        return 0;

    }


int FileStream::cptree(const char *d_path, const char *s_path, mode_t mod)
{
        struct dir { DIR *d; char *path, *end; };
        const int MAX_FILE_SIZE = 256;
        const int MAX_PATH = 512;
        int ret;

        struct dir src;
        struct dir dst;
        struct dirent *f;
        struct stat b;


        char* spath = (char*)malloc(MAX_PATH);
        if (spath == NULL){
            ESP_LOGE(_log_tag, " Error allocating spath");
            return -1;
        }

        char* dpath = (char*)malloc(MAX_PATH);
        if (dpath == NULL){
            ESP_LOGE(_log_tag, " Error allocating dpath");
            free(spath);
            return -1;
        }

        strncpy(spath,s_path,MAX_PATH);
        strncpy(dpath,d_path,MAX_PATH);

        char* fspath = (char*)malloc(MAX_FILE_SIZE);
        if(fspath==NULL){
                ESP_LOGE(_log_tag,"Error allocating fspath");
                free(spath);
                free(dpath);
                return -1;
        }

        char* fdpath = (char*)malloc(MAX_FILE_SIZE);
        if(fdpath==NULL){
                ESP_LOGE(_log_tag,"Error allocating fdpath");
                free(fspath);
                free(spath);
                free(dpath);
                return -1;
        }
        

        src.end = strchr(spath, '\0'); ///< mark end of spath
        dst.end = strchr(dpath, '\0'); ///< mark end of dpath

        //printf("spath: %s\n",spath);
        printf("mkdir: %s\n",dpath);

        if( mkdir(dpath, mod) == -1 && errno != EEXIST ){
                ESP_LOGE(_log_tag, "Error mkdir %s", dpath);
                free(fspath);
                free(fdpath);
                free(spath);
                free(dpath);
                return -1;
        }

        src.d = xopendir(src.path = spath);
        if(src.d == NULL){
            ESP_LOGE(_log_tag, "Error openning dir %s", spath);
            free(fspath);
            free(fdpath);
            free(spath);
            free(dpath);
            return -1;
        }


        dst.d = xopendir(dst.path = dpath);
        if(dst.d == NULL){
            ESP_LOGE(_log_tag, "Error openning dir %s", dpath);
            free(fspath);
            free(fdpath);
            free(spath);
            free(dpath);
            closedir(src.d);
            return -1;
        }


        //printf("readdir\n");

        while( (f = readdir(src.d)) != NULL ) {
                //printf("loop readdir %s\n", f->d_name);
                if( !strcmp(f->d_name, ".") || !strcmp(f->d_name, "..") ){
                        continue;
                }
                //getchar();
                if( f->d_type == DT_DIR ){
                        //printf("dir %s\n",f->d_name);
                        pathcat(spath, f->d_name); ///< change spath
                        pathcat(dpath, f->d_name); ///< change spath
                        xstat(spath, &b);
                        ret = cptree(spath, dpath, b.st_mode);
                        if (ret == -1){
                            free(fspath);
                            free(fdpath);
                            free(spath);
                            free(dpath);
                            closedir(src.d);
                            closedir(dst.d);
                            return -1;
                        }
                        *(src.end) = '\0'; ///< returns to original spath
                        *(dst.end) ='\0';

                }
                else if( f->d_type == DT_REG ){
                    //printf("file %s\n",f->d_name);
                    snprintf(fspath,MAX_PATH,"%s/%s",spath,f->d_name);
                    xstat(fspath, &b);
                    snprintf(fdpath,MAX_PATH,"%s/%s",dpath,f->d_name);
                    cp(fdpath,fspath);
                    xstat(fdpath, &b);
                }
        }
        //printf("end readdir loop\n");
        xclosedir(src.d, spath);
        xclosedir(dst.d, dpath);
        free(spath);
        free(dpath);
        free(fspath);
        free(fdpath);

        return 0;
} 


    int FileStream::cp(const char* fw_name,const char* fr_name) 
    { 
        FILE *fptr1, *fptr2; 
        int c; 
        errno =0;
    
        // Open one file for reading 
        fptr1 = xopenfile(fr_name,"r");
        if (fptr1 == NULL) return -1;
    
        // Open another file for writing 
        fptr2 = xopenfile(fw_name, "w");
        if(fptr2 == NULL) {
            xclosefile(fptr1,fr_name);
            return -1;
        }
    
        // Read contents from file 
        c = fgetc(fptr1); 
        while (c != EOF) 
        { 
            fputc(c, fptr2); 
            c = fgetc(fptr1); 
        }

        if (!feof(fptr1)){
            ESP_LOGE(_log_tag,"Error copying \"%s\"",fr_name);
        }
        else printf("... file \"%s\" copied to \"%s\"\n", fr_name,fw_name); 
    
        xclosefile(fptr1,fr_name); 
        xclosefile(fptr2,fw_name); 
        return 0; 
    }





    int  FileStream::makdir(const char* dirname,mode_t mode)
    {

        if( mkdir(dirname, mode) == -1 && errno != EEXIST )
        {
                ESP_LOGE(_log_tag, "Error mkdir %s", dirname);
                return -1;
        }
        return 0;

    }

    






    /****************************************
     *          PROTECTED MEMBER FUNCTIONS
     * 
    *****************************************/

    //***********************************************

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


    void
    FileStream::xrealpath(const char *s, char *resolved)
    {
            errno=0;
            if( realpath(s, resolved) == NULL ){
                    ESP_LOGE(_log_tag,"Error xrealpat %s: %s", s, strerror(errno));
            }
    }

    DIR *
    FileStream::xopendir(const char *path)
    {
            errno=0;
            DIR *d = opendir(path);
            if( d == NULL ) {
                    ESP_LOGE(_log_tag,"Error xopendir %s: %s", path,strerror(errno));
            }
            return d;
    }

    FILE *
    FileStream::xopenfile(const char *path, const char* mode)
    {
            errno=0;
            FILE *f = fopen(path, mode);
            if( f == NULL ) {
                    ESP_LOGE(_log_tag,"Error xopenfile %s: %s", path,strerror(errno));
            }
            return f;
    }

    void
    FileStream::xstat(const char *path, struct stat *buf)
    {
            errno=0;
            if( stat(path, buf) ){
                    ESP_LOGE(_log_tag,"Error xstat %s:%s", path,strerror(errno));
            }
    } 

    int
    FileStream::xclosedir(DIR *d, const char *path)
    {
            errno=0;
            if( closedir(d) ) {
                    ESP_LOGE(_log_tag,"Error xclosedir %s:%s", path,strerror(errno));
                    return -1;
                    
            }
            return 0;
    }

    int
    FileStream::xclosefile(FILE *f, const char *path)
    {
            errno=0;
            if( fclose(f) ) {
                    ESP_LOGE(_log_tag,"Error xclosefile %s:%s", path,strerror(errno));
                    return -1;
                    
            }
            return 0;
    }

    void
    FileStream::pathcat(char *p, const char *n)
    {
            const int MAX_PATH = 512;
            char q[MAX_PATH];
            strncpy(q,p,MAX_PATH);
            
            if( snprintf(p, MAX_PATH,"%s/%s", q, n) > MAX_PATH - 1 ){
                    ESP_LOGE(_log_tag,"Error pathcat %s: PATH_MAX exceeded", p);
            }
    }




}
