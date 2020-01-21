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

#include "lescalierd.h"
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
  Serial.println("Ready to rumble!");
#endif
  /* Start capture timer */
  capture_timer.start();
  capture_running = true;
}

void loop()
{
  if(adc_detect_flag || (1 == main_mode.control.manual))
  {
    /* Halt capture routine */
    if(true == capture_running)
    {
      capture_timer.stop();
      capture_running = false;
#ifdef SERIAL_DEBUG
      Serial.println("Stop capture");
#endif
    }

    /* Set temperature */
    FastLED.setTemperature(main_mode.light.temperature);
    /* Set brightness */
    FastLED.setBrightness(main_mode.light.brightness);
    /* Set new color if random is enabled */
    if(1 == main_mode.light.random)
    {
      main_mode.light.color = random(0, 0xFFFFFF);
    }

    /* Run animation according to triggered sensor */
    if(ADC_1_ID_MASK == (adc_detect_flag & ADC_1_ID_MASK))
    {
      runAnimation(main_mode.animation, main_mode.strip, main_mode.light.color, main_mode.light.rainbow, BOTTOM2TOP);
    }
    else if(ADC_2_ID_MASK == (adc_detect_flag & ADC_2_ID_MASK))
    {
      runAnimation(main_mode.animation, main_mode.strip, main_mode.light.color, main_mode.light.rainbow, TOP2BOTTOM);
    }
    else
    {
      runAnimation(main_mode.animation, main_mode.strip, main_mode.light.color, main_mode.light.rainbow, TOP2BOTTOM);
    }

    /* Persistance delay */
    if(0 < main_mode.light.delay)
    {
      delay(main_mode.light.delay * 1000);
    }

    /* TODO: add support for closing */
    if(main_mode.animation->support.open)
    {
      fadeOutAll(main_mode.strip->leds, main_mode.strip->strip_cnt, main_mode.strip->led_cnt);
    }

    if((false == capture_running) && (0 == main_mode.control.manual))
    {
      /* Reset accelerometer detect flag  */
      adc_detect_flag = 0;
      /* Reset ADC capture */
      adc_reset();
      /* Restart capture routine */
#ifdef SERIAL_DEBUG
      Serial.println("Start capture");
#endif
      capture_timer.start();
    }
  }
  else if((false == capture_running) && (0 == main_mode.control.manual))
  {
    /* auto and capture stop -> need to restart capture */
    capture_timer.reset();
    capture_running = true;
  }

  delay(500);
}
