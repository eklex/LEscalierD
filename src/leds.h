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

#ifndef __LEDS_H__
#define __LEDS_H__

#include "application.h"
#include "FastLED/FastLED.h"
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
