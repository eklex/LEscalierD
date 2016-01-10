#include "strips.h"

/* Accelerometer #1 */
MMA_7455 acc1        = MMA_7455(spi_protocol, ACC_1_CS);
volatile bool   acc1_pulse  = false;
volatile bool   led_pulse  = false;
bool     acc1_enable = true;

/* Accelerometer #2 */
MMA_7455 acc2        = MMA_7455(spi_protocol, ACC_2_CS);
volatile bool   acc2_pulse  = false;
bool     acc2_enable = true;


/* Accelerometer #1 Pulse Interrupt */
void acc1_isr_pulse(void)
{
  acc1_pulse = true;
  //led_pulse = true;
}

/* Accelerometer #2 Pulse Interrupt */
void acc2_isr_pulse(void)
{
  acc2_pulse = true;
  //opening_mode = random(1, 10) | 0x100;
}

int acc_init(
            MMA_7455 *acc,
            MODE acc_mode,
            PULSE_MODE pls_mode,
            TH_MODE th_mode,
            ISR_MODE isr_mode,
            uint16_t isr_pin,
            isr_func_t isr_func,
            int16_t x_off,
            int16_t y_off,
            int16_t z_off
            )
{
  /* Start accelerometer */
  acc->begin();

  /* Set accelerometer sensibility */
  if(acc_mode == measure)
  {
    acc->setSensitivity(2);
    if(acc->getSensitivity() != 2)  return -1;
  }
  else
  {
    acc->setSensitivity(8);
    if(acc->getSensitivity() != 8)  return -1;
  }

  /* Set accelerometer mode */
  acc->setMode(acc_mode);
  if(acc->getMode() != acc_mode)    return -2;

  /* Set axis offsets */
  /* Note: the offset is hardware specific
   * and defined thanks to the auto-calibration example. */
  acc->setAxisOffset(x_off, y_off, z_off);
  /* Enable the detection on each axis */
  acc->enableDetectionXYZ(true, true, true);
  /* Set pulse polarity */
  acc->setPulsePolarity(pls_mode);
  /* Set threshold mode */
  acc->setThresholdMode(th_mode);
  /* Set threshold limit */
  acc->setPulseThresholdLimit(35); /* 32 / 16N/g = 2g */
  /* Set max pulse duration */
  acc->setPulseDuration(100); /* 100 * 0.5ms = 50ms */
  /* Enable Interrupt pins */
  /* Note: This function disables pin DRDY and enable pin INT1 */
  acc->enableInterruptPins(true);
  /* Set interrupt mode */
  acc->setInterruptMode(isr_mode);
  /* Sanity clear of interrupts */
  acc->clearInterrupt();
  /* Set interrupt pin */
  pinMode(isr_pin, INPUT);
  /* Attach interrupt function */
  attachInterrupt(isr_pin, isr_func, RISING);

  return 0;
}
