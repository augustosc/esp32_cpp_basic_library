#pragma once

/***************************************************
 * Use Nvs32 class:
 * 1. init Nvs: status = nvs.init(nvs_namespace);
 * 2. write Nvs: status = nvs.write(key,&data,size);
 *   a. write automatically verify the writen data.
 * 3. read Nvs: status = nvs.read(key,&data,n_itens);
***************************************************/

#include <cstring>

#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"



namespace NVS
{

class Nvs
{
    const char* const   _log_tag{"NVS"};          ///< cstring of logging tag
    nvs_handle_t        handle{};                   ///< API NVS handle
  

public:

    constexpr Nvs()
    {}

    //***********************************************
    /// @brief init nvs
    /// @param nvs_namespace namespace
    /// @return ESP_OK on success
    [[nodiscard]] esp_err_t init(const char* nvs_namespace)
        { return _open(nvs_namespace, handle); } // init

    //***********************************************
    /// @brief close namespace on nvs
    void close(void)
        {
            ESP_LOGI(_log_tag, "%s:%d Calling nvs_close", __func__, __LINE__);

            nvs_close(handle);

            ESP_LOGI(_log_tag, "%s:%d nvs_close end", __func__, __LINE__);
            
        } // close


    //***********************************************
    /// @brief write blob on nvs
    /// @param key key
    /// @param input pointer to data to be written
    /// @param n_itens number of itens of T to be written
    /// @return ESP_OK on success
    template <typename T>
    [[nodiscard]] esp_err_t write(const char* const key, const T* input, const size_t n_itens)
        {
            esp_err_t status{ESP_OK};

            if (ESP_OK == status)
            {
                status = _set_buf(handle, key, input, n_itens); 
            }

            return status;
        } // write


    //***********************************************
    /// @brief read blob from nvs
    /// @param key key
    /// @param output pointer to buffer that receives data
    /// @param n_itens number of itens of T to be read
    /// @return ESP_OK on success
        template <typename T>
    [[nodiscard]] esp_err_t read(const char* const key, T* output, size_t& n_itens)  //// ja esta com T* output
        { 
            esp_err_t status = ESP_OK;

            if (ESP_OK == status)
            {
                status = _get_buf(handle, key, output, n_itens);
            }

            return status; 
            
        } // read


#ifdef NOT_USED
    template <typename T>
    [[nodiscard]] esp_err_t get(const char* key, T* output)  //// melhor T* output?
        { 
            size_t len{1}; return _get_buf(handle, key, output, len); }

    template <typename T>
    [[nodiscard]] esp_err_t set(const char* const key, const T& input) //// melhor T* input?
        { return _set_buf(handle, key, &input, 1); }


    template <typename T>
    [[nodiscard]] esp_err_t verify(const char* const key, const T& input) const //// melhor T* input?
        { return _verify_buf(handle, key, &input, 1); }

    template <typename T>
    [[nodiscard]] esp_err_t get_buffer(const char* const key, T* output, size_t& len)  //// ja esta com T* output
        { return _get_buf(handle, key, output, len); }


    template <typename T>
    [[nodiscard]] esp_err_t set_buffer(const char* const key, const T* input, const size_t len)
        { return _set_buf(handle, key, input, len); }

    template <typename T>
    [[nodiscard]] esp_err_t verify_buffer(const char* const key, const T* input, const size_t len) const
        { return _verify_buf(handle, key, input, len); }
#endif


    //***********************************************
    /// @brief get nvs singleton instance
    /// @return instance
    static Nvs& get_instance(void)
    {
        static Nvs nvs;
        return nvs;
    }

private:

