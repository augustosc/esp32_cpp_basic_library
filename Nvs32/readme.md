# Nvs Class
To create singleton Nvs object

## Member Functions
init(const char* nvs_namespace) - init nvs

get_instance(void) - get nvs singleton instance

write(const char* const key, const T* input, const size_t n_itens) - write blob on nvs

read(const char* const key, T* output, size_t& n_itens) - read blob from nvs





