cmake_minimum_required(VERSION 3.13)

if(NOT PICO_SDK_PATH)
  set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/pico-sdk)
endif()
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(picofs_project C CXX ASM)

set(PICO_BOARD pico_w)
set(FAMILY rp2040)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(picofs
  main.c
  flash.c
  msc_disk.c
  usb_descriptors.c
  fatfs_flash_driver.c
  ${PICO_SDK_PATH}/lib/tinyusb/lib/fatfs/source/ff.c
  ${PICO_SDK_PATH}/lib/tinyusb/lib/fatfs/source/ffsystem.c
  ${PICO_SDK_PATH}/lib/tinyusb/lib/fatfs/source/ffunicode.c
)

target_include_directories(picofs
  PRIVATE
  ${PICO_SDK_PATH}/lib/tinyusb/lib/fatfs/source
  ${CMAKE_CURRENT_LIST_DIR}
)
target_link_libraries(picofs
  pico_stdlib
  tinyusb_additions
  tinyusb_board
  tinyusb_device
  hardware_flash
  hardware_sync
)

pico_enable_stdio_usb(picofs 1)
pico_add_extra_outputs(picofs)
