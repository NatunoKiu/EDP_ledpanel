cmake_minimum_required(VERSION 3.13)
# FILL YOUR "pico-sdk" PATH
set(PICO_SDK_PATH  /home/pc/pico/pico-sdk)

include(pico_sdk_import.cmake)
project(EDP_ledpanel C CXX ASM)
set(CMAKE_C_STNDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(EDP_ledpanel
	EDP_ledpanel.c
)

pico_enable_stdio_usb(EDP_ledpanel 1)
pico_enable_stdio_uart(EDP_ledpanel 0)
pico_add_extra_outputs(EDP_ledpanel)

target_link_libraries(EDP_ledpanel pico_stdlib hardware_adc hardware_i2c hardware_pio)

