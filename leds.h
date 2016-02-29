#ifndef __LEDS_H__
#define __LEDS_H__

#include "application.h"
#include "FastLED.h"
FASTLED_USING_NAMESPACE;

/* Number of strips */
#define STRIP_CNT   (8)
/* Number of LEDs per strip */
#define LED_CNT     (45)

typedef CRGB CRGB_p[LED_CNT];

typedef struct _led_strip_t {
  CRGB_p      *leds;
  unsigned int strip_cnt;
  unsigned int led_cnt;
} led_strip_t;

const led_strip_t* led_setup(void);



#endif /* __LEDS_H__ */
