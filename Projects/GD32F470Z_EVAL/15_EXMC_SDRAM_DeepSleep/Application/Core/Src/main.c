/*!
    \file    main.c
    \brief   EXMC SDRAM Deepsleep demo

    \version 2024-12-20, V3.3.1, demo for GD32F4xx
*/

/*
    Copyright (c) 2024, GigaDevice Semiconductor Inc

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
#include "gd32f470z_eval.h"
#include "exmc_sdram.h"

/* SDRAM */
#define BUFFER_SIZE                ((uint32_t)0x0400)
#define WRITE_READ_ADDR            ((uint32_t)0x0000)

uint32_t writereadstatus = 0;
uint8_t txbuffer[BUFFER_SIZE];
uint8_t rxbuffer[BUFFER_SIZE];

void system_clock_configuration(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    exmc_sdram_command_parameter_struct     sdram_command_init_struct;
    uint16_t i = 0;
    ErrStatus init_state;

    /* initialize LEDs */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);

    /* enable PMU clock */
    rcu_periph_clock_enable(RCU_PMU);

    /* initialize key */
    gd_eval_key_init(KEY_USER, KEY_MODE_EXTI);

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

    printf("\r\n\r\nSDRAM initialized!");
    delay_ms(1000);

    /* fill txbuffer */
    fill_buffer(txbuffer, BUFFER_SIZE, 0x0000);

    /* write data to SDRAM */
    sdram_writebuffer_16(EXMC_SDRAM_DEVICE0, (uint16_t *)txbuffer, WRITE_READ_ADDR, BUFFER_SIZE / 2);

    printf("\r\nSDRAM write data completed!");
    delay_ms(1000);

    /* enter self-refresh mode */
    sdram_command_init_struct.command = EXMC_SDRAM_SELF_REFRESH;
    sdram_command_init_struct.bank_select = EXMC_SDRAM_DEVICE0_SELECT;
    sdram_command_init_struct.auto_refresh_number = EXMC_SDRAM_AUTO_REFLESH_1_SDCLK;
    sdram_command_init_struct.mode_register_content = 0;

    while(exmc_flag_get(EXMC_SDRAM_DEVICE0, EXMC_SDRAM_FLAG_NREADY) == SET);
    /* send the command */
    exmc_sdram_command_config(&sdram_command_init_struct);
    /* wait until the SDRAM controller is ready */
    while(exmc_flag_get(EXMC_SDRAM_DEVICE0, EXMC_SDRAM_FLAG_NREADY) == SET);
    /* check SDRAM self-refresh status */
    while(exmc_sdram_bankstatus_get(EXMC_SDRAM_DEVICE0) != EXMC_SDRAM_DEVICE_SELF_REFRESH);

    printf("\r\nEnter deepsleep mode!");
    delay_ms(1000);
    printf("\r\nPress the user key to wakeup the MCU!\r\n");
    delay_ms(1000);

    /* enter deepsleep mode, light on LED2 */
    gd_eval_led_on(LED2);
    pmu_to_deepsleepmode(PMU_LDO_NORMAL, PMU_LOWDRIVER_DISABLE, WFI_CMD);
    gd_eval_led_off(LED2);

    /* reinitialize the system clock */
    system_clock_configuration();

    printf("\r\nUser key has been pressed!");
    delay_ms(1000);

    /* return normal mode */
    sdram_command_init_struct.command = EXMC_SDRAM_NORMAL_OPERATION;
    sdram_command_init_struct.bank_select = EXMC_SDRAM_DEVICE0;
    sdram_command_init_struct.auto_refresh_number = EXMC_SDRAM_AUTO_REFLESH_2_SDCLK;
    sdram_command_init_struct.mode_register_content = 0;
    /* wait until the SDRAM controller is ready */
    while(exmc_flag_get(EXMC_SDRAM_DEVICE0, EXMC_SDRAM_FLAG_NREADY) != RESET)
        /* send the command */
    {
        exmc_sdram_command_config(&sdram_command_init_struct);
    }
    /* wait until the SDRAM controller is ready */
    while(exmc_flag_get(EXMC_SDRAM_DEVICE0, EXMC_SDRAM_FLAG_NREADY) != RESET)
        /* check SDRAM self-refresh status */
        while(exmc_sdram_bankstatus_get(EXMC_SDRAM_DEVICE0) != EXMC_SDRAM_DEVICE_NORMAL);

    printf("\r\nSDRAM read data completed!");
    delay_ms(1000);

    /* read data from SDRAM */
    sdram_readbuffer_16(EXMC_SDRAM_DEVICE0, (uint16_t *)rxbuffer, WRITE_READ_ADDR, BUFFER_SIZE / 2);

    printf("\r\nCheck the data!");
    delay_ms(1000);

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

/*!
    \brief      reconfigure the system clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void system_clock_configuration(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;

    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do {
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_HXTALSTB);
    } while((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if(0U == (RCU_CTL & RCU_CTL_HXTALSTB)) {
        while(1) {
        }
    }

    RCU_APB1EN |= RCU_APB1EN_PMUEN;
    PMU_CTL |= PMU_CTL_LDOVS;

    /* HXTAL is stable */
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB/2 */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV2;
    /* APB1 = AHB/4 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV4;

    /* Configure the main PLL, PSC = 25, PLL_N = 480, PLL_P = 2, PLL_Q = 10 */
    RCU_PLL = (25U | (480U << 6U) | (((2U >> 1U) - 1U) << 16U) |
               (RCU_PLLSRC_HXTAL) | (10U << 24U));
    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* wait until PLL is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLSTB)) {
    }

    /* enable the high-drive to extend the clock frequency to 120 Mhz */
    PMU_CTL |= PMU_CTL_HDEN;
    while(0U == (PMU_CS & PMU_CS_HDRF)) {
    }

    /* select the high-drive mode */
    PMU_CTL |= PMU_CTL_HDS;
    while(0U == (PMU_CS & PMU_CS_HDSRF)) {
    }

    /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLLP;

    /* wait until PLL is selected as system clock */
    while(0U == (RCU_CFG0 & RCU_SCSS_PLLP)) {
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t) ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
