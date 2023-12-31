#pragma once

#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "FileStream.h"

namespace SDCARD{

class Sdcard : public FILESTREAM::FileStream
{
public:
    Sdcard(const int PIN_CS, const int PIN_MOSI, const int PIN_CLK, const int PIN_MISO);

    /// @brief mount sdcard
    /// @param BASE_PATH base path
    /// @param MAX_FILES maximum number of files
    /// @return ESP_OK on success otherwise ESP_FAIL
    esp_err_t mount(const char* BASE_PATH, const int MAX_FILES);

    /// @brief unmount sdcard
    /// @return ESP_OK on success otherwise ESP_FAIL
    esp_err_t unmount();

    /// @brief print sdcard info
    void print_info();



private:
    const char* _log_tag{"Sdcard"};
    spi_bus_config_t _spi_bus_config;
    sdmmc_host_t _host;
    sdspi_device_config_t _slot_config;
    esp_vfs_fat_sdmmc_mount_config_t _mount_config;
    sdmmc_card_t *_card;
    char _base_path[16];


};


}