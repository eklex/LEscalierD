#include "strips.h"

/* LED array */
static CRGB         leds[STRIP_CNT][LED_CNT]    = {CRGB::Black};
/* LED pointer */
static CRGB_p       *leds_ptr                   = leds;
/* Strip index */
static unsigned int strip_idx                   = 0;
/* LED index */
static unsigned int led_idx                     = 0;

/* Misc. */
static int error = 0;
#ifdef ACC_PULL
int8_t x8, y8, z8;
bool xpls, ypls, zpls;
#endif

SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);

void setup()
{
  delay(5000);
#ifdef SERIAL_DEBUG
  /* Set serial baud rate */
  Serial.begin(9600);
#endif
  /* Initialize led arrays */
  FastLED.addLeds<WS2812, STRIP_1, GRB>(leds[0], LED_CNT);
  FastLED.addLeds<WS2812, STRIP_2, GRB>(leds[1], LED_CNT);
  FastLED.addLeds<WS2812, STRIP_3, GRB>(leds[2], LED_CNT);
  FastLED.addLeds<WS2812, STRIP_4, GRB>(leds[3], LED_CNT);
  FastLED.addLeds<WS2812, STRIP_5, GRB>(leds[4], LED_CNT);
  FastLED.addLeds<WS2812, STRIP_6, GRB>(leds[5], LED_CNT);
  FastLED.addLeds<WS2812, STRIP_7, GRB>(leds[6], LED_CNT);
  FastLED.addLeds<WS2812, STRIP_8, GRB>(leds[7], LED_CNT);

  /* Initialize accelerometer #1 */
  error = acc_init(&acc1,
                   pulse,
                   pls_positive,
                   th_absolute,
                   pulse_pulse,
                   ACC_1_ISR,
                   acc1_isr_pulse,
                   -4, 28, -38);
  if(error != 0)
  {
#ifdef SERIAL_DEBUG
    Serial.print("Error initializing Accelerometer #1\n");
#endif
    acc1_enable = false;
  }
  /* Initialize accelerometer #2 */
  error = acc_init(&acc2,
                   pulse,
                   pls_positive,
                   th_absolute,
                   pulse_pulse,
                   ACC_2_ISR,
                   acc2_isr_pulse,
                   -10, 40, -38);
  if(error != 0)
  {
#ifdef SERIAL_DEBUG
    Serial.print("Error initializing Accelerometer #2\n");
#endif
    acc2_enable = false;
  }

  Particle.function("open", setOpeningMode);
  //Particle.variable("mode", &opening_mode, INT);
}

void loop()
{
  if(opening_temperature == true)
  {
    FastLED.setTemperature(Candle);
    FastLED.setBrightness(50);
  }
  else
  {
    FastLED.setTemperature(UncorrectedTemperature);
    FastLED.setBrightness(100);
  }


  if((opening_mode & 0xff) != 9 && (opening_mode & 0xff) != 11)           fadeOutAll(leds_ptr, STRIP_CNT, LED_CNT);
  //if     (opening_mode & 0x100)            opening_mode = random(1, 10) | 0x100;

  if     ((opening_mode & 0xff) == 0)      solidcolor(leds_ptr, STRIP_CNT, LED_CNT, CRGB::Black);
  else if((opening_mode & 0xff) == 1)      solidcolor(leds_ptr, STRIP_CNT, LED_CNT, (CRGB)opening_color);
  else if((opening_mode & 0xff) == 2)      openingEpic(leds_ptr, STRIP_CNT, LED_CNT, TOP2BOTTOM);
  else if((opening_mode & 0xff) == 3)      openingEpic(leds_ptr, STRIP_CNT, LED_CNT, BOTTOM2TOP);
  else if((opening_mode & 0xff) == 4)      openingCenter(leds_ptr, STRIP_CNT, LED_CNT, TOP2BOTTOM);
  else if((opening_mode & 0xff) == 5)      openingCenter(leds_ptr, STRIP_CNT, LED_CNT, BOTTOM2TOP);
  else if((opening_mode & 0xff) == 6)      openingCone(leds_ptr, STRIP_CNT, LED_CNT, TOP2BOTTOM);
  else if((opening_mode & 0xff) == 7)      openingCone(leds_ptr, STRIP_CNT, LED_CNT, BOTTOM2TOP);
  else if((opening_mode & 0xff) == 8)      openingStar(leds_ptr, STRIP_CNT, LED_CNT);
  else if((opening_mode & 0xff) == 9)      constellation(leds_ptr, STRIP_CNT, LED_CNT);
  else if((opening_mode & 0xff) == 10)     solidcolor(leds_ptr, STRIP_CNT, LED_CNT, CRGB::White);
  else if((opening_mode & 0xff) == 11)     acc_display(leds_ptr, STRIP_CNT, LED_CNT);
  else                                     opening(leds_ptr, STRIP_CNT, LED_CNT);
  if((opening_mode & 0xff) != 9 && (opening_mode & 0xff) != 11) delay(opening_delay * 1000);
  // TODO: create function for endings

  if(acc1_pulse)
  {
#ifdef SERIAL_DEBUG
    acc1.readAxis8(&x8, &y8, &z8);
    Serial.print("INT1");
    Serial.print("\tX: ");  Serial.print(x8, DEC);
    Serial.print("\tY: ");  Serial.print(y8, DEC);
    Serial.print("\tZ: ");  Serial.print(z8, DEC);
    acc1.getPulseDetection(&xpls, &ypls, &zpls);
    /* Display which axis triggered the interrupt */
    if(xpls)  Serial.print("\tXpls");
    if(ypls)  Serial.print("\tYpls");
    if(zpls)  Serial.print("\tZpls");
    Serial.println();
    //delay(500);
#endif
    acc1_pulse = false;
    acc1.clearInterrupt();
    acc1.clearInterrupt();
  }
  if(acc2_pulse)
  {
#ifdef SERIAL_DEBUG
    Serial.print("INT2\n");
#endif
    acc2_pulse = false;
    acc2.clearInterrupt();
    acc2.clearInterrupt();
  }
}
