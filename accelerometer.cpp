#include "strips.h"

/**
 * Define private prototypes
 */
static void acc1_drdy_isr(void);
static void acc2_drdy_isr(void);
static int  acc_init(MMA_7455*, uint16_t, isr_func_t);
static void acc_capture(void);

/* Accelerometers strctures */
static MMA_7455 acc[ACC_CNT] = {
  MMA_7455(spi_protocol, ACC_1_CS),
  MMA_7455(spi_protocol, ACC_2_CS)
};

/* Data Ready flag */
volatile bool acc1_drdy = true;
volatile bool acc2_drdy = true;
/* flag modified by timer routine */
volatile uint8_t acc_detect_flag = 0;
/* flag modified by main routine */
volatile uint8_t acc_read_flag   = 0;

/* Sofware timer for accelerometer capture */
Timer acc_timer(2, acc_capture, false);

/* Accelerometer #1 Data Ready Interrupt */
static void acc1_drdy_isr(void)
{
  if(!acc1_drdy) acc1_drdy = true;
}

/* Accelerometer #2 Data Ready Interrupt */
static void acc2_drdy_isr(void)
{
  if(!acc2_drdy) acc2_drdy = true;
}

static int acc_init(
  MMA_7455 *acc,
  uint16_t isr_pin,
  isr_func_t isr_func
  )
{
  /* Start accelerometer */
  acc->begin();

  /* Set strong drive strength for MISO */
  acc->enableStrongDriveStrength(true);
  /* Set bandwidth to 125Hz */
  acc->setLowPassFilter(lpf_125hz);
  /* Set accelerometer sensibility */
  acc->setSensitivity(2);
  if(acc->getSensitivity() != 2) return -1;

  /* Set accelerometer mode */
  acc->setMode(measure);
  if(acc->getMode() != measure)  return -2;

  /* Enable DRDY pin */
  /* Note: This function enables pin DRDY and disables pin INT1 */
  acc->enableInterruptPins(false);
  /* Set interrupt pin */
  pinMode(isr_pin, INPUT);
  /* Attach interrupt function */
  attachInterrupt(isr_pin, isr_func, RISING);

  return 0;
}

static void acc_capture()
{
  static const float movavg_rc = 0.01;
  static const float lpf_rc = 0.5;

  static struct
  {
    int8_t value;
    float newf;
    float oldf;
    float avg;
    float lpf;
  } acc_cap[ACC_CNT];
#if 0
  static int8_t  z_new[ACC_CNT] = {0};
  static int8_t  z_old[ACC_CNT] = {0};
  static float   avgf[ACC_CNT]  = {0};
  static int8_t  avg8[ACC_CNT]  = {0};
#endif
  static uint8_t drdy_timeout[ACC_CNT] = {0};
  volatile bool *acc_drdy      = NULL;
  uint8_t acc_idx;

  /* reset detect flag when it has been read */
  acc_detect_flag &= ~acc_read_flag;

  for(acc_idx = 0; acc_idx < ACC_CNT; acc_idx++)
  {
    switch(acc_idx)
    {
      case 0:  acc_drdy = &acc1_drdy; break;
      case 1:  acc_drdy = &acc2_drdy; break;
      default: acc_drdy = NULL;       break;
    }

    if(acc_drdy != NULL && *acc_drdy == true)
    {
      acc_cap[acc_idx].value = acc[acc_idx].readAxis8('z');
      *acc_drdy = false;

      /* Sanity check to avoid invalid value */
      if((uint8_t)acc_cap[acc_idx].value == 0x00 ||
         (uint8_t)acc_cap[acc_idx].value == 0x7F ||
         (uint8_t)acc_cap[acc_idx].value == 0x80) return;

        /* Calculate the moving average */
      acc_cap[acc_idx].avg = movavg_rc*(float)acc_cap[acc_idx].value +
                            (1 - movavg_rc)*acc_cap[acc_idx].avg;
      /* Low-pass filter */
      acc_cap[acc_idx].lpf = lpf_rc*(float)acc_cap[acc_idx].value +
                            (1 - lpf_rc)*acc_cap[acc_idx].oldf;
      /* Cap value if too close to moving average */
      acc_cap[acc_idx].newf =
        (acc_cap[acc_idx].lpf < acc_cap[acc_idx].avg + 1.0 &&
         acc_cap[acc_idx].lpf > acc_cap[acc_idx].avg - 1.0) ?
         acc_cap[acc_idx].avg : acc_cap[acc_idx].lpf;

      /* Detect step */
      if(acc_cap[acc_idx].newf <= acc_cap[acc_idx].avg - 1.5 &&
         acc_cap[acc_idx].oldf >= acc_cap[acc_idx].avg + 1.5 &&
         acc_cap[acc_idx].oldf - acc_cap[acc_idx].newf >= 4)
      {
        acc_detect_flag |= (1 << acc_idx);
        Serial.print("Detected on Acc#"); Serial.println(acc_idx);
        Serial.print(acc_cap[acc_idx].oldf, DEC); Serial.print(" ");
        Serial.print(acc_cap[acc_idx].newf, DEC); Serial.print(" ");
        Serial.println(acc_cap[acc_idx].avg, DEC);
      }
      acc_cap[acc_idx].oldf = acc_cap[acc_idx].newf;
      drdy_timeout[acc_idx] = 0;
    }
    else if(acc_drdy != NULL)
    {
      drdy_timeout[acc_idx]++;
      if(drdy_timeout[acc_idx] > 128)
      {
        drdy_timeout[acc_idx] = 0;
        if(acc[acc_idx].readReg(WHOAMI_OFF) == 0x55)
        {
          Serial.print("ISR hung on Acc#"); Serial.println(acc_idx);
          acc[acc_idx].readAxis8('z');
          *acc_drdy = false;
        }
        else
        {
          Serial.print("Error on Acc#");
          Serial.println(acc_idx);
        }
      }
    }
  }
}

