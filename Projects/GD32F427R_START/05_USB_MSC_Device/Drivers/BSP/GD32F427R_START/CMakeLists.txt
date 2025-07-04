project(GD32F427R_START LANGUAGES C CXX ASM)

add_library(GD32F427R_START OBJECT
    ${DRIVERS_DIR}/BSP/GD32F427R_START/gd32f427r_start.c
    )

target_include_directories(GD32F427R_START PUBLIC
    ${DRIVERS_DIR}/BSP/GD32F427R_START
    )

target_link_libraries(GD32F427R_START PUBLIC GD32F4xx_standard_peripheral)
