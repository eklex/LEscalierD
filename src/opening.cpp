/**
 * This file is part of the LEscalierD project (https://github.com/eklex/LEscalierD).
 * Copyright (C) 2016, 2020  Alexandre Boni.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "lescalierd.h"

void solidcolor(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color)
{
  unsigned int strip_idx = 0;
  unsigned int led_idx   = 0;

  for(strip_idx = 0; strip_idx < strip_cnt; strip_idx++)
  {
    for(led_idx = 0; led_idx < led_cnt; led_idx++)
    {
      pLeds[strip_idx][led_idx] = color;
    }
  }
  FastLED.show();
}

void solidcolorStrip(CRGB_p *pLeds, unsigned int strip_idx, unsigned int strip_cnt, unsigned int led_cnt, CRGB color)
{
  unsigned int led_idx   = 0;

  if(strip_idx < strip_cnt)
  {
    for(led_idx = 0; led_idx < led_cnt; led_idx++)
    {
      pLeds[strip_idx][led_idx] = color;
    }
  }
  FastLED.show();
}

void openingLine(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color)
{
  int   strip_idx = 0;
  int   led_idx   = 0;
  CRGB *led_p     = NULL;

  for(led_idx = 0; led_idx < led_cnt; led_idx++)
  {
    for(strip_idx = 0; strip_idx < strip_cnt; strip_idx++)
    {
      led_p = (strip_idx % 2) ? &pLeds[strip_idx][led_cnt-led_idx-1] : &pLeds[strip_idx][led_idx];
      *led_p = color;
    }
    FastLED.show();
    delay(3);
  }
}

void openingEpicSides(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int rainbow, int order)
{
  int strip_idx = 0;
  int led_idx   = 0;

  /* turn border pixel on */
  for(strip_idx = (order > 0) ? 0 : strip_cnt - 1;
      ((order > 0) && (strip_idx < strip_cnt)) || ((order < 0) && (strip_idx >= 0));
      strip_idx += order)
  {
    if(rainbow)
    {
      pLeds[strip_idx][0]         = random(0, 0xFFFFFF);
      pLeds[strip_idx][led_cnt-1] = random(0, 0xFFFFFF);
    }
    else
    {
      pLeds[strip_idx][0] = color;
      pLeds[strip_idx][led_cnt-1] = color;
    }

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

      if(rainbow)
      {
        pLeds[strip_idx][led_idx] = random(0, 0xFFFFFF);
        pLeds[strip_idx][led_cnt-led_idx-1] = random(0, 0xFFFFFF);
      }
      else
      {
        pLeds[strip_idx][led_idx] = color;
        pLeds[strip_idx][led_cnt-led_idx-1] = color;
      }
    }
    FastLED.show();
    if(led_idx == led_cnt/2) delay(300);
    else delay(7);
  }
}

void openingEpicCenter(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int rainbow, int order)
{
  int strip_idx = 0;
  int led_idx   = 0;

  for(strip_idx = (order > 0) ? 0 : strip_cnt - 1;
      ((order > 0) && (strip_idx < strip_cnt)) || ((order < 0) && (strip_idx >= 0));
      strip_idx += order)
  {
    if(rainbow)
    {
      pLeds[strip_idx][led_cnt/2 + led_cnt%2] = random(0, 0xFFFFFF);
    }
    else
    {
      pLeds[strip_idx][led_cnt/2 + led_cnt%2] = color;
    }
    FastLED.show();
    delay(50);
  }

  for(led_idx = 0; led_idx < led_cnt/2 + led_cnt%2; led_idx++)
  {
    for(strip_idx = 0; strip_idx < strip_cnt; strip_idx++)
    {
      if(rainbow)
      {
        pLeds[strip_idx][led_cnt/2 - led_idx] = random(0, 0xFFFFFF);;
        pLeds[strip_idx][led_cnt/2 + led_idx] = random(0, 0xFFFFFF);;
      }
      else
      {
        pLeds[strip_idx][led_cnt/2 - led_idx] = color;
        pLeds[strip_idx][led_cnt/2 + led_idx] = color;
      }
    }
    FastLED.show();
    delay(7);
  }
}

void openingCenter(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int rainbow, int order)
{
  int strip_idx = 0;
  int led_idx   = 0;

  for(strip_idx = (order > 0) ? 0 : strip_cnt - 1;
      ((order > 0) && (strip_idx < strip_cnt)) || ((order < 0) && (strip_idx >= 0));
      strip_idx += order)
  {
    if(rainbow)
    {
      pLeds[strip_idx][led_cnt/2 + led_cnt%2] = random(0, 0xFFFFFF);
    }
    else
    {
      pLeds[strip_idx][led_cnt/2 + led_cnt%2] = color;
    }
    FastLED.show();
    delay(50);
  }
}