int acc_setup(void)
{
  int error = 0;

  /* Initialize accelerometer #1 */
  error = acc_init(&acc[0], ACC_1_ISR, acc1_drdy_isr);
  if(error != 0)
  {
#ifdef SERIAL_DEBUG
    Serial.print("Error initializing Accelerometer #1\n");
    Serial.print("Expected : Read\n");
    Serial.print("55 : "); Serial.println(acc[0].readReg(WHOAMI_OFF), HEX);
    Serial.print("1D : "); Serial.println(acc[0].readReg(I2CAD_OFF),  HEX);
    Serial.print("AA : "); Serial.println(acc[0].readReg(USRINF_OFF), HEX);
#endif
    return -1;
  }

  /* Initialize accelerometer #2 */
  error = acc_init(&acc[1], ACC_2_ISR, acc2_drdy_isr);
  if(error != 0)
  {
#ifdef SERIAL_DEBUG
    Serial.print("Error initializing Accelerometer #2\n");
    Serial.print("Expected : Read\n");
    Serial.print("55 : "); Serial.println(acc[0].readReg(WHOAMI_OFF), HEX);
    Serial.print("1D : "); Serial.println(acc[0].readReg(I2CAD_OFF),  HEX);
    Serial.print("AA : "); Serial.println(acc[0].readReg(USRINF_OFF), HEX);
#endif
    return -2;
  }

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

  acc[0].readAxis10(&x, &y, &z);

#ifdef SERIAL_DEBUG
  Serial.print("X: ");    Serial.print(x, DEC);
  Serial.print("\tY: ");  Serial.print(y, DEC);
  Serial.print("\tZ: ");  Serial.println(z, DEC);
#endif
  if(acc_read_flag)
  {
    color = random(0, 0xFFFFFF);
    for(led_idx = 0; led_idx < led_cnt; led_idx++)  pLeds[7][led_idx] = color;
  }
  for(led_idx = 0; led_idx < abs(x)%led_cnt; led_idx++)  pLeds[0][led_idx] = CRGB::White;
  for(led_idx = 0; led_idx < abs(y)%led_cnt; led_idx++)  pLeds[1][led_idx] = CRGB::White;
  for(led_idx = 0; led_idx < abs(z)%led_cnt; led_idx++)  pLeds[2][led_idx] = CRGB::White;
  FastLED.show();
}
