#pragma once

#include "esp_log.h"
#include "esp_err.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "FileStream.h"


namespace MYFATFS
{
    class Fatfs : public FILESTREAM::FileStream
    {
private:
    const char* _log_tag {"MyFatfs"};
    wl_handle_t wl_handle;
    esp_vfs_fat_mount_config_t _mount_config;
public:
    esp_err_t init(const char* base_path, const char* partition_label, esp_vfs_fat_mount_config_t* mount_config = NULL);

    };

} // namespace FATFS