/*!
    \file    gd32f470z_lcd_eval.c
    \brief   firmware functions to manage LCD

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

#include "gd32f470z_lcd_eval.h"


#define LCD_SPI_CS(a)   \
                        if (a)  \
                        gpio_bit_set(LCD_CS_GPIO_PORT, LCD_CS_PIN); \
                        else        \
                        gpio_bit_reset(LCD_CS_GPIO_PORT, LCD_CS_PIN);
#define SPI_DCLK(a) \
                        if (a)  \
                        gpio_bit_set(LCD_SPI_SCK_GPIO_PORT, LCD_SPI_SCK_PIN);   \
                        else        \
                        gpio_bit_reset(LCD_SPI_SCK_GPIO_PORT, LCD_SPI_SCK_PIN);
#define SPI_SDA(a)  \
                        if (a)  \
                        gpio_bit_set(LCD_SPI_MOSI_GPIO_PORT, LCD_SPI_MOSI_PIN); \
                        else        \
                        gpio_bit_reset(LCD_SPI_MOSI_GPIO_PORT, LCD_SPI_MOSI_PIN);
static void delay(uint32_t count);

/*!
    \brief      enable the LCD
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_enable(void)
{
    gpio_bit_set(LCD_CS_GPIO_PORT, LCD_CS_PIN);
}

/*!
    \brief      disable the LCD
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_disable(void)
{
    gpio_bit_reset(LCD_CS_GPIO_PORT, LCD_CS_PIN);
}

/*!
    \brief      configure the LCD control line
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_ctrl_line_config(void)
{
    /* enable GPIOs clock*/
    rcu_periph_clock_enable(LCD_CS_GPIO_CLK);
    rcu_periph_clock_enable(LCD_RS_GPIO_CLK);

    /* configure LCD_CS_GPIO_PORT(PD11) and LCD_RS_GPIO_PORT(PE3) */
    gpio_mode_set(LCD_CS_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_CS_PIN);
    gpio_output_options_set(LCD_CS_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,LCD_CS_PIN);

    gpio_mode_set(LCD_RS_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_RS_PIN);
    gpio_output_options_set(LCD_RS_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,LCD_RS_PIN);

    /* set the chip select pin */
    lcd_ctrl_line_set(LCD_CS_GPIO_PORT, LCD_CS_PIN);
}

/*!
    \brief      set the LCD control line
    \param[in]  gpiox: control line GPIO
      \arg        LCD_CS_GPIO_PORT: LCD chip select GPIO
      \arg        LCD_RS_GPIO_PORT: LCD register/RAM selection GPIO
    \param[in]  gpiopin: control line pin
      \arg        LCD_CS_PIN: LCD chip select pin
      \arg        LCD_RS_PIN: LCD register/RAM selection pin
    \param[out] none
    \retval     none
*/
void lcd_ctrl_line_set(uint32_t gpiox, uint16_t gpiopin)
{
    gpio_bit_set(gpiox, gpiopin);
}

/*!
    \brief      reset the LCD control line
    \param[in]  gpiox: control line GPIO
      \arg        LCD_CS_GPIO_PORT: LCD chip select GPIO
      \arg        LCD_RS_GPIO_PORT: LCD register/RAM selection GPIO
    \param[in]  gpiopin: control line pin
      \arg        LCD_CS_PIN: LCD chip select pin
      \arg        LCD_RS_PIN: LCD register/RAM selection pin
    \param[out] none
    \retval     none
*/
void lcd_ctrl_line_reset(uint32_t gpiox, uint16_t gpiopin)
{
    gpio_bit_reset(gpiox, gpiopin);
}

