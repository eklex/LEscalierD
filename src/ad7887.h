#ifndef __AD7887_H__
#define __AD7887_H__

/* Mandatory include for Particle */
#include "application.h"
#include "debug.h"

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
