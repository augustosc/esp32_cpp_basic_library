#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <sys/dirent.h>
#include <errno.h>
#include "MyFatfs.h"

//#define DEBUG



namespace MYFATFS
{
    esp_err_t Fatfs::init(const char* base_path, const char* partition_label, esp_vfs_fat_mount_config_t* mount_config)
    {
        if(mount_config == NULL)
        {
            _mount_config = {
                .format_if_mount_failed = true,
                .max_files = 10,
                .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
                .disk_status_check_enable = false,
            };
        } else {
            _mount_config = *(mount_config);
        }
        
        esp_err_t status = esp_vfs_fat_spiflash_mount_rw_wl(base_path, partition_label, &_mount_config, &wl_handle);

        return status;

    }


} //namespace FATFS