/*!
    \brief      configure the LCD SPI and it's GPIOs
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_spi_config(void)
{
#ifndef USE_LCD_VERSION_1_4
    spi_parameter_struct spi_init_struct;
    rcu_periph_clock_enable(LCD_SPI_SCK_GPIO_CLK);
    rcu_periph_clock_enable(LCD_SPI_MOSI_GPIO_CLK);
    rcu_periph_clock_enable(LCD_SPI_CLK);

    /* configure SPI5_SCK(PG13) and SPI5_MOSI(PG14) */
    gpio_af_set(LCD_SPI_SCK_GPIO_PORT, GPIO_AF_5, LCD_SPI_SCK_PIN);
    gpio_af_set(LCD_SPI_MOSI_GPIO_PORT, GPIO_AF_5, LCD_SPI_MOSI_PIN);
    
    gpio_mode_set(LCD_SPI_SCK_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LCD_SPI_SCK_PIN);
    gpio_output_options_set(LCD_SPI_SCK_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_SPI_SCK_PIN);
    gpio_mode_set(LCD_SPI_MOSI_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LCD_SPI_MOSI_PIN);
    gpio_output_options_set(LCD_SPI_MOSI_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_SPI_MOSI_PIN);
    spi_i2s_deinit(SPI5);
    
    if(0 == (SPI_CTL0(LCD_SPI) & SPI_CTL0_SPIEN)){    
        spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
        spi_init_struct.device_mode          = SPI_MASTER;
        spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
        spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
        spi_init_struct.nss                  = SPI_NSS_SOFT;
        spi_init_struct.prescale             = SPI_PSC_16;
        spi_init_struct.endian               = SPI_ENDIAN_MSB;
        spi_init(LCD_SPI, &spi_init_struct);
        spi_enable(LCD_SPI);
    }
#else
    /* GPIO clock enable */
    rcu_periph_clock_enable(LCD_SPI_SCK_GPIO_CLK);
    rcu_periph_clock_enable(LCD_SPI_MOSI_GPIO_CLK);

    /* configure the LCD SPI pins */
    gpio_mode_set(LCD_SPI_SCK_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_SPI_SCK_PIN);
    gpio_output_options_set(LCD_SPI_SCK_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_SPI_SCK_PIN);
    gpio_mode_set(LCD_SPI_MOSI_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_SPI_MOSI_PIN);
    gpio_output_options_set(LCD_SPI_MOSI_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_SPI_MOSI_PIN);
#endif
}

/*!
    \brief      write command to select LCD register
    \param[in]  lcd_register: the address of the selected register
    \param[out] none
    \retval     none
*/
void lcd_command_write(uint8_t lcd_register)
{
    /* reset LCD_RS to send command */
    lcd_ctrl_line_reset(LCD_RS_GPIO_PORT, LCD_RS_PIN);

    /* reset LCD control line and send command */
    lcd_disable();
    while(RESET == spi_i2s_flag_get(LCD_SPI, SPI_FLAG_TBE)) ;
    spi_i2s_data_transmit(LCD_SPI, lcd_register);

    /* wait until a data is sent */
    while(RESET != spi_i2s_flag_get(LCD_SPI, SPI_FLAG_TRANS));

    lcd_enable();
}

/*!
    \brief      write data to select LCD register
    \param[in]  value: the value that will be written to the selected register
    \param[out] none
    \retval     none
*/
void lcd_data_write(uint8_t value)
{
    /* set LCD_RS to send data */
    lcd_ctrl_line_set(LCD_RS_GPIO_PORT, LCD_RS_PIN);

    /* reset LCD control line and send data */  
    lcd_disable();
    while(RESET == spi_i2s_flag_get(LCD_SPI, SPI_FLAG_TBE)) ;

    spi_i2s_data_transmit(LCD_SPI, value);

    /* wait until a data is sent */
    while(RESET != spi_i2s_flag_get(LCD_SPI, SPI_FLAG_TRANS)) ;

    lcd_enable();
}

/*!
    \brief      GPIO emulated SPI byte write
    \param[in]  byte: data to be sent
    \param[out] none
    \retval     none
*/
void spi_io_byte_write(unsigned char byte)
{
    unsigned char n;

    for(n = 0; n < 8; n++) {
        if(byte & 0x80) {
            SPI_SDA(1)
        } else {
            SPI_SDA(0)
        }
        byte <<= 1;

        SPI_DCLK(0);
        SPI_DCLK(1);
    }
}

