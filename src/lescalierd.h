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

#ifndef __LESCALIERD_H__
#define __LESCALIERD_H__

#include "application.h"
#include "leds.h"
#include "debug.h"

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

void openingEpicSides (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int rainbow, int order);
void openingEpicCenter(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int rainbow, int order);
void openingCenter    (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int rainbow, int order);
void openingCone      (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int rainbow, int order);
void openingSides     (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int rainbow, int order);
void openingCascade   (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int rainbow, int order);
void openingTree      (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int rainbow, int order);
void openingLine      (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color);
void openingStar      (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int rainbow);
void constellation    (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color, int rainbow);
void solidcolor       (CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color);
void solidcolorStrip  (CRGB_p *pLeds, unsigned int strip_idx, unsigned int strip_cnt, unsigned int led_cnt, CRGB color);

void closingLine(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color);

void publishCmd();
int processCloudCmd(String cmd);

char** cmdsplit(const char *str, const char *sep, int *argc, unsigned long int *val);
void cmdfree(char** str, int str_cnt);
unsigned long int strnum(char *str);

#endif /* __LESCALIERD_H__ */
