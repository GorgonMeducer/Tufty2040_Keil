/*----------------------------------------------*/
/* zhRGB565 Decoder System Configurations 1.0.0 */
/*----------------------------------------------*/

#ifndef __ARM_2D_ZHRGB565_CFG_H__
#define __ARM_2D_ZHRGB565_CFG_H__

#include "arm_2d.h"

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>Qoi System Configurations
// =======================

// <q>Use Loader IO
// <i> This feature is disabled by default. If the compressed image is accessible inside the 4G memory space, please disable this option.
#ifndef __ARM_2D_ZHRGB565_USE_LOADER_IO__
#   define __ARM_2D_ZHRGB565_USE_LOADER_IO__                0
#endif

// <o>The pixel cache size <6-64>
// <i> Number of pixels in the cache. It must be at least 6 pixels. This option is valid only when __ARM_2D_ZHRGB565_USE_LOADER_IO__ is `1`
// <i> Default: 32
#ifndef __ARM_2D_ZHRGB565_PIXEL_CACHE_SIZE__
#   define __ARM_2D_ZHRGB565_PIXEL_CACHE_SIZE__             32
#endif

//</h>
// <<< end of configuration section >>>

#endif


