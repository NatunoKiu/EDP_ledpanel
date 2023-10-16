cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_PATH /home/pc/pico-sdk)

include(pico_sdk_import.cmake)
project(EDP_ledpanel)
pico_sdk_init()


add_executable(EDP_ledpanel
	EDP_ledpanel.c
)


target_link_libraries(EDP_ledpanel pico_stdlib hardware_adc hardware_i2c)

pico_enable_stdio_usb(EDP_ledpanel 1)
pico_enable_stdio_uart(EDP_ledpanel 0)
pico_add_extra_outputs(EDP_ledpanel)