
#include "esp_err.h"
#include "esp_log.h"
#include "Sdcard.h"


namespace SDCARD
{

Sdcard::Sdcard(const int PIN_CS, const int PIN_MOSI, const int PIN_CLK, const int PIN_MISO)
{
    _spi_bus_config = {
        .mosi_io_num {PIN_MOSI},
        .miso_io_num {PIN_MISO},
        .sclk_io_num {PIN_CLK},
        .quadwp_io_num {-1},
        .quadhd_io_num {-1}
    };

    _host = SDSPI_HOST_DEFAULT();

    esp_err_t ret = spi_bus_initialize((spi_host_device_t)_host.slot, &_spi_bus_config, SDSPI_DEFAULT_DMA);
    
    if(ESP_OK != ret)
    {
        ESP_LOGE(_log_tag,"Error initializing spi bus (%s)",esp_err_to_name(ret));

    } else {

    _slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    _slot_config.gpio_cs = (gpio_num_t)PIN_CS;
    _slot_config.host_id = (spi_host_device_t)_host.slot;
    }
}

esp_err_t Sdcard::mount(const char* BASE_PATH, const int MAX_FILES)
{
    strncpy(_base_path,BASE_PATH, sizeof(_base_path));

    _mount_config = {
        .format_if_mount_failed = true,
        .max_files = MAX_FILES,
        .allocation_unit_size = 16 * 1024,
        .disk_status_check_enable = false};

    esp_err_t ret = esp_vfs_fat_sdspi_mount(BASE_PATH, &_host, &_slot_config, &_mount_config, &_card);

    if(ESP_OK != ret)
    {
        ESP_LOGE(_log_tag,"Error mounting sdcard (%s)",esp_err_to_name(ret));
    }

    return ret;

}

esp_err_t Sdcard::unmount()
{
    esp_err_t ret = esp_vfs_fat_sdcard_unmount(_base_path, _card);
    if(ESP_OK != ret)
    {
        ESP_LOGE(_log_tag,"Error unmounting sdcard (%s)",esp_err_to_name(ret));
        return ret;
    }

    ret = spi_bus_free((spi_host_device_t)_host.slot);
    if(ESP_OK != ret)
    {
        ESP_LOGE(_log_tag,"Error unmounting spi bus (%s)",esp_err_to_name(ret));
        return ret;
    }
    return ret;
}

void Sdcard::print_info()
{
    sdmmc_card_print_info(stdout, _card);
}

}