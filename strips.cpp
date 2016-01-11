#include "strips.h"

SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);

/* LED array */
static CRGB leds[STRIP_CNT][LED_CNT] = {CRGB::Black};

void setup()
{
  int acc_error = 0;
  uint8_t acc_strip = 0;

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

  do
  {
    acc_error = acc_setup();
    if(acc_error != 0)
    {
#ifdef SERIAL_DEBUG
      Serial.print("Acc. initialization failed!!!");
#endif
      /* Blink strip close to failing acc */
      if     (acc_error == -1) acc_strip = 0;
      else if(acc_error == -2) acc_strip = STRIP_CNT - 1;
      memset(leds[acc_strip], 0xff, LED_CNT * sizeof(CRGB));
      FastLED.show();
      delay(5000);
      memset(leds[acc_strip], 0x00, LED_CNT * sizeof(CRGB));
      FastLED.show();
      delay(1000);
    }
  } while(acc_error != 0);

  acc_timer.start();

  Particle.function("open", setOpeningMode);
}

void loop()
{
  static CRGB_p *leds_ptr = leds;
  static uint32_t iter = 0;

  if(acc_detect_flag)
  {
    /* Save accelerometer status  */
    acc_read_flag = acc_detect_flag;
    /* Halt capture routine */
    acc_timer.stop();

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


    if     ((opening_mode & 0xff) == 0)     solidcolor(leds_ptr, STRIP_CNT, LED_CNT, CRGB::Black);
    else if((opening_mode & 0xff) == 1)     solidcolor(leds_ptr, STRIP_CNT, LED_CNT, (CRGB)opening_color);
    else if((opening_mode & 0xff) == 2) {
      if     (acc_read_flag & (1 << 1))     openingEpic(leds_ptr, STRIP_CNT, LED_CNT, TOP2BOTTOM);
      else if(acc_read_flag & (1 << 0))     openingEpic(leds_ptr, STRIP_CNT, LED_CNT, BOTTOM2TOP);
    }
    else if((opening_mode & 0xff) == 4) {
      if     (acc_read_flag & (1 << 1))     openingCenter(leds_ptr, STRIP_CNT, LED_CNT, TOP2BOTTOM);
      else if(acc_read_flag & (1 << 0))     openingCenter(leds_ptr, STRIP_CNT, LED_CNT, BOTTOM2TOP);
    }
    else if((opening_mode & 0xff) == 6) {
      if     (acc_read_flag & (1 << 1))     openingCone(leds_ptr, STRIP_CNT, LED_CNT, TOP2BOTTOM);
      else if(acc_read_flag & (1 << 0))     openingCone(leds_ptr, STRIP_CNT, LED_CNT, BOTTOM2TOP);
    }
    else if((opening_mode & 0xff) == 8)     openingStar(leds_ptr, STRIP_CNT, LED_CNT);
    //else if((opening_mode & 0xff) == 9)      constellation(leds_ptr, STRIP_CNT, LED_CNT);
    else if((opening_mode & 0xff) == 10)    solidcolor(leds_ptr, STRIP_CNT, LED_CNT, CRGB::White);
    //else if((opening_mode & 0xff) == 11)     acc_display(leds_ptr, STRIP_CNT, LED_CNT);
    else                                     opening(leds_ptr, STRIP_CNT, LED_CNT);
    if((opening_mode & 0xff) != 9 && (opening_mode & 0xff) != 11) delay(opening_delay * 1000);
    // TODO: create function for endings

    if((opening_mode & 0xff) != 9 && (opening_mode & 0xff) != 11) fadeOutAll(leds_ptr, STRIP_CNT, LED_CNT);
    //if     (opening_mode & 0x100)            opening_mode = random(1, 10) | 0x100;

    /* Restrat capture routine */
    acc_timer.start();

  }
  
  delay(500);

  /* Reset backed-up accelerometer status */
  if(acc_read_flag)
  {
    acc_read_flag = 0;
    iter = 0;
  }

  /* Switch off all strips if not used every 10 min */
  if(acc_read_flag == 0 && iter++ > 1200)
  {
    solidcolor(leds_ptr, STRIP_CNT, LED_CNT, CRGB::Black);
    FastLED.show();
    iter = 0;
  }
}
