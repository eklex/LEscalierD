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

#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "application.h"

#define ANIMA_CONTINOUS  (1 << 0)
#define ANIMA_OPEN       (1 << 1)
#define ANIMA_CLOSE      (1 << 2)
#define ANIMA_ORDER      (1 << 3)
#define ANIMA_RAINBOW    (1 << 4)

typedef void (*anima_func_w2_t)(CRGB_p*, unsigned int, unsigned int, CRGB, int, int);
typedef void (*anima_func_w1_t)(CRGB_p*, unsigned int, unsigned int, CRGB, int);
typedef void (*anima_func_no_t)(CRGB_p*, unsigned int, unsigned int, CRGB);

typedef struct _anima_def_t {
  String          name;
  anima_func_w2_t func;
  union {
    uint8_t raw;
    struct {
      uint8_t continous:1;
      uint8_t open     :1;
      uint8_t close    :1;
      uint8_t order    :1;
      uint8_t rainbow  :1;
    };
  } support;
} anima_def_t;

typedef struct _light_mode_t {
  uint32_t   temperature;
  uint32_t   delay;
  uint32_t   color;
  uint8_t    random;
  uint8_t    brightness;
  uint8_t    rainbow;
} light_mode_t;

typedef struct _light_control_t {
  uint8_t    manual;
  struct {
    uint8_t  enable;
    uint32_t value;
  } timer;
} light_control_t;

typedef struct _main_control_t {
  light_mode_t       light;
  light_control_t    control;
  const led_strip_t *strip;
  const anima_def_t *animation;
} main_control_t;

extern main_control_t main_mode;

const anima_def_t* getAnimation(String name);
const anima_def_t* getAnimation(unsigned int index);
void runAnimation(const anima_def_t *animation, const led_strip_t *strip, uint32_t color, int rainbow, int order);

#endif /* __ANIMATION_H__ */
