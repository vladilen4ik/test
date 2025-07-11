# ESP32 Matter Lock Bridge CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

# Set the project name
set(PROJECT_NAME "esp32_matter_lock_bridge")

# Include the ESP-IDF CMake project framework
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Set the supported targets
set(SUPPORTED_TARGETS esp32 esp32s2 esp32s3 esp32c2 esp32c3 esp32c6 esp32h2)

# Define the project
project(${PROJECT_NAME})