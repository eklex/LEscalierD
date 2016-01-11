#include "strips.h"

void solidblack(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt)
{
  int strip_idx = 0;
  int led_idx   = 0;

  for(strip_idx = 0; strip_idx < strip_cnt; strip_idx++)
  {
    memset(pLeds[strip_idx], 0, led_cnt * sizeof(CRGB));
  }
  FastLED.show();
}

void solidwhite(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt)
{
  int strip_idx = 0;
  int led_idx   = 0;

  for(strip_idx = 0; strip_idx < strip_cnt; strip_idx++)
  {
    memset(pLeds[strip_idx], 0xff, led_cnt * sizeof(CRGB));
  }
  FastLED.show();
}

void solidcolor(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color)
{
  int strip_idx = 0;
  int led_idx   = 0;

  for(strip_idx = 0; strip_idx < strip_cnt; strip_idx++)
  {
    for(led_idx = 0; led_idx < led_cnt; led_idx++)
    {
      pLeds[strip_idx][led_idx] = color;
    }
  }
  FastLED.show();
}

void opening(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt)
{
  int   strip_idx = 0;
  int   led_idx   = 0;
  CRGB *led_p     = NULL;

  for(led_idx = 0; led_idx < led_cnt; led_idx++)
  {
    for(strip_idx = 0; strip_idx < strip_cnt; strip_idx++)
    {
      led_p = (strip_idx % 2) ? &pLeds[strip_idx][led_cnt-led_idx-1] : &pLeds[strip_idx][led_idx];
      *led_p = (strip_idx % 2) ? CRGB::Blue : CRGB::Red;
    }
    FastLED.show();
    delay(50);
  }
  for(led_idx = 0; led_idx < led_cnt; led_idx++)
  {
    for(strip_idx = 0; strip_idx < strip_cnt; strip_idx++)
    {
      led_p = (strip_idx % 2) ? &pLeds[strip_idx][led_cnt-led_idx-1] : &pLeds[strip_idx][led_idx];
      *led_p = CRGB::Black;
    }
    FastLED.show();
    delay(50);
  }
}

void openingEpic(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, int order)
{
  int strip_idx = 0;
  int led_idx   = 0;

  /* turn border pixel on */
  for(strip_idx = (order > 0) ? 0 : strip_cnt - 1;
      ((order > 0) && (strip_idx < strip_cnt)) || ((order < 0) && (strip_idx >= 0));
      strip_idx += order)
  {
#ifdef RAINBOW
    pLeds[strip_idx][0]         = random(0, 0xFFFFFF);
    pLeds[strip_idx][led_cnt-1] = random(0, 0xFFFFFF);
#else
    pLeds[strip_idx][0] = CRGB::White;
    pLeds[strip_idx][led_cnt-1] = CRGB::White;
#endif
    FastLED.show();
    delay(50);
  }
  /* move the border pixel on each strip to the center,
   * and turn every pixels on beyond the center */
  for(led_idx = 0; led_idx < led_cnt; led_idx++)
  {
    for(strip_idx = 0; strip_idx < strip_cnt; strip_idx++)
    {
      if(led_idx > 0 && led_idx < led_cnt/2 + led_cnt%2)
      {
        pLeds[strip_idx][led_idx-1] = CRGB::Black;
        pLeds[strip_idx][led_cnt-led_idx] = CRGB::Black;
      }
#ifdef RAINBOW
      pLeds[strip_idx][led_idx] = random(0, 0xFFFFFF);
      pLeds[strip_idx][led_cnt-led_idx-1] = random(0, 0xFFFFFF);
#else
      pLeds[strip_idx][led_idx] = CRGB::White;
      pLeds[strip_idx][led_cnt-led_idx-1] = CRGB::White;
#endif
    }
    FastLED.show();
    if(led_idx == led_cnt/2) delay(300);
    else delay(7);
  }
}

void openingCenter(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, int order)
{
  int strip_idx = 0;
  int led_idx   = 0;

  for(strip_idx = (order > 0) ? 0 : strip_cnt - 1;
      ((order > 0) && (strip_idx < strip_cnt)) || ((order < 0) && (strip_idx >= 0));
      strip_idx += order)
  {
    pLeds[strip_idx][led_cnt/2 + led_cnt%2] = CRGB::White;
    FastLED.show();
    delay(50);
  }

  for(led_idx = 0; led_idx < led_cnt/2 + led_cnt%2; led_idx++)
  {
    for(strip_idx = 0; strip_idx < strip_cnt; strip_idx++)
    {
      pLeds[strip_idx][led_cnt/2 - led_idx] = CRGB::White;
      pLeds[strip_idx][led_cnt/2 + led_idx] = CRGB::White;
    }
    FastLED.show();
    delay(7);
  }
}

void openingCone(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, int order)
{
  int  strip_idx = 0;
  int  cone_idx  = 0;
  uint offset    = 0;

  for(strip_idx = (order > 0) ? 0 : strip_cnt - 1, cone_idx = strip_cnt - 1;
      ((order > 0) && (strip_idx < strip_cnt)) || ((order < 0) && (strip_idx >= 0));
      strip_idx += order, cone_idx--)
  {
    cone_idx = (cone_idx >= 0) ? cone_idx : 0;
    offset = cone_idx * ((led_cnt - 1) / (2 * (strip_cnt - 1)));
    memset(pLeds[strip_idx] + offset, 0xFF, (led_cnt - 2 * offset) * sizeof(CRGB));
    FastLED.show();
    delay(50);
  }
}

void openingStar(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt)
{
  int index = 0;
  uint led = 0;
  uint r, g, b;

  while(index++ < strip_cnt*led_cnt)
  {
    do
    {
      led = random(0, strip_cnt*led_cnt);
    } while(pLeds[led / led_cnt][led % led_cnt]);

#ifdef RAINBOW
    pLeds[led / led_cnt][led % led_cnt] = random(0, 0xFFFFFF);
#else
    pLeds[led / led_cnt][led % led_cnt] = CRGB::White;
#endif
    FastLED.show();

    if(index < strip_cnt*led_cnt/10) delay(80);
    if(index < strip_cnt*led_cnt/8) delay(40);
    else if(index < strip_cnt*led_cnt/4) delay(10);
    else if(index < strip_cnt*led_cnt/2) delay(2);
  }
}

void constellation(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt)
{
  static int index = 0;
  uint led_idx = 0;
  uint led = 0;
  int fade_idx = 0;

  do
  {
    led = random(0, strip_cnt*led_cnt);
  } while(pLeds[led / led_cnt][led % led_cnt] != (CRGB)0x000000
          && led_idx++ < strip_cnt * led_cnt);
  if(led_idx >= strip_cnt * led_cnt) solidblack(pLeds, strip_cnt, led_cnt);
#ifdef RAINBOW
  fadeIn(pLeds, led / led_cnt, led % led_cnt, random(0, 0xFFFFFF));
#else
  fadeIn(pLeds, led / led_cnt, led % led_cnt, CRGB::White);
#endif
  index++;
  if(index >= strip_cnt*led_cnt/6)
  {
    do
    {
      led = random(0, strip_cnt*led_cnt);
    } while(pLeds[led / led_cnt][led % led_cnt] == (CRGB)0x000000);

    fadeOut(pLeds, led / led_cnt, led % led_cnt);
    index--;
  }
  FastLED.show();
}
