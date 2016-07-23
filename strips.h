#ifndef __STRIPS_H__
#define __STRIPS_H__

#include "application.h"
#include "leds.h"
#include "debug.h"

#undef RAINBOW

/* Display rules */
#define TOP2BOTTOM  (-1)
#define BOTTOM2TOP  (1)

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

void openingEpicSides (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int order);
void openingEpicCenter(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int order);
void openingCenter    (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int order);
void openingCone      (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int order);
void openingSides     (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int order);
void openingLine      (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color);
void openingStar      (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color);
void constellation    (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color);
void solidcolor       (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color);
void solidcolorStrip  (CRGB_p *pLeds, unsigned int strip_idx, unsigned int strip_cnt, unsigned int led_cnt, CRGB color);

void closingLine(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color);

void publishCmd();
int processCloudCmd(String cmd);

char** cmdsplit(const char *str, const char *sep, int *argc, unsigned long int *val);
void cmdfree(char** str, int str_cnt);
unsigned long int strnum(char *str);

#endif /* __STRIPS_H__ */
