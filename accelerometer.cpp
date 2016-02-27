#include "strips.h"

/**
 * Define private prototypes
 */
static int  acc_init_device(acc_bundle_t*, MMA_7455*, uint16_t);
static void acc_init_detect(acc_detect_t*, float);
static void acc_capture(void);
static int  process_sample(acc_detect_t*, int);

/* Accelerometers structures */
static MMA_7455 acc_device[ACC_CNT] = {
  MMA_7455(spi_protocol, ACC_1_CS),
  MMA_7455(spi_protocol, ACC_2_CS)
};
static acc_bundle_t acc[ACC_CNT];

/* Detect flag */
volatile uint8_t acc_detect_flag = 0;

/* Sofware timer for accelerometer capture */
Timer capture_timer(2, acc_capture, false);

static int acc_init_device(
  acc_bundle_t *acc,
  MMA_7455 *device,
  uint16_t pin_drdy
  )
{
  /* Initialize main structure */
  acc->device = device;
  acc->pin_drdy = pin_drdy;

  /* Start accelerometer */
  acc->device->begin();

  /* Set strong drive strength for MISO */
  acc->device->enableStrongDriveStrength(true);

  /* Set bandwidth to 125Hz */
  acc->device->setLowPassFilter(lpf_125hz);

  /* Set accelerometer sensibility */
  acc->device->setSensitivity(2);
  if(acc->device->getSensitivity() != 2) return -1;

  /* Set accelerometer mode */
  acc->device->setMode(measure);
  if(acc->device->getMode() != measure)  return -2;

  /* Enable DRDY pin */
  /* Note: This function enables pin DRDY and disables pin INT1 */
  acc->device->enableInterruptPins(false);
  /* Set data ready pin */
  pinMode(acc->pin_drdy, INPUT);

  return 0;
}

static void acc_init_detect(acc_detect_t *acc, float threshold)
{
  unsigned int i;

  for(i = 0; i < w + delta; i++) acc->sample_buff[i] = 0;
  for(i = 0; i < N; i++)         acc->cov_buff[i]    = 0;

  acc->sum_sample_last  = 0;
  acc->sum_sample_delta = 0;
  acc->sum_sample_prod  = 0;
  acc->sum_cov_sqrt     = 0;
  acc->sample_idx       = 0;
  acc->sample_last_idx  = delta;
  acc->sample_delta_idx = 0;
  acc->cov_idx          = 0;
  acc->threshold        = threshold;
  acc->initialized      = false;
}

int acc_setup(void)
{
  int error = 0;

  acc[0].initialized  = false;
  acc[0].timeout_drdy = 0;
  acc[1].initialized  = false;
  acc[1].timeout_drdy = 0;

  /* Initialize accelerometer detect structure */
  acc_init_detect(&acc[0].detect, th);
  acc_init_detect(&acc[1].detect, th);

  /* Initialize accelerometer #1 */
  error = acc_init_device(&acc[0], &acc_device[0], ACC_1_DRDY);
  if(error != 0)
  {
#ifdef SERIAL_DEBUG
    Serial.print("Error initializing Accelerometer #1\n");
    Serial.print("Expected : Read\n");
    Serial.print("55 : "); Serial.println(acc[0].device->readReg(WHOAMI_OFF), HEX);
    Serial.print("1D : "); Serial.println(acc[0].device->readReg(I2CAD_OFF),  HEX);
    Serial.print("AA : "); Serial.println(acc[0].device->readReg(USRINF_OFF), HEX);
#endif
    return -1;
  }
  acc[0].initialized = true;

  /* Initialize accelerometer #2 */
  error = acc_init_device(&acc[1], &acc_device[1], ACC_2_DRDY);
  if(error != 0)
  {
#ifdef SERIAL_DEBUG
    Serial.print("Error initializing Accelerometer #2\n");
    Serial.print("Expected : Read\n");
    Serial.print("55 : "); Serial.println(acc[0].device->readReg(WHOAMI_OFF), HEX);
    Serial.print("1D : "); Serial.println(acc[0].device->readReg(I2CAD_OFF),  HEX);
    Serial.print("AA : "); Serial.println(acc[0].device->readReg(USRINF_OFF), HEX);
#endif
    return -2;
  }
  acc[1].initialized = true;

  return 0;
}

static void acc_capture(void)
{
  static uint8_t acc_idx  = 0;
         int8_t  z_sample = 0;
         int8_t  detected = 0;
         int     acc_drdy = 0;

  /* Look at the data ready flag,
   * according to the current acc index */
  switch(acc_idx)
  {
    case 0:
    case 1:
      if(acc[acc_idx].initialized == false)
      {
        /* ignore capture
         * if accelerometer initialization did not run */
        return;
      }
      acc_drdy = pinReadFast(acc[acc_idx].pin_drdy);
      break;
    default:
      /* Invalid accelerometer */
      acc_drdy = 0;
      /* Reset index */
      acc_idx = 0;
      return;
      break;
  }

  /* Data ready for selected acc */
  if(acc_drdy == HIGH)
  {
    z_sample = acc[acc_idx].device->readAxis8('z');
    detected = process_sample(&acc[acc_idx].detect, z_sample);
    if(detected == 1)
    {
      acc_detect_flag |= (1 << acc_idx);
      //Serial.print("Detected on Acc#"); Serial.println(acc_idx);
    }
    /* Reset data ready timeout */
    acc[acc_idx].timeout_drdy = 0;
  }
  /**
   * Data not ready
   * When the data is not ready, the accelerometer can be stuck
   * or the interrupt was missed. So if the interrupt does not
   * happend after 750 checks (3 sec), we make sure ID register
   * is still valid, and we read the axis register to reset
   * the data ready interrupt.
   */
  else
  {
    acc[acc_idx].timeout_drdy++;
    if(acc[acc_idx].timeout_drdy > 750) /* 1 check = 4ms */
    {
      /* Reset timeout counter */
      acc[acc_idx].timeout_drdy = 0;

      Serial.print("Data ready timeout on Acc#"); Serial.println(acc_idx);
      acc_drdy = acc_init_device(&acc[acc_idx],
                                 acc[acc_idx].device,
                                 acc[acc_idx].pin_drdy);
      if(acc_drdy != 0 ||
         acc[acc_idx].device->readReg(WHOAMI_OFF) != 0x55)
      {
        Serial.print("Hardware error on Acc#"); Serial.println(acc_idx);
      }
    }
  }
  /* Increase accelerometer index */
  acc_idx = (acc_idx + 1) % ACC_CNT;
  return;
}

