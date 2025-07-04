/*!
    \file    main.c
    \brief   EXMC NandFlash demo

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
#include "gd32f470i_eval.h"
#include "exmc_nandflash.h"

#define BUFFER_SIZE                 (0x100U)
#define NAND_GD_MAKERID             (0xC8U)
#define NAND_HY_MAKERID             (0xADU)
#define NAND_DEVICEID               (0xF1U)

nand_id_struct nand_id;
uint8_t txbuffer[BUFFER_SIZE], rxbuffer[BUFFER_SIZE];
__IO uint32_t writereadstatus = 0, status = 0;
uint32_t writereadaddr;
uint16_t zone, block, page, pageoffset;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint32_t j = 0, k = 0;

    /* LED initialize */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED3);

    /* configure systick clock */
    systick_config();

    /* configure the USART */
    gd_eval_com_init(EVAL_COM0);

    /* configure the EXMC access mode */
    exmc_nandflash_init(EXMC_BANK1_NAND);

    printf("\r\nNAND flash initialized!");
    delay_ms(1000);

    /* read NAND ID */
    nand_read_id(&nand_id);

    printf("\r\nRead NAND ID!");
    delay_ms(1000);
    /* print NAND ID */
    printf("\r\nNand flash ID:0x%X 0x%X 0x%X 0x%X\r\n", nand_id.maker_id, nand_id.device_id,
           nand_id.third_id, nand_id.fourth_id);
    delay_ms(1000);

    if(((NAND_GD_MAKERID == nand_id.maker_id) || (NAND_HY_MAKERID == nand_id.maker_id)) && (NAND_DEVICEID == nand_id.device_id)) {
        /* set the read and write the address */
        zone = 0;
        block = 10;
        page = 0;
        pageoffset = 1200;
        writereadaddr = ((zone * NAND_ZONE_SIZE + block) * NAND_BLOCK_SIZE + page)
                        * NAND_PAGE_SIZE + pageoffset;

        /* whether address cross-border */
        if((writereadaddr + BUFFER_SIZE) > NAND_MAX_ADDRESS) {
            printf("\r\nAddress cross-border!");

            /* failure, light on LED3 */
            gd_eval_led_on(LED3);
            while(1);
        }

        /* fill writebuffer with 0x00.. */
        fill_buffer_nand(txbuffer, BUFFER_SIZE, 0x00);

        /* write data to nand flash */
        status = nand_write(writereadaddr, txbuffer, BUFFER_SIZE);
        if(NAND_OK == status) {
            printf("\r\nWrite data successfully!");
            delay_ms(1000);
        } else {
            printf("\r\nWrite data failure!");

            /* failure, light on LED3 */
            gd_eval_led_on(LED3);
            while(1);
        }

        /* read data from nand flash */
        status = nand_read(writereadaddr, rxbuffer, BUFFER_SIZE);
        if(NAND_OK == status) {
            printf("\r\nRead data successfully!");
            delay_ms(1000);
        } else {
            printf("\r\nRead data failure!");

            /* failure, light on LED3 */
            gd_eval_led_on(LED3);
            while(1);
        }

        printf("\r\nCheck the data!");
        delay_ms(1000);
        /* read and write data comparison for equality */
        writereadstatus = 0;
        for(j = 0; j < BUFFER_SIZE; j++) {
            if(txbuffer[j] != rxbuffer[j]) {
                writereadstatus++;
                break;
            }
        }

        if(writereadstatus == 0) {
            printf("\r\nAccess NAND flash successfully!");
            delay_ms(1000);

            gd_eval_led_on(LED1);
        } else {
            printf("\r\nAccess NAND flash failure!");

            /* failure, light on LED3 */
            gd_eval_led_on(LED3);
            while(1);
        }
        printf("\r\nThe data to be read:\r\n");
        delay_ms(1000);
        for(k = 0; k < BUFFER_SIZE; k ++) {
            printf("%5x ", rxbuffer[k]);
            if(((k + 1) % 16) == 0) {
                printf("\r\n");
            }
        }
    } else {
        printf("\r\nRead NAND ID failure!");

        /* failure, light on LED3 */
        gd_eval_led_on(LED3);
        while(1);
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
