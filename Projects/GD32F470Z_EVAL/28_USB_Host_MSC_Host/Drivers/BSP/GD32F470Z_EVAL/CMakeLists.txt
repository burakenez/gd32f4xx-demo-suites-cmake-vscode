project(GD32F470Z_EVAL LANGUAGES C CXX ASM)

add_library(GD32F470Z_EVAL OBJECT
    ${DRIVERS_DIR}/BSP/GD32F470Z_EVAL/gd32f470z_eval.c
    )

target_include_directories(GD32F470Z_EVAL PUBLIC
    ${DRIVERS_DIR}/BSP/GD32F470Z_EVAL
    )

target_link_libraries(GD32F470Z_EVAL PUBLIC GD32F4xx_standard_peripheral)