static int process_sample(acc_detect_t *acc, int sample_last)
{
  unsigned int i;
  int sample_last_oldest, sample_delta_oldest;
  int sample_delta;
  float cov, out;
  char buff[100];

  if(sample_last >= 0 || sample_last < -95) return -2;

  if(acc->initialized == false)
  {
    acc->sample_buff[acc->sample_idx] = sample_last;

    if(acc->sample_idx >= delta)
    {
      acc->sum_sample_delta += acc->sample_buff[acc->sample_idx - delta];
      acc->sum_sample_last  += acc->sample_buff[acc->sample_idx];
      acc->sum_sample_prod  += acc->sample_buff[acc->sample_idx] *
                               acc->sample_buff[acc->sample_idx - delta];
    }

    acc->sample_idx++;
    if(acc->sample_idx >= w + delta)
    {
      acc->sample_idx  = w + delta - 1;
      acc->initialized = true;
    }
    return -1;
  }

  sample_last_oldest  = acc->sample_buff[acc->sample_last_idx];
  sample_delta_oldest = acc->sample_buff[acc->sample_delta_idx];
  acc->sum_sample_last  -= sample_last_oldest;
  acc->sum_sample_delta -= sample_delta_oldest;
  acc->sum_sample_prod  -= sample_last_oldest*sample_delta_oldest;
  sample_delta = acc->sample_buff[acc->sample_idx];
  acc->sample_idx = (acc->sample_idx + 1) % (w + delta);

  acc->sample_buff[acc->sample_idx] = sample_last;
  acc->sum_sample_last += sample_last;
  acc->sum_sample_delta += sample_delta;
  acc->sum_sample_prod += sample_last*sample_delta;
  cov = (float)((float)acc->sum_sample_prod/w) -
        (float)((float)(acc->sum_sample_last*acc->sum_sample_delta)/(w*w));

  acc->sum_cov_sqrt += cov * cov;
  acc->sum_cov_sqrt -= acc->cov_buff[acc->cov_idx] * acc->cov_buff[acc->cov_idx];
  acc->cov_buff[acc->cov_idx] = cov;

  acc->sample_last_idx = (acc->sample_last_idx + 1) % (w + delta);
  acc->sample_delta_idx = (acc->sample_delta_idx + 1) % (w + delta);
  acc->cov_idx = (acc->cov_idx + 1) % N;

  out = acc->sum_cov_sqrt / N;
#if 0
  sprintf(buff, ";%x;%d;",
          (uint32_t)acc,
          acc->sample_buff[acc->sample_idx]);
  Serial.print(buff);
  Serial.print(out); Serial.print(";\n");
#endif
  if(out > acc->threshold) {
    sprintf(buff, ";%x;%d;",
            (uint32_t)acc,
            acc->sample_buff[acc->sample_idx]);
    Serial.print(buff);
    Serial.print(out); Serial.print(";\n");
    //init_acc_detect (acc, acc->threshold);
    //Serial.print("1;\n");
    return 1;
  }
  //Serial.print("0;\n");
  return 0;
}

void acc_display(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt)
{
  int strip_idx = 0;
  int led_idx   = 0;
  int16_t x, y, z;
  uint32_t color;

  for(strip_idx = 0; strip_idx < strip_cnt - 1; strip_idx++)
  {
    memset(pLeds[strip_idx], 0, led_cnt * sizeof(CRGB));
  }

  acc[0].device->readAxis10(&x, &y, &z);

#ifdef SERIAL_DEBUG
  Serial.print("X: ");    Serial.print(x, DEC);
  Serial.print("\tY: ");  Serial.print(y, DEC);
  Serial.print("\tZ: ");  Serial.println(z, DEC);
#endif
  if(acc_detect_flag)
  {
    color = random(0, 0xFFFFFF);
    for(led_idx = 0; led_idx < led_cnt; led_idx++)  pLeds[7][led_idx] = color;
  }
  for(led_idx = 0; led_idx < abs(x)%led_cnt; led_idx++)  pLeds[0][led_idx] = CRGB::White;
  for(led_idx = 0; led_idx < abs(y)%led_cnt; led_idx++)  pLeds[1][led_idx] = CRGB::White;
  for(led_idx = 0; led_idx < abs(z)%led_cnt; led_idx++)  pLeds[2][led_idx] = CRGB::White;
  FastLED.show();
}

void acc_reset(void)
{
  acc_init_detect(&acc[0].detect, acc[0].detect.threshold);
  acc_init_detect(&acc[1].detect, acc[1].detect.threshold);
}
