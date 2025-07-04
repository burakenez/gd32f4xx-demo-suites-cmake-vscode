project(GD32F470I_EVAL LANGUAGES C CXX ASM)

add_library(GD32F470I_EVAL OBJECT
    ${DRIVERS_DIR}/BSP/GD32F470I_EVAL/gd32f470i_eval.c
    )

target_include_directories(GD32F470I_EVAL PUBLIC
    )

target_link_libraries(GD32F470I_EVAL PUBLIC GD32F4xx_standard_peripheral)
