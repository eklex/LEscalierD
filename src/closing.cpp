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

void closingLine(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color)
{
  int   strip_idx = 0;
  int   led_idx   = 0;
  CRGB *led_p     = NULL;

  for(led_idx = 0; led_idx < led_cnt; led_idx++)
  {
    for(strip_idx = 0; strip_idx < strip_cnt; strip_idx++)
    {
      led_p = (strip_idx % 2) ? &pLeds[strip_idx][led_cnt-led_idx-1] : &pLeds[strip_idx][led_idx];
      *led_p = CRGB::Black;
    }
    FastLED.show();
    delay(10);
  }
}
