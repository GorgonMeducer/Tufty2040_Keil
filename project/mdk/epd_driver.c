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

#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "arm_2d_disp_adapters.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


static
void EPD_SetWindow(int16_t iX, int16_t iY, int16_t iWidth, int16_t iHeight)
{
    EPD_2IN9D_SetPartReg();
    EPD_2IN9D_SendCommand(0x91);            //This command makes the display enter partial mode
    EPD_2IN9D_SendCommand(0x90);            //resolution setting
    

    EPD_2IN9D_SendData(iX);                 //x-start
    EPD_2IN9D_SendData(iX + iWidth - 1);    //x-end

    EPD_2IN9D_SendData(iY >> 8);
    EPD_2IN9D_SendData(iY & 0xFF);          //y-start
    
    iY += iHeight - 1;
    EPD_2IN9D_SendData(iY);
    EPD_2IN9D_SendData(iY & 0xFF);          //y-end
    EPD_2IN9D_SendData(0x28);
}

void EPD_DrawBitmap(int16_t iX, int16_t iY, int16_t iWidth, int16_t iHeight, const uint8_t *pchBuffer)
{
    assert((iX & 0x7) == 0);
    assert((iWidth & 0x7) == 0);


    iX += iWidth - 1;

    int16_t iRotatedX = iY;
    int16_t iRotatedY = EPD_2IN9D_HEIGHT - iX - 1;
    int16_t iRotatedWidth = iHeight;
    int16_t iRotatedHeight = iWidth;

    EPD_SetWindow(iRotatedX, iRotatedY, iRotatedWidth, iRotatedHeight);
    
    EPD_2IN9D_SendCommand(0x13);

    for (int16_t i = 0; i < iRotatedHeight; i++) {

        for (int16_t j = 0; j < iRotatedWidth; j+= 8) {
            uint8_t chData = 0;

            const uint8_t *pchBlock = &pchBuffer[iWidth - i - 1 + j * iWidth];
            
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
            chData <<= 1;
            
            chData |= *pchBlock >= 0x80 ? 0x01 : 0x00;
            pchBlock += iWidth;

            EPD_2IN9D_SendData(chData);
        
        }
    }
    
    //EPD_2IN9D_SendCommand(0x11);
    EPD_2IN9D_SendCommand(0x92);

}

void Disp0_DrawBitmap(  int16_t x, 
                        int16_t y, 
                        int16_t width, 
                        int16_t height, 
                        const uint8_t *bitmap)
{
    EPD_DrawBitmap(x, y, width, height, bitmap);
}

bool Disp0_Flush(void)
{
    static enum {
        START = 0,
        WAIT_BUSY,
    } s_chState = START;
    
    switch (s_chState) {
        case START:
            s_chState++;
            EPD_2IN9D_SendCommand(0x12);
            //fall-through;
        case WAIT_BUSY:
            EPD_2IN9D_SendCommand(0x71);
            uint8_t busy = gpio_get(26);
            busy =!(busy & 0x01);
            if (busy) {
                break;
            }
            s_chState = START;
            return true;

        default:
            s_chState = START;
            break;
    }

    return false;
}