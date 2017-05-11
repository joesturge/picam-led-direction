/*
 * neopixelring.h
 *
 * Copyright (c) 2017 James Prance
 */

#ifndef NEOPIXELRING_H
#define NEOPIXELRING_H

#include "ws2811.h"

#define ARRAY_SIZE(stuff)                        (sizeof(stuff) / sizeof(stuff[0]))

// defaults for cmdline options
#define TARGET_FREQ                              WS2811_TARGET_FREQ
#define GPIO_PIN                                 18
#define DMA                                      5
#define STRIP_TYPE                             WS2811_STRIP_RGB      // WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE                         WS2811_STRIP_GBR      // WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE                         SK6812_STRIP_RGBW      // SK6812RGBW (NOT SK6812RGB)

#define WIDTH                                    12
#define HEIGHT                                   1
#define LED_COUNT                                (WIDTH * HEIGHT)

int width = WIDTH;
int height = HEIGHT;
int led_count = LED_COUNT;

int clear_on_exit = 0;

#ifdef __cplusplus
extern "C" {
#endif

ws2811_t ledstring;
ws2811_return_t ret;

void neopixelInit();
void neopixelUpdate(int led, int red, int blue, int green);
void neopixelClose();
void neopixelClear(int led);

void initLEDstring(void);

ws2811_led_t *matrix;

void matrix_render(void);
void matrix_rasie(void);
void matrix_clear(void);
void matrix_bottom(void);

#ifdef __cplusplus
}
#endif

#endif // NEOPIXELRING_H
