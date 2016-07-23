#include "strips.h"
#include "animation.h"
#include "leds.h"
#include "ad7887.h"

SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);

bool capture_running = false;
extern char running_cmd[256];

void setup()
{
  capture_timer.stop();

#ifdef SERIAL_DEBUG
  /* Set serial baud rate */
  Serial.begin(115200);
#endif

  /* Initialize LED strips */
  main_mode.strip = led_setup();

  /* Initialize ADC */
  adc_setup();

  /* Initialize default animation */
  main_mode.animation = getAnimation("sides");
  /* Register cloud function */
  Particle.function("cmd", processCloudCmd);
  Particle.variable("pull", running_cmd);
  publishCmd();

#ifdef SERIAL_DEBUG
  Serial.println("Ready to rumbule!");
#endif
  capture_timer.start();
  capture_running = true;
}

void loop()
{
  if(adc_detect_flag || main_mode.control.manual)
  {
    /* Halt capture routine */
    if(capture_running)
    {
      capture_timer.stop();
      capture_running = false;
#ifdef SERIAL_DEBUG
      Serial.println("Stop capture");
#endif
    }

    FastLED.setTemperature(main_mode.light.temperature);
    FastLED.setBrightness(main_mode.light.brightness);
    if(main_mode.light.random == 1) main_mode.light.color = random(0, 0xFFFFFF);

         if(adc_detect_flag & (1 << 1)) runAnimation(main_mode.animation, main_mode.strip, main_mode.light.color, TOP2BOTTOM);
    else if(adc_detect_flag & (1 << 0)) runAnimation(main_mode.animation, main_mode.strip, main_mode.light.color, BOTTOM2TOP);
    else                                runAnimation(main_mode.animation, main_mode.strip, main_mode.light.color, TOP2BOTTOM);
    delay(main_mode.light.delay * 1000);
    if(main_mode.animation->support.open)
    {
      fadeOutAll(main_mode.strip->leds, main_mode.strip->strip_cnt, main_mode.strip->led_cnt);
    }

    if(capture_running == false && main_mode.control.manual == 0)
    {
      /* Reset accelerometer detect flag  */
      adc_detect_flag = 0;
      /* Reset ADC capture */
      adc_reset();
      /* Restart capture routine */
      Serial.println("Start capture");
      capture_timer.start();
    }
  }
  else if(capture_running == false && main_mode.control.manual == 0)
  {
    /* auto and capture stop -> need to restart capture */
    capture_timer.reset();
    capture_running = true;
  }

  delay(500);
}
