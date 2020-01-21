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

#include "ad7887.h"

/**
 * Define hardware specs
 */
#define AD7887_REF_DIS      (1 << 5)  /* on-chip reference disable */
#define AD7887_DUAL         (1 << 4)  /* dual-channel mode */
#define AD7887_CH_AIN1      (1 << 3)  /* convert on channel 1, DUAL=1 */
#define AD7887_CH_AIN0      (0 << 3)  /* convert on channel 0, DUAL=0,1 */
#define AD7887_PM_MODE1     (0)       /* CS based shutdown */
#define AD7887_PM_MODE2     (1)       /* full on */
#define AD7887_PM_MODE3     (2)       /* auto shutdown after conversion */
#define AD7887_PM_MODE4     (3)       /* standby mode */

#define ADC_CTL_REG         (AD7887_PM_MODE1 | AD7887_CH_AIN0 | AD7887_REF_DIS)

/**
 * Define private constants
 */
static const float        adc_th_value   = 3500;
static const unsigned int adc_th_len     = 30;
static const unsigned int adc_sample_cnt = 50;

/**
 * Define private structures
 */
typedef struct _adc_detect_t {
  unsigned int  sample_buff[adc_sample_cnt];
  unsigned int  sample_sum;
  unsigned int  sample_idx;
  float         th_value;
  unsigned int  th_len;
  unsigned int  th_idx;
  unsigned int  pin_cs;
} adc_detect_t;

/**
 * Define private variables
 */
/* ADC structure */
static adc_detect_t adc[ADC_CNT];

/**
 * Define private prototypes
 */
/* Periodic callback */
static void adc_capture(void);
/* Read ADC value */
static uint16_t adc_read(unsigned int id);
/* Initialize ADC structure */
static void adc_init(adc_detect_t *adc_p);
/* Process ADC value */
static int8_t adc_process(adc_detect_t *adc_p, uint16_t adc_val);

/**
 * Define public variables
 */
/* Detect flag */
volatile uint8_t adc_detect_flag = 0;
/* Software timer for accelerometer capture */
Timer capture_timer(5, adc_capture, false);

int adc_setup(void)
{
  unsigned int idx;

  /* Setup SPI bus */
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setDataMode(SPI_MODE3);

  /* Setup Chip Select pin */
  if(ADC_CNT > 0) adc[0].pin_cs = ADC_1_CS;
  if(ADC_CNT > 1) adc[1].pin_cs = ADC_2_CS;
  if(ADC_CNT > 2) adc[2].pin_cs = ADC_3_CS;
  for(idx = 0; idx < ADC_CNT; idx++)
  {
    pinMode(adc[idx].pin_cs, OUTPUT);
    digitalWrite(adc[idx].pin_cs, HIGH);

    /* Setup ADC threshold */
    adc[idx].th_value = adc_th_value;
    adc[idx].th_len   = adc_th_len;
  }

  /* Reset capture variables */
  adc_reset();

  return(0);
}

void adc_reset(void)
{
  unsigned int idx;
  for(idx = 0; idx < ADC_CNT; idx++)
  {
    adc_init(&adc[idx]);
  }
}

static uint16_t adc_read(unsigned int id)
{
  uint8_t  lsb = 0;
  uint8_t  msb = 0;
  uint16_t val = 0;

  /* Sanity check */
  id = id % ADC_CNT;

  /* Read ADC */
  digitalWrite(adc[id].pin_cs, LOW);
  msb = SPI.transfer(ADC_CTL_REG);
  lsb = SPI.transfer(0x00);
  digitalWrite(adc[id].pin_cs, HIGH);

  /* Assemble the 12-bit value */
  val  = msb;
  val <<= 8;
  val |= lsb;

  return(val);
}

static void adc_init(adc_detect_t *adc_p)
{
  unsigned int idx;

  for(idx = 0; idx < adc_sample_cnt; idx++)
  {
    adc_p->sample_buff[idx] = 0;
  }
  adc_p->sample_idx    = 0;
  adc_p->sample_sum    = 0;
  adc_p->th_idx        = 0;
}

static void adc_capture(void)
{
  static uint8_t adc_idx  = 0;
  uint16_t adc_val;
  int8_t detect;

  /* Read ADC value */
  adc_val = adc_read(adc_idx);
  /* Process ADC value */
  detect = adc_process(&adc[adc_idx], adc_val);
  if(1 == detect)
  {
    adc_detect_flag |= (1 << adc_idx);
  }

  /* Increase ADC index */
  adc_idx = (adc_idx + 1) % ADC_CNT;
  return;
}

static int8_t adc_process(adc_detect_t *adc_p, uint16_t adc_val)
{
  float sample_avg;
  /* Remove oldest sample from sum */
  adc_p->sample_sum -= adc_p->sample_buff[adc_p->sample_idx];
  /* Add lastest sample to sum */
  adc_p->sample_sum += adc_val;
  /* Save lastest sample */
  adc_p->sample_buff[adc_p->sample_idx] = adc_val;
  /* Increase sample buffer index */
  adc_p->sample_idx = (adc_p->sample_idx + 1) % adc_sample_cnt;
  /* Get and compare sample average */
  sample_avg = (float)adc_p->sample_sum / (float)adc_sample_cnt;
  if(sample_avg > adc_p->th_value)
  {
    adc_p->th_idx += 1;
  }
  else
  {
    adc_p->th_idx = 0;
  }

#if defined(SERIAL_DEBUG) && defined(ADC_DEBUG)
  Serial.print("idx;"); Serial.print(adc_p->sample_idx);
  Serial.print(";\tadc;"); Serial.print(adc_val);
  Serial.print(";\tsum;"); Serial.print(adc_p->sample_sum);
  Serial.print(";\tavg;"); Serial.print(sample_avg);
  Serial.print(";\tcnt;"); Serial.print(adc_p->th_idx);
  Serial.println(";");
#endif

  if(adc_p->th_idx > adc_p->th_len)
  {
    adc_init(adc_p);
    return(1);
  }
  return(0);
}
