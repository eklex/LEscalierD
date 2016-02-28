#ifndef __ACCELERO_H__
#define __ACCELERO_H__

/* Mandatory include for Particle */
#include "application.h"

/**
 * Define public variables
 */
/* Capture timer for accelerometer */
extern          Timer   capture_timer;
/* Accelerometer detection flag */
extern volatile uint8_t acc_detect_flag;

/**
 * Define public prototypes
 */
/* Initialization of the accelerometers */
int  acc_setup(void);
/* Reset detection structure */
void acc_reset(void);

#endif /* __ACCELERO_H__ */