/*!
    \brief      GPIO emulated SPI write command
    \param[in]  cmd: command to be sent
    \param[out] none
    \retval     none
*/
void spi_io_comm_write(uint8_t cmd)
{
    LCD_SPI_CS(0);
    SPI_SDA(0);
    SPI_DCLK(0);
    SPI_DCLK(1);
    spi_io_byte_write(cmd);

    LCD_SPI_CS(1);
}

/*!
    \brief      GPIO emulated SPI write data
    \param[in]  tem_data: data to be sent
    \param[out] none
    \retval     none
*/
void spi_io_data_write(uint8_t tem_data)
{
    LCD_SPI_CS(0);
    SPI_SDA(1);
    SPI_DCLK(0);
    SPI_DCLK(1);
    spi_io_byte_write(tem_data);
    LCD_SPI_CS(1);
}

/*!
    \brief      configure the LCD based on the power on sequence(for V1.1 LCD or earlier)
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_power_on(void)
{
    lcd_command_write(0x11);
    delay(120);
    lcd_command_write(0x36);
    lcd_data_write(0x48);
    lcd_command_write(0x3A);
    lcd_data_write(0x55);
    lcd_command_write(0xB4);
    lcd_data_write(0x11);
    lcd_command_write(0xB3);
    lcd_data_write(0x00);
    lcd_data_write(0x00);
    lcd_data_write(0x00);
    lcd_data_write(0x20);
    lcd_command_write(0xC0);
    lcd_data_write(0x10);
    lcd_data_write(0x3B);
    lcd_data_write(0x00);
    lcd_data_write(0x12);
    lcd_data_write(0x01);
    lcd_command_write(0xC5);
    lcd_data_write(0x07);
    lcd_command_write(0xC8);
    lcd_data_write(0x01 );
    lcd_data_write(0x36);
    lcd_data_write(0x00);
    lcd_data_write(0x02);
    lcd_data_write(0x00);
    lcd_data_write(0x1C);
    lcd_data_write(0x77);
    lcd_data_write(0x14);
    lcd_data_write(0x67);
    lcd_data_write(0x20);
    lcd_data_write(0x0E);
    lcd_data_write(0x00);
    lcd_command_write(0xD0);
    lcd_data_write(0x44);
    lcd_data_write(0x41 );
    lcd_data_write(0x08);
    lcd_data_write(0xC2);
    lcd_command_write(0xD1);
    lcd_data_write(0x50);
    lcd_data_write(0x11);
    lcd_command_write(0xD2);
    lcd_data_write(0x05);
    lcd_data_write(0x12);

    lcd_command_write(0xC6);
    lcd_data_write(0x83);
    lcd_command_write(0x29);
    delay(5);
}

/*!
    \brief      configure the LCD based on the power on sequence 1(for V1.2 LCD)
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_power_on1(void)
{
    lcd_command_write(0xC0); //power control1 command/w/
    lcd_data_write(0x0A);    //P-Gamma level//4.1875v
    lcd_data_write(0x0A);    //N-Gamma level
    lcd_command_write(0xC1); //BT & VC Setting//power contrl2 command/w/
    lcd_data_write(0x41);
    lcd_data_write(0x07);    //VCI1 = 2.5V
    lcd_command_write(0xC2); //DC1.DC0 Setting//power control3 for normal mode
    lcd_data_write(0x33);
    lcd_command_write(0xC5); //VCOM control
    lcd_data_write(0x00);    //NV memory is not programmed
    lcd_data_write(0x42);    //VCM Setting
    lcd_data_write(0x80);    //VCM Register Enable
    lcd_command_write(0xB0); //interface mode control //Polarity Setting
    lcd_data_write(0x02);
    lcd_command_write(0xB1); //frame rate control for normal mode
    lcd_data_write(0xB0);    //Frame Rate Setting//70 frame per second//no division for internal clocks
    lcd_data_write(0x11);    //17 clocks per line period for idle mode at cpu interface
    lcd_command_write(0xB4); //dispaly inversion control
    lcd_data_write(0x00);    // disable Z-inversion , column inversion
    lcd_command_write(0xB6); //display function control// RM.DM Setting
    lcd_data_write(0x70);    //0xF0
    lcd_data_write(0x02);    //direction of gate scan: G1->G480 one by one, source scan: S1->S960, scan cycle if interval scan in non-display area
    lcd_data_write(0x3B);    //number of lines to drive LCD: 8*(0x3C) = 480
    lcd_command_write(0xB7); //Entry Mode
    lcd_data_write(0x07);    //disable low voltage detection, normal display, 
    lcd_command_write(0xF0); //Enter ENG , must be set before gamma setting
    lcd_data_write(0x36);
    lcd_data_write(0xA5);
    lcd_data_write(0xD3);
    lcd_command_write(0xE5); //Open gamma function , must be set before gamma setting
    lcd_data_write(0x80);
    lcd_command_write(0xE5); //Page 1
    lcd_data_write(0x01);
    lcd_command_write(0XB3); //WEMODE=0(Page 1) , pixels over window setting will be ignored.//frame rate control in partial mode/full colors
    lcd_data_write(0x00);
    lcd_command_write(0xE5); //Page 0
    lcd_data_write(0x00);
    lcd_command_write(0xF0); //Exit ENG , must be set before gamma setting
    lcd_data_write(0x36);
    lcd_data_write(0xA5);
    lcd_data_write(0x53);
    lcd_command_write(0xE0); //Gamma setting
    //y fine adjustment register for positive polarity
    lcd_data_write(0x00);
    lcd_data_write(0x35);
    lcd_data_write(0x33);
    //y gradient adjustment register for positive polarity
    lcd_data_write(0x00);
    //y amplitude adjustment register for positive polarity
    lcd_data_write(0x00);
    lcd_data_write(0x00);
    //y fine adjustment register for negative polarity
    lcd_data_write(0x00);
    lcd_data_write(0x35);
    lcd_data_write(0x33);
    //y gradient adjustment register for negative polarity
    lcd_data_write(0x00);
    //y amplitude adjustment register for negative polarity
    lcd_data_write(0x00);
    lcd_data_write(0x00);
    lcd_command_write(0x36); //memory data access control
    lcd_data_write(0x48);
    lcd_command_write(0x3A); //interface pixel format setting
    lcd_data_write(0x55);    //16-bits
    lcd_command_write(0x11); //Exit sleep mode
    lcd_command_write(0x29); //Display on 

    delay(10);
}

/*!
    \brief      configure the LCD based on the power on sequence 2(for V1.3 LCD, TK035F3296)
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_power_on2(void)
{
    delay(120);

    lcd_command_write(0xB9);
    lcd_data_write(0xFF); 
    lcd_data_write(0x83); 
    lcd_data_write(0x57); 
    delay(50);

    lcd_command_write(0xB3); 
    lcd_data_write(0x03);         //SDO_EN,BYPASS,EPF[1:0],0,0,RM,DM
    lcd_data_write(0x00);         //DPL,HSPL,VSPL,EPL
    lcd_data_write(0x06);         //RCM, HPL[5:0]
    lcd_data_write(0x06);         //VPL[5:0]

    lcd_command_write(0x21); 
    lcd_command_write(0x11); 
    delay(50);

    lcd_command_write(0x36);      //Memory Data Access Control
    lcd_data_write(0x48);         //RGB565, 16bit/pixel
        
    lcd_command_write(0x3A);      //Interface Pixel Format,
    lcd_data_write(0x66);         //66 for RGB666

    delay(50);
    lcd_command_write(0x29);      //Display ON 
    delay(50);

    delay(120);
}

void lcd_power_on3(void)
{
    delay(120);

    LCD_SPI_CS(1);
    delay(20);
    LCD_SPI_CS(0);

    spi_io_comm_write(0xE0);      //P-Gamma
    spi_io_data_write(0x00);
    spi_io_data_write(0x10);
    spi_io_data_write(0x14);
    spi_io_data_write(0x03);
    spi_io_data_write(0x0E);
    spi_io_data_write(0x04);
    spi_io_data_write(0x36);
    spi_io_data_write(0x56);
    spi_io_data_write(0x4B);
    spi_io_data_write(0x04);
    spi_io_data_write(0x0C);
    spi_io_data_write(0x0A);
    spi_io_data_write(0x30);
    spi_io_data_write(0x34);
    spi_io_data_write(0x0F);

    spi_io_comm_write(0XE1);      //N-Gamma
    spi_io_data_write(0x00);
    spi_io_data_write(0x0E);
    spi_io_data_write(0x13);
    spi_io_data_write(0x03);
    spi_io_data_write(0x10);
    spi_io_data_write(0x06);
    spi_io_data_write(0x3E);
    spi_io_data_write(0x34);
    spi_io_data_write(0x55);
    spi_io_data_write(0x05);
    spi_io_data_write(0x0F);
    spi_io_data_write(0x0E);
    spi_io_data_write(0x3A);
    spi_io_data_write(0x3E);
    spi_io_data_write(0x0F);

    spi_io_comm_write(0XC0);      //Power Control 1
    spi_io_data_write(0x0F);      //Vreg1out
    spi_io_data_write(0x0C);      //Verg2out

    spi_io_comm_write(0xC1);      //Power Control 2
    spi_io_data_write(0x41);      //VGH,VGL

    spi_io_comm_write(0xC5);      //Power Control 3
    spi_io_data_write(0x00);
    spi_io_data_write(0x21);      //Vcom
    spi_io_data_write(0x80);

    spi_io_comm_write(0x2a);
    spi_io_data_write(0 >> 8);
    spi_io_data_write(0);
    spi_io_data_write(LCD_PIXEL_WIDTH >> 8);
    spi_io_data_write((uint8_t)LCD_PIXEL_WIDTH);

    spi_io_comm_write(0x2b);
    spi_io_data_write(0 >> 8);
    spi_io_data_write(0);
    spi_io_data_write(LCD_PIXEL_HEIGHT >> 8);
    spi_io_data_write((uint8_t)LCD_PIXEL_HEIGHT);

    spi_io_comm_write(0x36);      //Memory Access
    spi_io_data_write(0x48);

    spi_io_comm_write(0x3A);      // Interface Pixel Format
    spi_io_data_write(0x66);      //18bit
    //spi_io_data_write(0x55);    //16bit

    spi_io_comm_write(0XB0);      // Interface Mode Control
    spi_io_data_write(0x00);

    spi_io_comm_write(0xB1);      //Frame rate
    spi_io_data_write(0xA0);      //60Hz

    spi_io_comm_write(0xB4);      //Display Inversion Control
    spi_io_data_write(0x02);      //2-dot

    spi_io_comm_write(0XB6);      //RGB/MCU Interface Control
    spi_io_data_write(0x22);      //MCU
    spi_io_data_write(0x02);      //Source,Gate scan dieection


    spi_io_comm_write(0XE9);      // Set Image Function
    spi_io_data_write(0x00);      //disable 24 bit data input

    spi_io_comm_write(0xF7);      //Adjust Control
    spi_io_data_write(0xA9);
    spi_io_data_write(0x51);
    spi_io_data_write(0x2C);
    spi_io_data_write(0x82);      //D7 stream, loose

    spi_io_comm_write(0x21);      //Normal Black

    spi_io_comm_write(0x11);      //Sleep out
    delay(120);
    spi_io_comm_write(0x29);      //Display on

    delay(120);
}

/*!
    \brief      insert a delay time
    \param[in]  count: delay time
    \param[out] none
    \retval     none
*/
static void delay(__IO uint32_t count)
{
    __IO uint32_t index = 0;
    for(index = count; index != 0; index--) {
    }
}