    //***********************************************
    /// @brief open namespace on nvs
    /// @param nvs_namespace namespace to be opened
    /// @param handle handle to namespace
    /// @return ESP_OK on success
    [[nodiscard]]  esp_err_t _open(const char* const nvs_namespace, nvs_handle_t& handle)
    { 
        ESP_LOGI(_log_tag, "%s:%d Calling nvs_open", __func__, __LINE__);

        esp_err_t status = nvs_open(nvs_namespace, NVS_READWRITE, &handle);

        ESP_LOGI(_log_tag, "%s:%d nvs_open:%s", __func__, __LINE__,esp_err_to_name(status));

        return status;
    }


    //***********************************************
    /// @brief get data from nvs to buffer output
    /// @param handle nvs handle created in init
    /// @param key key
    /// @param output buffer to receive data
    /// @param len buffer len
    /// @return ESP_OK on success
    template <typename T>
    [[nodiscard]]   esp_err_t _get_buf(nvs_handle_t handle, const char* key,T* output, size_t& len)
    {
        size_t n_bytes{sizeof(T) * len}; 

        if (nullptr == key || 0 == strlen(key) || nullptr == output || 0 == len)
            return ESP_ERR_INVALID_ARG;
        else
        {
            ESP_LOGI(_log_tag, "%s:%d Calling nvs_get_blob", __func__, __LINE__);

            const esp_err_t status = nvs_get_blob(handle, key, output, &n_bytes);

            ESP_LOGI(_log_tag, "%s:%d nvs_get_blob:%s", __func__, __LINE__,esp_err_to_name(status));

            len = n_bytes / sizeof(T);

            return status;
        }
    }

    //***********************************************
    /// @brief set data to nvs from input buffer
    /// @param handle nvs handle created in init
    /// @param key key 
    /// @param input data to be written
    /// @param len buffe size
    /// @return ESP_OK on success
    template <typename T>
    [[nodiscard]]  esp_err_t _set_buf(nvs_handle_t handle, const char* const key, 
                                            const T* input, const size_t len)
    {
        esp_err_t status{ESP_OK};

        if (nullptr == key || 0 == strlen(key) || nullptr == input || 0 == len)
            status = ESP_ERR_INVALID_ARG;
        else
        {
            ESP_LOGI(_log_tag, "%s:%d Calling nvs_set_blob", __func__, __LINE__);

            status = nvs_set_blob(handle, key, input, sizeof(T) * len);

            ESP_LOGI(_log_tag, "%s:%d nvs_set_blob:%s", __func__, __LINE__,esp_err_to_name(status));

            if (ESP_OK == status)
            {
                ESP_LOGI(_log_tag, "%s:%d Calling nvs_commit", __func__, __LINE__);

                status = nvs_commit(handle);

                ESP_LOGI(_log_tag, "%s:%d nvs_commit:%s", __func__, __LINE__,esp_err_to_name(status));

            }

            if (ESP_OK == status)
            {
                status = _verify_buf(handle, key, input, len);
            }
                
        }

        return status;
    }

    //***********************************************
    /// @brief verify data written on nvs
    /// @param handle nvs handle created in init
    /// @param key key
    /// @param input data to be verified
    /// @param len data len
    /// @return ESP_OK on success
    template <typename T>
    [[nodiscard]]  esp_err_t _verify_buf(nvs_handle_t handle, const char* const key, 
                                                const T* input, const size_t len)
    {
        esp_err_t status{ESP_OK};

        T*      buf_in_nvs{new T[len]{}};
        size_t  n_items_in_nvs{len};

        if (buf_in_nvs)
        {
            status = _get_buf(handle, key, buf_in_nvs, n_items_in_nvs);

            if (ESP_OK == status)
            {
                if (len == n_items_in_nvs)
                {
                    if (0 != memcmp(input, buf_in_nvs, len * sizeof(T)))
                        status = ESP_FAIL;
                }
                else
                    status = ESP_ERR_NVS_INVALID_LENGTH;
            }

            delete[] buf_in_nvs;
        }
        else
        {
            status = ESP_ERR_NO_MEM;
        }

        return status;
    }

    
    

};

} // namespace NVS