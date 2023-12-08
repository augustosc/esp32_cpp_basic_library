

idf_component_register( SRCS
                        "MyFatfs.cpp"

                        INCLUDE_DIRS 
                        "include"
                            
                        REQUIRES 
                        "fatfs"
                        "vfs")