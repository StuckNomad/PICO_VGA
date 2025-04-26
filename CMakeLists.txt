cmake_minimum_required(VERSION 3.19)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)


project(USB_PIO)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


# Initialize the SDK
pico_sdk_init()


add_executable(vga_pio_exec)

pico_generate_pio_header(vga_pio_exec ${CMAKE_CURRENT_LIST_DIR}/VGA_ASM.pio)

target_sources(vga_pio_exec PRIVATE VGA.c)

target_link_libraries(vga_pio_exec PRIVATE
        pico_stdlib
        hardware_pio
        hardware_pwm
        hardware_dma
)

pico_enable_stdio_usb(vga_pio_exec 1)
pico_enable_stdio_uart(vga_pio_exec 0)

pico_add_extra_outputs(vga_pio_exec)