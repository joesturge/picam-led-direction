#ifndef NEOPIXELRING_H
#define NEOPIXELRING_H

#include "ws2811.h"

#define ARRAY_SIZE(stuff)                        (sizeof(stuff) / sizeof(stuff[0]))

// defaults for cmdline options
#define TARGET_FREQ                              WS2811_TARGET_FREQ
#define GPIO_PIN                                 18
#define DMA                                      5
#define STRIP_TYPE				                 WS2811_STRIP_RGB		// WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE				             WS2811_STRIP_GBR		// WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE				             SK6812_STRIP_RGBW		// SK6812RGBW (NOT SK6812RGB)

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

void neopixelInit();
void neopixelUpdate(int led, int red, int blue, int green);
void neopixelClose();
//void ws2811_render();
//void ws2811_init();
//void ws2811_fini();
//void ws2811_get_return_t_str();



ws2811_t ledstring;
ledstring.freq = TARGET_FREQ;
//ledstring.freq = 6;
ledstring.dmanum = DMA;
ledstring.channel[0].gpionum = GPIO_PIN;
ledstring.channel[0].count = LED_COUNT;
ledstring.channel[0].invert = 0;
ledstring.channel[0].brightness = 255;
ledstring.channel[0].strip_type = STRIP_TYPE;
ledstring.channel[1].gpionum = 0;
ledstring.channel[1].count = 0;
ledstring.channel[1].invert = 0;
ledstring.channel[1].brightness = 0;
/*ws2811_t ledstring =
{
    //"ledstring",
    .freq = 800000,
    .dmanum = DMA,
    .channel =
    {
        [0] =
        {
            .gpionum = GPIO_PIN,
            .count = LED_COUNT,
            .invert = 0,
            .brightness = 255,
            .strip_type = STRIP_TYPE,
        },
        [1] =
        {
            .gpionum = 0,
            .count = 0,
            .invert = 0,
            .brightness = 0,
        },
    },
};*/

//extern struct

ws2811_led_t *matrix;

void matrix_render(void);
void matrix_rasie(void);
void matrix_clear(void);
void matrix_bottom(void);


#ifdef __cplusplus
}
#endif

//void neopixelInit(void);
//void neopixelClose(void);
//void neopixelUpdate(int led, int red, int blue, int green);



#endif // NEOPIXELRING_H
