project(GD32F407V_START LANGUAGES C CXX ASM)

add_library(GD32F407V_START OBJECT
    ${DRIVERS_DIR}/BSP/GD32F407V_START/gd32f407v_start.c
    )

target_include_directories(GD32F407V_START PUBLIC
    ${DRIVERS_DIR}/BSP/GD32F407V_START
    )

target_link_libraries(GD32F407V_START PUBLIC GD32F4xx_standard_peripheral)
