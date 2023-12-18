
idf_component_register(SRCS 
                            ./Src/main.cpp
                        
                        INCLUDE_DIRS
                            . Inc
                        
                        REQUIRES 
                            

                        )

fatfs_create_spiflash_image(storage ../dir1 FLASH_IN_PROJECT)