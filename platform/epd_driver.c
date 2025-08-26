/****************************************************************************
*  Copyright 2025 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
*                                                                           *
*  Licensed under the Apache License, Version 2.0 (the "License");          *
*  you may not use this file except in compliance with the License.         *
*  You may obtain a copy of the License at                                  *
*                                                                           *
*     http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                           *
*  Unless required by applicable law or agreed to in writing, software      *
*  distributed under the License is distributed on an "AS IS" BASIS,        *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
*  See the License for the specific language governing permissions and      *
*  limitations under the License.                                           *
*                                                                           *
****************************************************************************/
/*============================ INCLUDES ======================================*/
#include "./platform.h"

#include "hardware/spi.h"

#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "arm_2d_disp_adapters.h"

/*============================ MACROS ========================================*/
#ifndef EPD_SCREEN_HEIGHT
#   define EPD_SCREEN_HEIGHT 296
#endif

#ifndef EPD_SCREEN_WIDTH
#   define EPD_SCREEN_WIDTH  128
#endif

#ifndef EPD_SPI_FREQ
#   define EPD_SPI_FREQ 12000000ul
#endif

#ifndef SPI_PORT
#   define SPI_PORT    spi0
#endif
/*============================ MACROFIED FUNCTIONS ===========================*/

#define SEND_LUT(__CMD, __LUT)                                                  \
    do {                                                                        \
        epd_send_cmd(__CMD);                                                    \
        epd_send_data((uint8_t *)(__LUT), sizeof(__LUT));                       \
    } while(0)

#define __epd_send_cmd2(__cmd, __ptr, __size)                                   \
                __epd_send_cmd((__cmd), (uint8_t *)(__ptr), sizeof(__size))
#define __epd_send_cmd1(__cmd, __array)                                         \
                __epd_send_cmd((__cmd), (uint8_t *)(__array), sizeof(__array))
#define __epd_send_cmd0(__cmd)                                                  \
                __epd_send_cmd((__cmd), NULL, 0)

