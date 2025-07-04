/*!
    \file    main.c
    \brief   EXMC SDRAM demo

    \version 2024-12-20, V3.3.1, demo for GD32F4xx
*/

/*
    Copyright (c) 2024, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "gd32f450z_eval.h"
#include "exmc_sdram.h"

/* SDRAM */
#define BUFFER_SIZE                ((uint32_t)0x0400)
#define WRITE_READ_ADDR            ((uint32_t)0x0000)

uint32_t writereadstatus = 0;
uint8_t txbuffer[BUFFER_SIZE];
uint8_t rxbuffer[BUFFER_SIZE];

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint16_t i = 0;
    ErrStatus init_state;

    /* initialize LEDs */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED3);

    /* configure systick clock */
    systick_config();

    /* configure the USART */
    gd_eval_com_init(EVAL_COM0);

    /* configure the EXMC access mode */
    init_state = exmc_synchronous_dynamic_ram_init(EXMC_SDRAM_DEVICE0);

    if(ERROR == init_state) {
        printf("\r\n\r\nSDRAM initialize fail!");
        while(1);
    }

    printf("\r\nSDRAM initialized!");
    delay_ms(1000);

    /* fill txbuffer */
    fill_buffer(txbuffer, BUFFER_SIZE, 0x0000);

    delay_ms(1000);

    /* write data to SDRAM */
    sdram_writebuffer_16(EXMC_SDRAM_DEVICE0, (uint16_t *)txbuffer, WRITE_READ_ADDR, BUFFER_SIZE / 2);

    printf("\r\nSDRAM write data completed!");
    delay_ms(1000);

    /* read data from SDRAM */
    sdram_readbuffer_16(EXMC_SDRAM_DEVICE0, (uint16_t *)rxbuffer, WRITE_READ_ADDR, BUFFER_SIZE / 2);

    printf("\r\nSDRAM read data completed!");
    delay_ms(1000);

    printf("\r\nCheck the data!");

    /* compare two buffers */
    for(i = 0; i < BUFFER_SIZE; i++) {
        if(rxbuffer[i] != txbuffer[i]) {
            writereadstatus ++;
            break;
        }
    }

    if(writereadstatus) {
        printf("\r\nSDRAM test failed!");

        /* failure, light on LED3 */
        gd_eval_led_on(LED3);
    } else {
        printf("\r\nSDRAM test successed!");
        delay_ms(1000);
        printf("\r\nThe data is:\r\n");
        delay_ms(1000);
        for(i = 0; i < BUFFER_SIZE; i++) {
            printf("%6x", rxbuffer[i]);
            if(((i + 1) % 16) == 0) {
                printf("\r\n");
            }
        }
        /* success, light on LED1 */
        gd_eval_led_on(LED1);
    }

    while(1);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t) ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
