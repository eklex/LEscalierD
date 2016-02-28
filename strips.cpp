#include "strips.h"
#include "light.h"

SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);

/* LED array */
static CRGB leds[STRIP_CNT][LED_CNT] = {CRGB::Black};
static const led_strip_t led_strip = {leds, STRIP_CNT, LED_CNT};

void setup()
{
  int acc_error = 0;
  uint8_t acc_strip = 0;

  capture_timer.stop();

#ifdef SERIAL_DEBUG
  /* Set serial baud rate */
  Serial.begin(115200);
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
  FastLED.show();

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

  //Particle.function("open", setOpeningMode);
  Particle.function("cmd", processCloudCmd);


#ifdef SERIAL_DEBUG
  Serial.println("Ready to rumbule!");
#endif
  capture_timer.start();
}

void loop()
{
  static uint32_t iter = 0;

  if(acc_detect_flag || trigger_mode.manual)
  {
    /* Halt capture routine */
    capture_timer.stop();
    Serial.println("Stop capture");

    FastLED.setTemperature(main_mode.temperature);
    FastLED.setBrightness(main_mode.brightness);
    if(main_mode.random == 1) main_mode.color = random(0, 0xFFFFFF);

         if(acc_detect_flag & (1 << 1)) runLightMode(main_mode.mode, led_strip, main_mode.color, TOP2BOTTOM);
    else if(acc_detect_flag & (1 << 0)) runLightMode(main_mode.mode, led_strip, main_mode.color, BOTTOM2TOP);
    else                                runLightMode(main_mode.mode, led_strip, main_mode.color, TOP2BOTTOM);
    delay(main_mode.delay * 1000);
    if(main_mode.mode->support.open)
    {
      fadeOutAll(led_strip.leds, STRIP_CNT, LED_CNT);
    }
    
    /* Restart capture routine */
    acc_reset();
    /* Reset accelerometer detect flag  */
    acc_detect_flag = 0;

    if(trigger_mode.manual == 0)
    {
      Serial.println("Start capture");
      capture_timer.start();
    }


  }

  delay(500);
#if 0
  /* Reset backed-up accelerometer status */
  if(acc_read_flag)
  {
    acc_read_flag = 0;
    iter = 0;
  }
  else
  {
    iter++;
    /* Switch off all strips if not used every 1h */
    if(iter > 7200) /* = prd_ms / 500 */
    {
      acc_timer.stop();
      Serial.println("Reset to black");
      solidcolor(leds_ptr, STRIP_CNT, LED_CNT, CRGB::Black);
      FastLED.show();
      iter = 0;
      acc_timer.start();
    }
  }
#endif

}
