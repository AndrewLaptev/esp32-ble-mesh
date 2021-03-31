cmake_minimum_required(VERSION 3.16.0)
list(APPEND EXTRA_COMPONENT_DIRS "/home/eternal/.platformio/packages/framework-espidf/examples/bluetooth/esp_ble_mesh/common_components/example_init")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32-ble-mesh)