# Main component CMakeLists.txt for ESP32 Matter Lock Bridge

set(SRC_DIRS_LIST ".")

set(INCLUDE_DIRS_LIST "."
                      "${CMAKE_CURRENT_LIST_DIR}"
                      "${CMAKE_CURRENT_LIST_DIR}/../"
                      "${CMAKE_CURRENT_LIST_DIR}/../../")

# Get the esp-matter path from environment or use default location
if(DEFINED ENV{ESP_MATTER_PATH})
    set(ESP_MATTER_PATH $ENV{ESP_MATTER_PATH})
else()
    set(ESP_MATTER_PATH "${CMAKE_CURRENT_LIST_DIR}/../../esp-matter")
endif()

# Matter dependencies
set(MATTER_SDK_PATH ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip)

set(CHIP_ROOT ${MATTER_SDK_PATH})
set(CHIP_CORE ${MATTER_SDK_PATH}/src)
set(APP_ROOT ${MATTER_SDK_PATH}/examples/lock-app/lock-common)

# Include Matter app common directories
list(APPEND INCLUDE_DIRS_LIST
     "${APP_ROOT}"
     "${CHIP_CORE}/app/util"
     "${CHIP_CORE}/app/server"
     "${CHIP_CORE}/app"
     "${CHIP_CORE}/include"
     "${CHIP_CORE}/lib/support"
     "${CHIP_CORE}/lib/core"
     "${CHIP_CORE}/app/clusters"
     "${CHIP_CORE}/app/clusters/door-lock-server"
     "${CHIP_CORE}/app/clusters/identify-server"
     "${CHIP_CORE}/app/clusters/on-off-server"
     "${CHIP_CORE}/app/clusters/descriptor"
     "${CHIP_CORE}/app/clusters/basic-information"
     "${CHIP_CORE}/app/clusters/general-commissioning-server"
     "${CHIP_CORE}/app/clusters/network-commissioning"
     "${CHIP_CORE}/app/clusters/operational-credentials-server"
     "${CHIP_CORE}/app/clusters/groups-server"
     "${CHIP_CORE}/app/clusters/scenes-server")

# Required component dependencies for Matter
set(PRIV_REQUIRES_LIST
    esp_matter
    esp_matter_console
    chip
    app_wifi
    app_driver
    app_reset
    button
    esp_secure_cert_mgr
    qrcode
    json_parser
    json_generator
    cbor
    led_strip
    ws2812_led
    nvs_flash
    spiffs
    fatfs)

# ESP-IDF component dependencies
list(APPEND PRIV_REQUIRES_LIST
     app_update
     bootloader_support
     driver
     esp_event
     esp_http_client
     esp_http_server
     esp_https_ota
     esp_netif
     esp_partition
     esp_system
     esp_timer
     esp_wifi
     freertos
     heap
     log
     lwip
     mdns
     newlib
     nvs_flash
     protocomm
     soc
     spi_flash
     vfs
     wifi_provisioning)

# Define the component
idf_component_register(SRC_DIRS ${SRC_DIRS_LIST}
                       INCLUDE_DIRS ${INCLUDE_DIRS_LIST}
                       PRIV_REQUIRES ${PRIV_REQUIRES_LIST}
                       REQUIRES esp_matter)