#define epd_send_cmd(__cmd, ...)                                                \
                PERFC_CONNECT2( __epd_send_cmd,                                 \
                                __PLOOC_VA_NUM_ARGS(__VA_ARGS__))               \
                                    ((__cmd),##__VA_ARGS__)

#define epd_send_cmd_with_data(__cmd, ...)                                      \
    do {                                                                        \
         uint8_t chData[] = __VA_ARGS__;                                        \
        epd_send_cmd((__cmd), chData);                                          \
    } while(0)



/*============================ TYPES =========================================*/

enum {
    PANEL_SETTING                             = 0x00,
    POWER_SETTING                             = 0x01,
    POWER_OFF                                 = 0x02,
    POWER_OFF_SEQUENCE_SETTING                = 0x03,
    POWER_ON                                  = 0x04,
    POWER_ON_MEASURE                          = 0x05,
    BOOSTER_SOFT_START                        = 0x06,
    DEEP_SLEEP                                = 0x07,
    DATA_START_TRANSMISSION_1                 = 0x10,
    DATA_STOP                                 = 0x11,
    DISPLAY_REFRESH                           = 0x12,
    DATA_START_TRANSMISSION_2                 = 0x13,
    LUT_VCOM                                  = 0x20,
    LUT_WW                                    = 0x21,
    LUT_BW                                    = 0x22,
    LUT_WB                                    = 0x23,
    LUT_BB                                    = 0x24,
    PLL_CONTROL                               = 0x30,
    TEMPERATURE_SENSOR_COMMAND                = 0x40,
    TEMPERATURE_SENSOR_CALIBRATION            = 0x41,
    TEMPERATURE_SENSOR_WRITE                  = 0x42,
    TEMPERATURE_SENSOR_READ                   = 0x43,
    VCOM_AND_DATA_INTERVAL_SETTING            = 0x50,
    LOW_POWER_DETECTION                       = 0x51,
    TCON_SETTING                              = 0x60,
    TCON_RESOLUTION                           = 0x61,
    SOURCE_AND_GATE_START_SETTING             = 0x62,
    GET_STATUS                                = 0x71,
    AUTO_MEASURE_VCOM                         = 0x80,
    VCOM_VALUE                                = 0x81,
    VCM_DC_SETTING_REGISTER                   = 0x82,
    PARTIAL_WINDOW                            = 0x90,
    PARTIAL_IN                                = 0x91,
    PARTIAL_OUT                               = 0x92,
    PROGRAM_MODE                              = 0xA0,
    ACTIVE_PROGRAMMING                        = 0xA1,
    READ_OTP                                  = 0xA2,
    POWER_SAVING                              = 0xE3,
};

enum {

    EPD_CS_PIN    = 17,
    EPD_CLK_PIN   = 18,
    EPD_MOSI_PIN  = 19,
    EPD_DC_PIN    = 20,
    EPD_RST_PIN   = 21,
    EPD_BUSY_PIN  = 26,

};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static volatile bool s_bInvertColor = false;
static volatile bool s_bEnableDither = true;

static const uint8_t c_chDitherTable[16][4][4] = {
    [0] = {0},
    [1] = { {0, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},},
    [2] = { {0, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 1},},
    [3] = { {0, 0, 0, 0},
            {0, 1, 0, 1},
            {0, 0, 0, 0},
            {0, 0, 0, 1},},
    [4] = { {0, 0, 0, 0},
            {0, 1, 0, 1},
            {0, 0, 0, 0},
            {0, 1, 0, 1},},

    [5] = { {0, 0, 0, 0},
            {0, 1, 0, 1},
            {0, 0, 1, 0},
            {0, 1, 0, 1},},
    [6] = { {1, 0, 0, 0},
            {0, 1, 0, 1},
            {0, 0, 1, 0},
            {0, 1, 0, 1},},
    [7] = { {1, 0, 1, 0},
            {0, 1, 0, 1},
            {0, 0, 1, 0},
            {0, 1, 0, 1},},
    [8] = { {1, 0, 1, 0},
            {0, 1, 0, 1},
            {1, 0, 1, 0},
            {0, 1, 0, 1},},

    [9] = { {1, 1, 1, 0},
            {0, 1, 0, 1},
            {1, 0, 1, 0},
            {0, 1, 0, 1},},
    [10] = {{1, 1, 1, 0},
            {0, 1, 0, 1},
            {1, 0, 1, 1},
            {0, 1, 0, 1},},
    [11] = {{1, 1, 1, 1},
            {0, 1, 0, 1},
            {1, 0, 1, 1},
            {0, 1, 0, 1},},
    [12] = {{1, 1, 1, 1},
            {0, 1, 0, 1},
            {1, 1, 1, 1},
            {0, 1, 0, 1},},

    [13] = {{1, 1, 1, 1},
            {1, 1, 0, 1},
            {1, 1, 1, 1},
            {0, 1, 0, 1},},
    [14] = {{1, 1, 1, 1},
            {1, 1, 0, 1},
            {1, 1, 1, 1},
            {0, 1, 1, 1},},
    [15] = {{1, 1, 1, 1},
            {1, 1, 1, 1},
            {1, 1, 1, 1},
            {1, 1, 1, 1},},
};
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void epd_spi_write_byte(uint8_t Value)
{
    spi_write_blocking((spi_inst_t *)SPI_PORT, &Value, 1);
}

void epd_spi_write(uint8_t *pchBuff, size_t tSize)
{
    spi_write_blocking((spi_inst_t *)SPI_PORT, pchBuff, tSize);
}

bool epd_screen_set_invert_colour_mode(bool bInvert)
{
    bool bOldSetting = false;
    __IRQ_SAFE {
        bOldSetting = s_bInvertColor;
        s_bInvertColor = bInvert;
    }
    
    return bOldSetting;
}

bool epd_screen_set_dither_mode(bool bEnable)
{
    bool bOldSetting = false;
    __IRQ_SAFE {
        bOldSetting = s_bEnableDither;
        s_bEnableDither = bEnable;
    }
    
    return bOldSetting;
}

static void epd_screen_reset(void)
{
    gpio_put(EPD_RST_PIN, 1);
    perfc_delay_ms(200);
    gpio_put(EPD_RST_PIN, 0);
    perfc_delay_ms(2);
    gpio_put(EPD_RST_PIN, 1);
    perfc_delay_ms(200);
}

static void __epd_send_cmd(uint8_t chCMD, uint8_t *pchData, size_t tSize)
{
    gpio_put(EPD_DC_PIN, 0);
    gpio_put(EPD_CS_PIN, 0);
    epd_spi_write_byte(chCMD);
    
    
    gpio_put(EPD_DC_PIN, 1);
    if (pchData != NULL && tSize > 0) {
        epd_spi_write(pchData, tSize);
    }
    
    gpio_put(EPD_CS_PIN, 1);
}


static void epd_send_byte(uint8_t Data)
{
    gpio_put(EPD_DC_PIN, 1);
    gpio_put(EPD_CS_PIN, 0);
    epd_spi_write_byte(Data);
    gpio_put(EPD_CS_PIN, 1);
}

static void epd_send_data(uint8_t *pchBuff, size_t tSize)
{
    gpio_put(EPD_DC_PIN, 1);
    gpio_put(EPD_CS_PIN, 0);
    epd_spi_write(pchBuff, tSize);
    gpio_put(EPD_CS_PIN, 1);
}

__STATIC_INLINE 
void epd_read_busy(void)
{
    do {
        epd_send_cmd(GET_STATUS);
    } while(!(gpio_get(EPD_BUSY_PIN) & 0x01));
}

bool epd_screen_is_busy(void)
{
    epd_send_cmd(GET_STATUS);
    return !(gpio_get(EPD_BUSY_PIN) & 0x01);
}

void epd_screen_init(void)
{
    /* IO configuration */
    do {
        gpio_set_function(EPD_RST_PIN, GPIO_FUNC_SIO);
        gpio_init(EPD_RST_PIN);
        gpio_set_dir(EPD_RST_PIN, GPIO_OUT);
        gpio_put(EPD_RST_PIN, 1);

        gpio_set_function(EPD_DC_PIN, GPIO_FUNC_SIO);
        gpio_init(EPD_DC_PIN);
        gpio_set_dir(EPD_DC_PIN, GPIO_OUT);
        gpio_put(EPD_DC_PIN, 1);

        gpio_set_function(EPD_CS_PIN, GPIO_FUNC_SIO);
        gpio_init(EPD_CS_PIN);
        gpio_set_dir(EPD_CS_PIN, GPIO_OUT);
        gpio_put(EPD_CS_PIN, 1);

        gpio_set_function(EPD_BUSY_PIN, GPIO_FUNC_SIO);
        gpio_init(EPD_BUSY_PIN);
        gpio_set_dir(EPD_BUSY_PIN, GPIO_IN);

        spi_init((spi_inst_t *)SPI_PORT, EPD_SPI_FREQ);
        gpio_set_function(EPD_CLK_PIN, GPIO_FUNC_SPI);
        gpio_set_function(EPD_MOSI_PIN, GPIO_FUNC_SPI);
    } while(0);

    epd_screen_reset();

    epd_send_cmd_with_data(POWER_SETTING, {0x03, 0x00, 0x26, 0x2b, 0x03});

    epd_send_cmd_with_data(BOOSTER_SOFT_START, {
        0x17,                   //!< A
        0x17,                   //!< B
        0x17});                 //!< C

    epd_send_cmd(POWER_ON);
    epd_read_busy();

    epd_send_cmd_with_data(PANEL_SETTING, {
        0xbf,                   //!< 128x296
        0x0e,                   //!< VCOM to 0V fast
        });

    epd_send_cmd_with_data(PLL_CONTROL, {
        0x3A                                                                    // 3a 100HZ   29 150Hz 39 200HZ 31 171HZ
    });                                

    epd_send_cmd_with_data(TCON_RESOLUTION, {
        EPD_SCREEN_WIDTH,                                                       //!< width
        (EPD_SCREEN_HEIGHT >> 8) & 0xff,                                        //!< height high byte
        EPD_SCREEN_HEIGHT & 0xff,                                               //!< height low byte
    });

    epd_send_cmd_with_data(VCM_DC_SETTING_REGISTER, {0x28});
}

static void epd_set_partial_refresh_mode(void)
{
    epd_send_cmd(VCM_DC_SETTING_REGISTER);
    epd_send_byte(0x00);
    epd_send_cmd(VCOM_AND_DATA_INTERVAL_SETTING);
    epd_send_byte(0xb7);
    
    epd_send_cmd_with_data( LUT_VCOM, {
        0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00,
    });

    epd_send_cmd_with_data( LUT_WW, {
        0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    });
    
    epd_send_cmd_with_data( LUT_BW, {
        0x80, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    });

    epd_send_cmd_with_data( LUT_WB, {
        0x40, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    });

    epd_send_cmd_with_data( LUT_BB, {
        0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    });
}

static void epd_set_full_refresh_mode(void)
{
    epd_send_cmd(VCOM_AND_DATA_INTERVAL_SETTING);
    epd_send_byte(0xb7);

    epd_send_cmd_with_data( LUT_VCOM, {
        0x00, 0x08, 0x00, 0x00, 0x00, 0x02,
        0x60, 0x28, 0x28, 0x00, 0x00, 0x01,
        0x00, 0x14, 0x00, 0x00, 0x00, 0x01,
        0x00, 0x12, 0x12, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00,});

    epd_send_cmd_with_data( LUT_WW, {
        0x40, 0x08, 0x00, 0x00, 0x00, 0x02,
        0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
        0x40, 0x14, 0x00, 0x00, 0x00, 0x01,
        0xA0, 0x12, 0x12, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    });
    
    epd_send_cmd_with_data( LUT_BW, {
        0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
        0x90, 0x0F, 0x0F, 0x00, 0x00, 0x03,
        0x40, 0x0A, 0x01, 0x00, 0x00, 0x01,
        0xA0, 0x0E, 0x0E, 0x00, 0x00, 0x02,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    });

    epd_send_cmd_with_data( LUT_WB, {
        0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
        0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
        0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
        0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    });

    epd_send_cmd_with_data( LUT_BB, {
        0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
        0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
        0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
        0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    });

}

void epd_sceen_clear(void)
{
    int16_t iWidth = (EPD_SCREEN_WIDTH + 7) >> 3;
    int16_t iHeight =  EPD_SCREEN_HEIGHT;

    epd_send_cmd(DATA_START_TRANSMISSION_1);
    for (int16_t j = 0; j < iHeight; j++) {
        for (int16_t i = 0; i < iWidth; i++) {
            epd_send_byte(0x00);
        }
    }

    epd_send_cmd(DATA_START_TRANSMISSION_2);
    for (int16_t j = 0; j < iHeight; j++) {
        for (int16_t i = 0; i < iWidth; i++) {
            epd_send_byte(0xFF);
        }
    }

    epd_set_full_refresh_mode();
    epd_flush();
    while(epd_screen_is_busy()) __NOP();
}

static
void epd_screen_set_window(int16_t iX, int16_t iY, int16_t iWidth, int16_t iHeight)
{
    epd_set_partial_refresh_mode();
    epd_send_cmd(PARTIAL_IN);                   //This command makes the display enter partial mode
    
    int16_t iYEnd = iY + iHeight - 1;

    epd_send_cmd_with_data(PARTIAL_WINDOW, {
        iX,
        iX + iWidth - 1,
        iY >> 8,
        iY & 0xFF,
        iYEnd,
        iYEnd & 0xFF,
        0x28,
    });

}

__STATIC_INLINE
uint8_t __epd_dither(uint8_t chGray8, int16_t iX, int16_t iY)
{
    iX &= 0x03;
    iY &= 0x03;
    
    return c_chDitherTable[chGray8 >> 4][iY][iX];
}

void EPD_DrawBitmap(int16_t iX, int16_t iY, int16_t iWidth, int16_t iHeight, const uint8_t *pchBuffer)
{
    assert((iX & 0x7) == 0);
    assert((iWidth & 0x7) == 0);

    iX += iWidth - 1;

    int16_t iRotatedX = iY;
    int16_t iRotatedY = EPD_SCREEN_HEIGHT - iX - 1;
    int16_t iRotatedWidth = iHeight;
    int16_t iRotatedHeight = iWidth;

    epd_screen_set_window(iRotatedX, iRotatedY, iRotatedWidth, iRotatedHeight);

    epd_send_cmd(DATA_START_TRANSMISSION_2);

    gpio_put(EPD_DC_PIN, 1);
    gpio_put(EPD_CS_PIN, 0);

    if (s_bEnableDither) {
        for (int16_t i = 0; i < iRotatedHeight; i++) {

            for (int16_t j = 0; j < iRotatedWidth;) {
                uint8_t chData = 0;

                const uint8_t *pchBlock = &pchBuffer[iWidth - i - 1 + j * iWidth];

                chData |= __epd_dither(*pchBlock, j++, i);
                pchBlock += iWidth;
                chData <<= 1;
                
                chData |= __epd_dither(*pchBlock, j++, i);
                pchBlock += iWidth;
                chData <<= 1;
                
                chData |= __epd_dither(*pchBlock, j++, i);
                pchBlock += iWidth;
                chData <<= 1;
                
                chData |= __epd_dither(*pchBlock, j++, i);
                pchBlock += iWidth;
                chData <<= 1;
                
                chData |= __epd_dither(*pchBlock, j++, i);
                pchBlock += iWidth;
                chData <<= 1;
                
                chData |= __epd_dither(*pchBlock, j++, i);
                pchBlock += iWidth;
                chData <<= 1;
                
                chData |= __epd_dither(*pchBlock, j++, i);
                pchBlock += iWidth;
                chData <<= 1;
                
                chData |= __epd_dither(*pchBlock, j++, i);
                pchBlock += iWidth;

                if (s_bInvertColor) { 
                    chData = ~chData; 
                }
                epd_spi_write_byte(chData);
            
            }
        }
    } else {
    
        for (int16_t i = 0; i < iRotatedHeight; i++) {

            for (int16_t j = 0; j < iRotatedWidth; j+= 8) {
                uint8_t chData = 0;

                const uint8_t *pchBlock = &pchBuffer[iWidth - i - 1 + j * iWidth];

                chData |=  *pchBlock >= 0x80 ? 0x01 : 0x00;
                pchBlock += iWidth;
                chData <<= 1;
                
                chData |= *pchBlock >= 0x80 ? 0x01 : 0x00;
                pchBlock += iWidth;
                chData <<= 1;
                
                chData |= *pchBlock >= 0x80 ? 0x01 : 0x00;
                pchBlock += iWidth;
                chData <<= 1;
                
                chData |= *pchBlock >= 0x80 ? 0x01 : 0x00;
                pchBlock += iWidth;
                chData <<= 1;
                
                chData |= *pchBlock >= 0x80 ? 0x01 : 0x00;
                pchBlock += iWidth;
                chData <<= 1;
                
                chData |= *pchBlock >= 0x80 ? 0x01 : 0x00;
                pchBlock += iWidth;
                chData <<= 1;
                
                chData |= *pchBlock >= 0x80 ? 0x01 : 0x00;
                pchBlock += iWidth;
                chData <<= 1;
                
                chData |= *pchBlock >= 0x80 ? 0x01 : 0x00;
                pchBlock += iWidth;

                if (s_bInvertColor) { 
                    chData = ~chData; 
                }
                epd_spi_write_byte(chData);
            
            }
        }
    }
    
    gpio_put(EPD_CS_PIN, 1);
    
    epd_send_cmd(DATA_STOP);
    epd_send_cmd(PARTIAL_OUT);

}

void Disp0_DrawBitmap(  int16_t x, 
                        int16_t y, 
                        int16_t width, 
                        int16_t height, 
                        const uint8_t *bitmap)
{
    EPD_DrawBitmap(x, y, width, height, bitmap);
}

void epd_flush(void)
{
    epd_send_cmd(DISPLAY_REFRESH);
}