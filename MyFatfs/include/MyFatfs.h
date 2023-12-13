#pragma once

#include "esp_log.h"
#include "esp_err.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "FileStream.h"


namespace MYFATFS
{
    /**************************************
     * IMPORTANT:
     * Before build:
     * Set "long filename in heap" in 
     * menuconfig Fat File System Support
    ***************************************/
    class Fatfs : public FILESTREAM::FileStream
    {
private:
    const char* _log_tag {"MyFatfs"};
    wl_handle_t wl_handle;
    esp_vfs_fat_mount_config_t _mount_config;
    char _base_path[16]{0};
public:
    esp_err_t mount(const char* base_path, const char* partition_label,int MAX_FILES, esp_vfs_fat_mount_config_t* mount_config = NULL);

    esp_err_t unmount();
    

    };

} // namespace FATFS