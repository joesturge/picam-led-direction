/*
 * neopixelring.c
 *
 * Copyright (c) 2017 James Prance
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdarg.h>
#include <getopt.h>


#include "clk.h"
#include "gpio.h"
#include "dma.h"
#include "pwm.h"
#include "version.h"
#include "ws2811.h"

#include "neopixelring.h"

ws2811_led_t dotcolors[] =
{
    0x00200000,  // red
    0x00201000,  // orange
    0x00202000,  // yellow
    0x00002000,  // green
    0x00002020,  // lightblue
    0x00000020,  // blue
    0x00100010,  // purple
    0x00200010,  // pink
};

ws2811_led_t dotcolors_rgbw[] =
{
    0x00200000,  // red
    0x10200000,  // red + W
    0x00002000,  // green
    0x10002000,  // green + W
    0x00000020,  // blue
    0x10000020,  // blue + W
    0x00101010,  // white
    0x10101010,  // white + W

};

void neopixelInit() {
  ws2811_return_t ret;

  matrix = (ws2811_led_t*)malloc(sizeof(ws2811_led_t) * width * height);

  //setup_handlers();
  initLEDstring();

  if ((ret = ws2811_init(&ledstring)) != WS2811_SUCCESS)
  {
      fprintf(stderr,"ws2811_init failed: %s\n",ws2811_get_return_t_str(ret));
      //cerr<<ret<<endl;
  }
}

void neopixelClose() {
    ledstring.channel[0].brightness = 0;
  	ws2811_render(&ledstring);
    ws2811_fini(&ledstring);
}

void neopixelUpdate(int led, int red, int blue, int green) {

    int rgb = _RGBtoHex(red,blue,green);

    ledstring.channel[0].leds[led] =  rgb;

    if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
    {
        fprintf(stderr,"ws2811_render failed: %s\n",ws2811_get_return_t_str(ret));
        //break;
    }
}

void neopixelClear(int led) {

    ledstring.channel[0].leds[led] =  0x00000000;

    ws2811_return_t ret;
    if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
    {
        fprintf(stderr,"ws2811_render failed: %s\n",ws2811_get_return_t_str(ret));
        //break;
    }
}

void initLEDstring() {
    ws2811_t init = {
        .freq = TARGET_FREQ,
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
        },
    };
    ledstring = init;
}

int _RGBtoHex(uint red, uint green, uint blue) {
  int total = blue+(green<<8)+(red<<16);
  return total;
}
