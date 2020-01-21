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

#ifndef __AD7887_H__
#define __AD7887_H__

/* Mandatory include for Particle */
#include "application.h"
#include "debug.h"

/**
 * Define ADC mask
 */
typedef enum _ADC_ID_MASK {
  ADC_1_ID_MASK = (1 << 0),
  ADC_2_ID_MASK = (1 << 1),
  ADC_N_ID_MASK = (ADC_1_ID_MASK | ADC_2_ID_MASK)
} ADC_ID_MASK;

/**
 * Define hardware specs
 */
/* ADC count */
#define ADC_CNT     (2)
/* ADC #1 Chip Select */
#define ADC_1_CS    (A0)
/* ADC #2 Chip Select */
#define ADC_2_CS    (A1)
/* ADC #3 Chip Select */
#define ADC_3_CS    (A2)

/**
 * Define public variables
 */
/* Capture timer for accelerometer */
extern          Timer   capture_timer;
/* Accelerometer detection flag */
extern volatile uint8_t adc_detect_flag;

/**
 * Define public prototypes
 */
/* Initialization of the ADC */
int  adc_setup(void);
/* Reset ADC capture */
void adc_reset(void);

#endif /* __AD7887_H__ */
