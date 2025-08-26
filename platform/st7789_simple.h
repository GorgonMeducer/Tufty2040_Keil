
#ifndef __ST7789_SIMPLE_H__
#define __ST7789_SIMPLE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ST7789_WIDTH   320
#define ST7789_HEIGHT  240

#ifndef ST7789_PIN_D0
#define ST7789_PIN_D0   14
#endif
#ifndef ST7789_PIN_WR
#define ST7789_PIN_WR   12
#endif
#ifndef ST7789_PIN_DC
#define ST7789_PIN_DC   11
#endif
#ifndef ST7789_PIN_CS
#define ST7789_PIN_CS   10
#endif
#ifndef ST7789_PIN_RD
#define ST7789_PIN_RD   13
#endif
#ifndef ST7789_PIN_RST
#define ST7789_PIN_RST  -1
#endif
#ifndef ST7789_PIN_BL
#define ST7789_PIN_BL   2 
#endif

#ifndef ST7789_PIO_INSTANCE
#define ST7789_PIO_INSTANCE  1  // 0 => pio0, 1 => pio1
#endif

#ifndef ST7789_PIO_CLKDIV
#define ST7789_PIO_CLKDIV    3
#endif


void st7789_Init(void);
void st7789_DrawBitmap(int16_t x, int16_t y, int16_t width, int16_t height, const uint8_t *pchBitmap);


#ifdef __cplusplus
}
#endif

#endif
