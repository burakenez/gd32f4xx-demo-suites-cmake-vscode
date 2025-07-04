project(GD32F4xx_usb_library LANGUAGES C CXX ASM)

add_library(GD32F4xx_usb_library OBJECT
    ${DRIVERS_DIR}/GD32F4xx_usb_library/driver/Source/drv_usb_core.c
    ${DRIVERS_DIR}/GD32F4xx_usb_library/driver/Source/drv_usb_host.c
    ${DRIVERS_DIR}/GD32F4xx_usb_library/driver/Source/drv_usbh_int.c
    ${DRIVERS_DIR}/GD32F4xx_usb_library/host/class/msc/Source/usbh_msc_bbb.c
    ${DRIVERS_DIR}/GD32F4xx_usb_library/host/class/msc/Source/usbh_msc_core.c
    ${DRIVERS_DIR}/GD32F4xx_usb_library/host/class/msc/Source/usbh_msc_fatfs.c
    ${DRIVERS_DIR}/GD32F4xx_usb_library/host/class/msc/Source/usbh_msc_scsi.c
    ${DRIVERS_DIR}/GD32F4xx_usb_library/host/core/Source/usbh_core.c
    ${DRIVERS_DIR}/GD32F4xx_usb_library/host/core/Source/usbh_enum.c
    ${DRIVERS_DIR}/GD32F4xx_usb_library/host/core/Source/usbh_pipe.c
    ${DRIVERS_DIR}/GD32F4xx_usb_library/host/core/Source/usbh_transc.c
    )

target_include_directories(GD32F4xx_usb_library PUBLIC
    ${DRIVERS_DIR}/GD32F4xx_usb_library/driver/Include
    ${DRIVERS_DIR}/GD32F4xx_usb_library/host/class/msc/Include
    ${DRIVERS_DIR}/GD32F4xx_usb_library/host/core/Include
    ${DRIVERS_DIR}/GD32F4xx_usb_library/ustd/class/msc
    ${DRIVERS_DIR}/GD32F4xx_usb_library/ustd/common
    )

target_link_libraries(GD32F4xx_usb_library PUBLIC GD32F427V_START)
target_link_libraries(GD32F4xx_usb_library PUBLIC FatFs)
