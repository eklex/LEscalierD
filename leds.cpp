#include "leds.h"

/* Pin assigned per strip */
#define STRIP_1     (D6)
#define STRIP_2     (D7)
#define STRIP_3     (D4)
#define STRIP_4     (D3)
#define STRIP_5     (D2)
#define STRIP_6     (D1)
#define STRIP_7     (D5)
#define STRIP_8     (D0)

/* LED array */
static CRGB leds[STRIP_CNT][LED_CNT] = {CRGB::Black};
static const led_strip_t led_strip = {leds, STRIP_CNT, LED_CNT};

const led_strip_t* led_setup(void)
{
  FastLED.addLeds<WS2812, STRIP_1, GRB>(leds[0], LED_CNT);
  FastLED.addLeds<WS2812, STRIP_2, GRB>(leds[1], LED_CNT);
  FastLED.addLeds<WS2812, STRIP_3, GRB>(leds[2], LED_CNT);
  FastLED.addLeds<WS2812, STRIP_4, GRB>(leds[3], LED_CNT);
  FastLED.addLeds<WS2812, STRIP_5, GRB>(leds[4], LED_CNT);
  FastLED.addLeds<WS2812, STRIP_6, GRB>(leds[5], LED_CNT);
  FastLED.addLeds<WS2812, STRIP_7, GRB>(leds[6], LED_CNT);
  FastLED.addLeds<WS2812, STRIP_8, GRB>(leds[7], LED_CNT);
  FastLED.show();
  return &led_strip;
}
