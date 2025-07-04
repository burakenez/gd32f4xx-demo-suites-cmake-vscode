project(GD32F450Z_EVAL LANGUAGES C CXX ASM)

add_library(GD32F450Z_EVAL OBJECT
    ${DRIVERS_DIR}/BSP/GD32F450Z_EVAL/gd32f450z_eval.c
    )

target_include_directories(GD32F450Z_EVAL PUBLIC
    ${DRIVERS_DIR}/BSP/GD32F450Z_EVAL
    )

target_link_libraries(GD32F450Z_EVAL PUBLIC GD32F4xx_standard_peripheral)
