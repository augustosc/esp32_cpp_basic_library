cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

# set(EXTRA_COMPONENT_DIRS 
#         core 
#         ../../src/Gpio
#         ../../src/Sntp
#         ../../src/Wifi
#         ../../src/Credentials
#         ../../src/Smartcfg
#         ../../src/Nvs32
# )

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(ESP-IDF_Cpp)