# Credentials class
To create singleton Credentials object

## Member Functions
Credentials& get_instance(void) - get singleton instance

empty_credentials(void) - check empty credentials

set_credentials (Wifi_credentials &cred) - set credentials in memory & nvs

get_credentials() - get credentials from memory

write_credentials(Wifi_credentials& cred) - write credentials on nvs

read_credentials (Wifi_credentials& cred) - read credentials from nvs

## Example
check in Nvs storage nvs_save_credentials_example
 




