/****************************************************************************
*  Copyright 2021 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
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
#include "platform/platform.h"

#include <stdio.h>

#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "arm_2d_disp_adapters.h"
#include "arm_2d_example_controls.h"

/*============================ MACROS ========================================*/
#undef this
#define this (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


static void system_init(void)
{
    platform_init();

    arm_2d_init();
    disp_adapter0_init();

}

void disp_adapter_nano_draw_example_blocking_version(void)
{
    /* on frame start */
    do {
        
    } while(0);

    DISP_ADAPTER0_NANO_DRAW() {

        arm_2d_canvas(ptTile, __top_canvas) {
            
            arm_lcd_text_set_colour(GLCD_COLOR_RED, GLCD_COLOR_WHITE);
            arm_lcd_printf_label(ARM_2D_ALIGN_CENTRE, "Arm-2D Nano Mode");
           
        }
    }

    /* on frame complete */
    do {

    } while(0);
}



void draw_gasgauge(void)
{

    struct {
        int64_t lTimestamp[1];
        battery_liquid_t    tBatteryLiquid;
        uint16_t            hwGasgauge;
        battery_status_t    tStatus;
        bool bOnLoad;
    }static s_tLocal = {.bOnLoad = true,}, *ptThis = &s_tLocal;

    if (this.bOnLoad) {
        this.bOnLoad = false;
        
        battery_gasgauge_liquid_init(&this.tBatteryLiquid);
    }
    

    /* on frame start */
    do {
        int32_t nResult;
        
        /* simulate a full battery charging/discharge cycle */
        arm_2d_helper_time_cos_slider(0, 1000, 30000, 0, &nResult, &this.lTimestamp[0]);
        
        if (this.hwGasgauge < nResult) {
            this.tStatus = BATTERY_STATUS_CHARGING;
        } else if (this.hwGasgauge > nResult) {
            this.tStatus = BATTERY_STATUS_DISCHARGING;
        }
        this.hwGasgauge = (uint16_t)nResult;
    } while(0);

    DISP_ADAPTER0_NANO_DRAW() {

        arm_2d_canvas(ptTile, __top_canvas) {
            
            arm_2d_align_centre(__top_canvas, 64, 100) {
            
                battery_gasgauge_liquid_show(   &this.tBatteryLiquid, 
                                                ptTile, 
                                                &__centre_region, 
                                                this.hwGasgauge,
                                                this.tStatus,
                                                bIsNewFrame);
                
                arm_2d_helper_dirty_region_update_item( 
                    &ptScene->tDirtyRegionHelper.tDefaultItem,
                    ptTile,
                    &__top_canvas,
                    &__centre_region);
                
                ARM_2D_OP_WAIT_ASYNC();
            
            }
           
        }
    }

    /* on frame complete */
    do {

    } while(0);
}


arm_fsm_rt_t draw_clock(void)
{
    struct {
        uint8_t chPT;
        uint8_t chHour;
        uint8_t chMin;
        uint8_t chSec;
        uint8_t chTenMs;

    }s_tLocal, *ptThis = &s_tLocal;

ARM_PT_BEGIN(this.chPT)

    /* on frame start */
    do {
        int64_t lTimeStampInMs = arm_2d_helper_convert_ticks_to_ms(
                                    arm_2d_helper_get_system_timestamp());

        /* calculate the hours */
        do {
            uint_fast8_t chHour = lTimeStampInMs / (3600ul * 1000ul);
            chHour %= 24;
            this.chHour = chHour;
            lTimeStampInMs %= (3600ul * 1000ul);
        } while(0);

        /* calculate the Minutes */
        do {
            uint_fast8_t chMin = lTimeStampInMs / (60ul * 1000ul);
            this.chMin = chMin;
            lTimeStampInMs %= (60ul * 1000ul);
        } while(0);

        /* calculate the Seconds */
        do {
            uint_fast8_t chSec = lTimeStampInMs / (1000ul);
            this.chSec = chSec;
            lTimeStampInMs %= (1000ul);
        } while(0);

        /* calculate the Ten-Miliseconds */
        do {
            uint_fast8_t chTenMs = lTimeStampInMs / (10ul);
            this.chTenMs = chTenMs;
        } while(0);
    } while(0);


    DISP_ADAPTER0_NANO_DRAW() {
 
        extern const arm_2d_tile_t c_tileCMSISLogoA4Mask;

        arm_2d_canvas(ptTile, __top_canvas) {

            arm_lcd_text_reset_display_region_tracking();
            
            arm_lcd_text_set_colour(GLCD_COLOR_WHITE, GLCD_COLOR_WHITE);
            arm_lcd_printf_label(
                ARM_2D_ALIGN_CENTRE, 
                "%02d:%02d:%02d:%02d", 
                this.chHour,
                this.chMin,
                this.chSec,
                this.chTenMs);
            
            arm_2d_region_t *ptTextRegion =  arm_lcd_text_get_last_display_region();

            arm_2d_helper_dirty_region_update_item( 
                &ptScene->tDirtyRegionHelper.tDefaultItem,
                ptTile,
                &__top_canvas,
                ptTextRegion);
        }

        /* You can ONLY yield here */
        ARM_PT_YIELD(arm_fsm_rt_on_going);
    }

    /* on frame complete */
    do {

    } while(0);

ARM_PT_END()

    return arm_fsm_rt_cpl;
}



int main(void) 
{
    system_init();

    __cycleof__("printf") {
        printf("Hello Tufty2040!\r\n");
    }

#if defined( __PERF_COUNTER_COREMARK__ ) && __PERF_COUNTER_COREMARK__
    printf("\r\nRun Coremark 1.0...\r\n");
    coremark_main();
#endif

    /* prepare and change canvas colour */
    disp_adapter0_nano_prepare()->tCanvas.wColour = GLCD_COLOR_GREEN;

    /* draw one frame */
    disp_adapter_nano_draw_example_blocking_version();
    
    perfc_delay_ms(1000);

    /* NOTE: 
     * 1. Please do NOT call disp_adapter0_nano_prepare() for each frame. 
     *    Usually you just need to call it once.
     * 2. You can call disp_adapter0_nano_prepare() at anytime to get 
     *    the ONLY and Default scene instance. 
     */
    arm_2d_scene_t *ptScene = disp_adapter0_get_default_scene();
    ptScene->tCanvas.wColour = GLCD_COLOR_BLACK;
    ptScene->bUseDirtyRegionHelper = true;

    disp_adapter0_nano_prepare();

    while (true) {

        //draw_clock();
        draw_gasgauge();

    }
    //return 0;
}
