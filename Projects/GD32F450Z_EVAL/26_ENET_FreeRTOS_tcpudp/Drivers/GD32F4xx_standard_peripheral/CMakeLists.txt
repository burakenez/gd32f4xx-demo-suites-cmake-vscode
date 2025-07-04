project(GD32F4xx_standard_peripheral LANGUAGES C CXX ASM)

# Comment-out unused source files.
add_library(GD32F4xx_standard_peripheral OBJECT
	${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_adc.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_can.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_crc.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_ctc.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_dac.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_dbg.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_dci.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_dma.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_enet.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_exmc.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_exti.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_fmc.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_fwdgt.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_gpio.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_i2c.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_ipa.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_iref.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_misc.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_pmu.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_rcu.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_rtc.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_sdio.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_spi.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_syscfg.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_timer.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_tli.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_trng.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_usart.c
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Source/gd32f4xx_wwdgt.c
    )

target_include_directories(GD32F4xx_standard_peripheral PUBLIC
    ${DRIVERS_DIR}/GD32F4xx_standard_peripheral/Include
    )

# CMSIS header only library is linked.
target_link_libraries(GD32F4xx_standard_peripheral PUBLIC CMSIS)
