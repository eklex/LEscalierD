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

static const anima_def_t animation_list[] =
{
  { "epicsides",  (anima_func_w2_t)openingEpicSides,  ANIMA_OPEN | ANIMA_ORDER | ANIMA_RAINBOW },
  { "epiccenter", (anima_func_w2_t)openingEpicCenter, ANIMA_OPEN | ANIMA_ORDER | ANIMA_RAINBOW },
  { "center",     (anima_func_w2_t)openingCenter,     ANIMA_OPEN | ANIMA_ORDER | ANIMA_RAINBOW },
  { "cone",       (anima_func_w2_t)openingCone,       ANIMA_OPEN | ANIMA_ORDER | ANIMA_RAINBOW },
  { "sides",      (anima_func_w2_t)openingSides,      ANIMA_OPEN | ANIMA_ORDER | ANIMA_RAINBOW },
  { "cascade",    (anima_func_w2_t)openingCascade,    ANIMA_OPEN | ANIMA_ORDER | ANIMA_RAINBOW },
  { "star",       (anima_func_w2_t)openingStar,       ANIMA_OPEN | ANIMA_CLOSE | ANIMA_RAINBOW },
  { "tree",       (anima_func_w2_t)openingTree,       ANIMA_OPEN | ANIMA_ORDER | ANIMA_RAINBOW },
  { "line",       (anima_func_w2_t)openingLine,       ANIMA_OPEN | ANIMA_CLOSE },
  { "const",      (anima_func_w2_t)constellation,     ANIMA_CONTINOUS | ANIMA_RAINBOW },
  { "solid",      (anima_func_w2_t)solidcolor,        ANIMA_OPEN },
};

main_control_t main_mode = {
  {                   /** light mode ********************/
    Candle,            /* temperature */
    45,                /* delay */
    0xffffff,          /* color value */
    1,                 /* random color */
    80,                /* brightness */
    0,                 /* rainbow */
  },
  {                  /** light control ******************/
    0,                /* manual */
    { 0, 0 }          /* timer: enable, value */
  },
  NULL,              /** led strip **********************/
  NULL               /** animation **********************/
};

char running_cmd[256];

void publishCmd()
{
  static char color[7] = {0};
  static uint8_t temp  = 0;
  if(main_mode.light.random) strcpy(color, "random");
  else sprintf(color, "%06x", main_mode.light.color);
  switch(main_mode.light.temperature)
  {
    case Candle:          temp = 1; break;
    case Tungsten40W:     temp = 2; break;
    case Tungsten100W:    temp = 3; break;
    case Halogen:         temp = 4; break;
    case CarbonArc:       temp = 5; break;
    case HighNoonSun:     temp = 6; break;
    case DirectSunlight:  temp = 7; break;
    case OvercastSky:     temp = 8; break;
    case ClearBlueSky:    temp = 9; break;
    default:              temp = 0; break;
  }
  //memset(running_cmd, 0, sizeof(running_cmd));
  sprintf(running_cmd, "effect:%s,color:%s,temp:%d,bright:%d,delay:%d,trigger:%d,rainbow:%d",
          main_mode.animation->name.c_str(),
          color,
          temp,
          main_mode.light.brightness,
          main_mode.light.delay,
          main_mode.control.manual,
          main_mode.light.rainbow);
}

int processCloudCmd(String cmd)
{
  const anima_def_t *animation = NULL;
  char *pstr = NULL;
  uint32_t temp = 0;
  int retVal = 0;

  animation = getAnimation(cmd);
  if(animation != NULL)
  {
    main_mode.animation = animation;
    retVal = 1;
  }
  else if(strstr(cmd, "delay:") != NULL)
  {
    pstr = strchr(cmd, ':') + 1;
    main_mode.light.delay = (uint32_t)strtol(pstr, NULL, 0);
    retVal = 2;
  }
  else if(strstr(cmd, "bright:") != NULL)
  {
    pstr = strchr(cmd, ':') + 1;
    main_mode.light.brightness = (uint32_t)strtol(pstr, NULL, 0);
    retVal = 3;
  }
  else if(strstr(cmd, "temp:") != NULL)
  {
    pstr = strchr(cmd, ':') + 1;
    temp = (uint32_t)strtol(pstr, NULL, 0);
    switch(temp)
    {
      case 1:
        main_mode.light.temperature = Candle;         break;
      case 2:
        main_mode.light.temperature = Tungsten40W;    break;
      case 3:
        main_mode.light.temperature = Tungsten100W;   break;
      case 4:
        main_mode.light.temperature = Halogen;        break;
      case 5:
        main_mode.light.temperature = CarbonArc;      break;
      case 6:
        main_mode.light.temperature = HighNoonSun;    break;
      case 7:
        main_mode.light.temperature = DirectSunlight; break;
      case 8:
        main_mode.light.temperature = OvercastSky;    break;
      case 9:
        main_mode.light.temperature = ClearBlueSky;   break;
      case 0:
      default:
        main_mode.light.temperature = UncorrectedTemperature;
        break;
    }
    retVal = 4;
  }
  else if(strstr(cmd, "color:") != NULL)
  {
    pstr = strchr(cmd, ':') + 1;
    if(strstr(pstr, "random") != NULL) main_mode.light.random = 1;
    else
    {
      main_mode.light.random = 0;
      main_mode.light.color = (uint32_t)strtol(pstr, NULL, 0);
    }
    retVal = 5;
  }
  else if(strstr(cmd, "trigger:") != NULL)
  {
    pstr = strchr(cmd, ':') + 1;
    if(strstr(cmd, "manual") != NULL) main_mode.control.manual = 1;
    else
    {
      main_mode.control.manual = 0;
    }
    retVal = 6;
  }
  else if(strstr(cmd, "rainbow:") != NULL)
  {
    pstr = strchr(cmd, ':') + 1;
    main_mode.light.rainbow = (uint8_t)strtol(pstr, NULL, 0);
    retVal = 7;
  }
  else if(cmd == "reset")
  {
    System.reset();
    retVal = 8;
  }
  else if(cmd == "pull")
  {
    retVal = 9;
  }
  publishCmd();
  return retVal;
}

const anima_def_t* getAnimation(String name)
{
  unsigned int index = 0;
  for(index = 0; index < sizeof(animation_list)/sizeof(animation_list[0]); index++)
  {
    if(animation_list[index].name == name)
    {
      return &animation_list[index];
    }
  }
  return NULL;
}

const anima_def_t* getAnimation(unsigned int index)
{
  if(index < sizeof(animation_list)/sizeof(animation_list[0]))
  {
    return &animation_list[index];
  }
  return NULL;
}

void runAnimation(const anima_def_t *animation, const led_strip_t *strip, uint32_t color, int rainbow, int order)
{
  /* Sanity check */
  if(animation == NULL || strip == NULL)
  {
    return;
  }

  if(animation->support.order && animation->support.rainbow)
  {
    ((anima_func_w2_t)animation->func)(strip->leds, strip->strip_cnt, strip->led_cnt, color, rainbow, order);
  }
  else if(animation->support.rainbow)
  {
    ((anima_func_w1_t)animation->func)(strip->leds, strip->strip_cnt, strip->led_cnt, color, rainbow);
  }
  else if(animation->support.order)
  {
    ((anima_func_w1_t)animation->func)(strip->leds, strip->strip_cnt, strip->led_cnt, color, order);
  }
  else
  {
    ((anima_func_no_t)animation->func)(strip->leds, strip->strip_cnt, strip->led_cnt, color);
  }
}