void openingCone(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int rainbow, int order)
{
  int  strip_idx = 0;
  int  led_idx   = 0;
  int  cone_idx  = 0;
  uint offset    = 0;

  for(strip_idx = (order > 0) ? 0 : strip_cnt - 1, cone_idx = strip_cnt - 1;
      ((order > 0) && (strip_idx < strip_cnt)) || ((order < 0) && (strip_idx >= 0));
      strip_idx += order, cone_idx--)
  {
    cone_idx = (cone_idx >= 0) ? cone_idx : 0;
    offset = cone_idx * ((led_cnt - 1) / (2 * (strip_cnt - 1)));
    for(led_idx = 0; led_idx < led_cnt - 2 * offset; led_idx++)
    {
      if(rainbow)
      {
        (pLeds[strip_idx] + offset)[led_idx] = random(0, 0xFFFFFF);
      }
      else
      {
        (pLeds[strip_idx] + offset)[led_idx] = color;
      }
    }
    FastLED.show();
    delay(50);
  }
}

void openingTree(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int rainbow, int order)
{
  int  strip_idx = 0;
  int  led_idx   = 0;
  uint offset    = 0;

  for(strip_idx = (order > 0) ? 0 : strip_cnt - 1;
      ((order > 0) && (strip_idx < strip_cnt)) || ((order < 0) && (strip_idx >= 0));
      strip_idx += order)
  {
    offset = strip_idx * ((led_cnt - 1) / (2 * (strip_cnt - 1)));
    for(led_idx = 0; led_idx < led_cnt - 2 * offset; led_idx++)
    {
      if(rainbow)
      {
        (pLeds[strip_idx] + offset)[led_idx] = random(0, 0xFFFFFF);
      }
      else
      {
        (pLeds[strip_idx] + offset)[led_idx] = color;
      }
    }
    FastLED.show();
    delay(50);
  }
}

void openingSides(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int rainbow, int order)
{
  int strip_idx = 0;
  int led_idx   = 0;

  /* turn border pixel on */
  for(strip_idx = (order > 0) ? 0 : strip_cnt - 1;
      ((order > 0) && (strip_idx < strip_cnt)) || ((order < 0) && (strip_idx >= 0));
      strip_idx += order)
  {
    if(rainbow)
    {
      pLeds[strip_idx][0] = random(0, 0xFFFFFF);
      pLeds[strip_idx][led_cnt-1] = random(0, 0xFFFFFF);
    }
    else
    {
      pLeds[strip_idx][0] = color;
      pLeds[strip_idx][led_cnt-1] = color;
    }
    FastLED.show();
    delay(50);
  }
}

void openingCascade(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int rainbow, int order)
{
  int step_idx  = 0;
  int strip_idx = 0;
  int led_idx   = 0;

  for(step_idx = 0; step_idx < strip_cnt; step_idx++)
  {
    for(strip_idx = (order < 0) ? 0 : strip_cnt - 1;
        ((order < 0) && (strip_idx < strip_cnt - step_idx)) ||
        ((order > 0) && (strip_idx >= step_idx));
        strip_idx -= order)
    {
      for(led_idx = 0; led_idx < led_cnt; led_idx++)
      {
        if(rainbow)
        {
          pLeds[strip_idx][led_idx] = random(0, 0xFFFFFF);
        }
        else
        {    
          pLeds[strip_idx][led_idx] = color;
        }
      }
      FastLED.show();
      if(((order < 0) && (strip_idx < strip_cnt - step_idx - 1)) ||
         ((order > 0) && (strip_idx >= step_idx + 1)))
      {
        for(led_idx = 0; led_idx < led_cnt; led_idx++)
        {
          pLeds[strip_idx][led_idx] = CRGB::Black;
        }
      }
      delay(50);
    }
  }
}

void openingStar(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int rainbow)
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

    if(rainbow)
    {
      pLeds[led / led_cnt][led % led_cnt] = random(0, 0xFFFFFF);
    }
    else
    {
      pLeds[led / led_cnt][led % led_cnt] = color;
    }
    FastLED.show();
#if 0
    if(index < strip_cnt*led_cnt/10) delay(80);
    if(index < strip_cnt*led_cnt/8) delay(40);
    else if(index < strip_cnt*led_cnt/4) delay(10);
    else if(index < strip_cnt*led_cnt/2) delay(2);
#endif
  }
}

void constellation(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int rainbow)
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
  if(led_idx >= strip_cnt * led_cnt)
  {
    solidcolor(pLeds, strip_cnt, led_cnt, CRGB::Black);
  }
  if(rainbow)
  {
    fadeIn(pLeds, led / led_cnt, led % led_cnt, random(0, 0xFFFFFF));
  }
  else
  {
    fadeIn(pLeds, led / led_cnt, led % led_cnt, color);
  }
  index++;
  if(index >= strip_cnt*led_cnt/2)
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
