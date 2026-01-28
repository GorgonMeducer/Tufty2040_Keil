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
#include "arm_2d_scenes.h"
#include "arm_2d_demos.h"

#ifdef RTE_Acceleration_Arm_2D_Extra_Benchmark
#   include "arm_2d_benchmark.h"
#endif

#include "arm_2d_scene_histogram.h"
#include "arm_2d_scene_meter.h"
#include "arm_2d_scene_bubble_charging.h"


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


void scene_rickrolling_loader(void) 
{
    arm_2d_scene_rickrolling_init(&DISP0_ADAPTER);
}

void scene_zhrgb565_loader(void) 
{
    arm_2d_scene_zhrgb565_init(&DISP0_ADAPTER);
}

void scene_tjpgd_loader(void) 
{
    arm_2d_scene_tjpgd_init(&DISP0_ADAPTER);
}

void scene_qoi_loader(void) 
{
    arm_2d_scene_qoi_init(&DISP0_ADAPTER);
}

void scene_qoi_animation_loader(void) 
{
    arm_2d_scene_qoi_animation_init(&DISP0_ADAPTER);
}

void scene_radars_loader(void) 
{
    arm_2d_scene_radars_init(&DISP0_ADAPTER);
}

void scene_blink_loader(void) 
{
    arm_2d_scene_blink_init(&DISP0_ADAPTER);
}

void scene_mask_generator_loader(void) 
{
    arm_2d_scene_mask_generation_init(&DISP0_ADAPTER);
}

void scene_flight_attitude_instrument_loader(void) 
{
    arm_2d_scene_flight_attitude_instrument_init(&DISP0_ADAPTER);
}

void scene_panel_loader(void) 
{
    arm_2d_scene_panel_init(&DISP0_ADAPTER);
}

void scene_histogram_loader(void) 
{
    arm_2d_scene_histogram_init(&DISP0_ADAPTER);
}

void scene_meter_loader(void) 
{
    arm_2d_scene_meter_init(&DISP0_ADAPTER);
}

void scene_qrcode_loader(void) 
{
    arm_2d_scene_qrcode_init(&DISP0_ADAPTER);
}

void scene_user_defined_opcode_loader(void) 
{
    arm_2d_scene_user_defined_opcode_init(&DISP0_ADAPTER);
}

void scene_space_badge_loader(void) 
{
    arm_2d_scene_space_badge_init(&DISP0_ADAPTER);
}

void scene_music_player_loader(void) 
{
    arm_2d_scene_music_player_init(&DISP0_ADAPTER);
}

void scene_bubble_charging_loader(void) 
{
    arm_2d_scene_bubble_charging_init(&DISP0_ADAPTER);
}

void scene_gas_gauge_loader(void) 
{
    arm_2d_scene_gas_gauge_init(&DISP0_ADAPTER);
}

void scene_watch_face_01_loader(void) 
{
    arm_2d_scene_watch_face_01_init(&DISP0_ADAPTER);
}

void scene_matrix_loader(void) 
{
    arm_2d_scene_matrix_init(&DISP0_ADAPTER);
}

void scene_ring_indicator_loader(void) 
{
    arm_2d_scene_ring_indicator_init(&DISP0_ADAPTER);
}

void scene_waveform_loader(void) 
{
    arm_2d_scene_waveform_init(&DISP0_ADAPTER);
}

void scene_virtual_resource_loader(void) 
{
    arm_2d_scene_virtual_resource_init(&DISP0_ADAPTER);
}


typedef struct demo_scene_t {
    int32_t nLastInMS;
    void (*fnLoader)(void);
} demo_scene_t;

static demo_scene_t const c_SceneLoaders[] = {

#if 1
    {
        20000,
        scene_radars_loader,
    },
    {
        20000,
        scene_blink_loader,
    },
    {
        20000,
        scene_space_badge_loader,
    },
#else
    {
        .fnLoader = 
        scene_radars_loader,
        //scene_qoi_loader
        //scene_zhrgb565_loader,
        //scene_bubble_charging_loader,
        //scene_watch_face_01_loader,
        //scene_waveform_loader,
        //scene_mask_generator_loader,
        //scene_ring_indicator_loader,
        //scene_meter_loader,
        //scene_histogram_loader,
        //scene_blink_loader,
        //scene_histogram_loader,
        //scene_flight_attitude_instrument_loader,
        //scene_radars_loader,
        //scene_music_player_loader,
        //scene_meter_loader,
        //scene_rickrolling_loader,
        //scene_space_badge_loader,
        //scene_qrcode_loader,
        //scene_mono_clock_loader
    },
#endif

};

static
struct {
    int8_t chIndex;
    bool bIsTimeout;
    int32_t nDelay;
    int64_t lTimeStamp;
    
} s_tDemoCTRL = {
    .chIndex = -1,
    .bIsTimeout = true,
};

/* load scene one by one */
void before_scene_switching_handler(void *pTarget,
                                    arm_2d_scene_player_t *ptPlayer,
                                    arm_2d_scene_t *ptScene)
{

    switch (arm_2d_scene_player_get_switching_status(&DISP0_ADAPTER)) {
        case ARM_2D_SCENE_SWITCH_STATUS_MANUAL_CANCEL:
            s_tDemoCTRL.chIndex--;
            break;
        default:
            s_tDemoCTRL.chIndex++;
            break;
    }

    if (s_tDemoCTRL.chIndex >= dimof(c_SceneLoaders)) {
        s_tDemoCTRL.chIndex = 0;
    } else if (s_tDemoCTRL.chIndex < 0) {
        s_tDemoCTRL.chIndex += dimof(c_SceneLoaders);
    }

    /* call loader */
    arm_with(const demo_scene_t, &c_SceneLoaders[s_tDemoCTRL.chIndex]) {
        if (_->nLastInMS > 0) {
            s_tDemoCTRL.bIsTimeout = false;
            s_tDemoCTRL.lTimeStamp = 0;
            s_tDemoCTRL.nDelay = _->nLastInMS;
        }
        _->fnLoader();
    }
}


static void system_init(void)
{
    platform_init();

    arm_2d_init();
    disp_adapter0_init();

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
#ifdef RTE_Acceleration_Arm_2D_Extra_Benchmark
    arm_2d_run_benchmark();
#else
    arm_2d_scene_player_register_before_switching_event_handler(
            &DISP0_ADAPTER,
            before_scene_switching_handler);
            
    arm_2d_scene_player_switch_to_next_scene(&DISP0_ADAPTER);
#endif
    while (true) {

        disp_adapter0_task(60);

        if (!s_tDemoCTRL.bIsTimeout) {

            if (arm_2d_helper_is_time_out(s_tDemoCTRL.nDelay, &s_tDemoCTRL.lTimeStamp)) {
                s_tDemoCTRL.bIsTimeout = true;

                arm_2d_scene_player_switch_to_next_scene(&DISP0_ADAPTER);
            }
        }

    }
    //return 0;
}
