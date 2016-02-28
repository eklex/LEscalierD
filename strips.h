#ifndef __STRIPS_H__
#define __STRIPS_H__

#include "application.h"
#include "FastLED.h"
FASTLED_USING_NAMESPACE;

#undef RAINBOW
#define SERIAL_DEBUG
#undef ACC_PULL

/* Number of strips */
#define STRIP_CNT   (8)
/* Number of LEDs per strip */
#define LED_CNT     (45)

/* Pin assigned per strip */
#define STRIP_1     (D6)
#define STRIP_2     (D7)
#define STRIP_3     (D4)
#define STRIP_4     (D3)
#define STRIP_5     (D2)
#define STRIP_6     (D1)
#define STRIP_7     (D5)
#define STRIP_8     (D0)

/* Display rules */
#define TOP2BOTTOM  (-1)
#define BOTTOM2TOP  (1)

typedef CRGB CRGB_p[LED_CNT];

typedef struct _led_strip_t {
  CRGB_p      *leds;
  unsigned int strip_cnt;
  unsigned int led_cnt;
} led_strip_t;

typedef struct _cloudcmd_t
{
  char *rawcmd;
  char **argv;
  int  argc;
} cloudcmd_t;


void fadeIn    (CRGB_p *pLeds, unsigned int strip_idx, unsigned int led_idx, CRGB color);
void fadeOut   (CRGB_p *pLeds, unsigned int strip_idx, unsigned int led_idx, CRGB color);
void fadeOut   (CRGB_p *pLeds, unsigned int strip_idx, unsigned int led_idx);
void fadeOutAll(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, unsigned int scale = 2);

void openingEpic  (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int order);
void openingCenter(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int order);
void openingCone  (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int order);
void openingLine  (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color);
void openingStar  (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color);
void constellation(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color);
void solidcolor   (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color);

void closingLine(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color);

int processCloudCmd(String cmd);

char** cmdsplit(const char *str, const char *sep, int *argc, unsigned long int *val);
void cmdfree(char** str, int str_cnt);
unsigned long int strnum(char *str);

#endif /* __STRIPS_H__ */
