#include "strips.h"
#include "light.h"

static const light_mode_t light_mode[] =
{
  { "epic",   (led_func_wo_t)openingEpic,   SUPPORT_OPEN | SUPPORT_ORDER | SUPPORT_RAINBOW },
  { "center", (led_func_wo_t)openingCenter, SUPPORT_OPEN | SUPPORT_ORDER | SUPPORT_RAINBOW },
  { "cone",   (led_func_wo_t)openingCone,   SUPPORT_OPEN | SUPPORT_ORDER | SUPPORT_RAINBOW },
  { "star",   (led_func_wo_t)openingStar,   SUPPORT_OPEN | SUPPORT_CLOSE | SUPPORT_RAINBOW },
  { "line",   (led_func_wo_t)openingLine,   SUPPORT_OPEN | SUPPORT_CLOSE },
  { "const",  (led_func_wo_t)constellation, SUPPORT_CONTINOUS },
  { "solid",  (led_func_wo_t)solidcolor,    SUPPORT_OPEN },
};

main_mode_t main_mode = {
  50,
  0,
  0xffffff,
  0,
  Candle,
  30,
  &light_mode[0]
};
trigger_mode_t trigger_mode;

static const light_mode_t* getLightMode(String name);

int processCloudCmd(String cmd)
{
  const light_mode_t *mode = NULL;
  char *pstr = NULL;
  uint32_t temp = 0;

  mode = getLightMode(cmd);
  if(mode != NULL)
  {
    main_mode.mode = mode;
    return 1;
  }
  else if(strstr(cmd, "delay:") != NULL)
  {
    pstr = strchr(cmd, ':') + 1;
    main_mode.delay = (uint32_t)strtol(pstr, NULL, 0);
    return 2;
  }
  else if(strstr(cmd, "bright:") != NULL)
  {
    pstr = strchr(cmd, ':') + 1;
    main_mode.brightness = (uint32_t)strtol(pstr, NULL, 0);
    return 3;
  }
  else if(strstr(cmd, "temp:") != NULL)
  {
    pstr = strchr(cmd, ':') + 1;
    temp = (uint32_t)strtol(pstr, NULL, 0);
    switch(temp)
    {
      case 1:
        main_mode.temperature = Candle;         break;
      case 2:
        main_mode.temperature = Tungsten40W;    break;
      case 3:
        main_mode.temperature = Tungsten100W;   break;
      case 4:
        main_mode.temperature = Halogen;        break;
      case 5:
        main_mode.temperature = CarbonArc;      break;
      case 6:
        main_mode.temperature = HighNoonSun;    break;
      case 7:
        main_mode.temperature = DirectSunlight; break;
      case 8:
        main_mode.temperature = OvercastSky;    break;
      case 9:
        main_mode.temperature = ClearBlueSky;   break;
      case 0:
      default:
        main_mode.temperature = UncorrectedTemperature;
        break;
    }
    return 4;
  }
  else if(strstr(cmd, "color:") != NULL)
  {
    pstr = strchr(cmd, ':') + 1;
    if(strstr(cmd, "random") != NULL) main_mode.random = 1;
    else
    {
      main_mode.random = 0;
      main_mode.color = (uint32_t)strtol(pstr, NULL, 0);
    }
    return 5;
  }
  else if(strstr(cmd, "trigger:") != NULL)
  {
    pstr = strchr(cmd, ':') + 1;
    if(strstr(cmd, "manual") != NULL) trigger_mode.manual = 1;
    else trigger_mode.manual = 0;
    return 6;
  }
  else if(cmd == "reset")
  {
    System.reset();
  }

  return 0;
}

static const light_mode_t* getLightMode(String name)
{
  unsigned int index = 0;
  for(index = 0; index < sizeof(light_mode)/sizeof(light_mode[0]); index++)
  {
    if(light_mode[index].name == name)
    {
      return &light_mode[index];
    }
  }
  return NULL;
}

void runLightMode(const light_mode_t *mode, const led_strip_t strip, uint32_t color, int order)
{
  /* Sanity check */
  if(mode == NULL) return;

  if(mode->support.order)
  {
    ((led_func_wo_t)mode->func)(strip.leds, strip.strip_cnt, strip.led_cnt, color, order);
  }
  else
  {
    ((led_func_no_t)mode->func)(strip.leds, strip.strip_cnt, strip.led_cnt, (CRGB)color);
  }
}
