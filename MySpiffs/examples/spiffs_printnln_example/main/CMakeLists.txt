
idf_component_register(SRCS 
                            ./Src/main.cpp
                        
                        INCLUDE_DIRS
                            . Inc
                        
                         REQUIRES 

                        )
                        
spiffs_create_partition_image(storage ../spiffs_dir FLASH_IN_PROJECT)