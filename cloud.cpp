#include "strips.h"
#include "animation.h"

static const anima_def_t animation_list[] =
{
  { "epic",   (anima_func_wo_t)openingEpic,   ANIMA_OPEN | ANIMA_ORDER | ANIMA_RAINBOW },
  { "center", (anima_func_wo_t)openingCenter, ANIMA_OPEN | ANIMA_ORDER | ANIMA_RAINBOW },
  { "cone",   (anima_func_wo_t)openingCone,   ANIMA_OPEN | ANIMA_ORDER | ANIMA_RAINBOW },
  { "star",   (anima_func_wo_t)openingStar,   ANIMA_OPEN | ANIMA_CLOSE | ANIMA_RAINBOW },
  { "line",   (anima_func_wo_t)openingLine,   ANIMA_OPEN | ANIMA_CLOSE },
  { "const",  (anima_func_wo_t)constellation, ANIMA_CONTINOUS },
  { "solid",  (anima_func_wo_t)solidcolor,    ANIMA_OPEN },
};

main_control_t main_mode = {
  {                   /** light mode ********************/
    Candle,            /* temperature */
    30,                /* delay */
    0xffffff,          /* color value */
    1,                 /* random color */
    50,                /* brightness */
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
  sprintf(running_cmd, "effect:%s,color:%s,temp:%d,bright:%d,delay:%d,trigger:%d",
          main_mode.animation->name.c_str(),
          color,
          temp,
          main_mode.light.brightness,
          main_mode.light.delay,
          main_mode.control.manual);
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
    if(strstr(cmd, "random") != NULL) main_mode.light.random = 1;
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
  else if(cmd == "reset")
  {
    System.reset();
    retVal = 7;
  }
  else if(cmd == "pull")
  {
    retVal = 8;
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

void runAnimation(const anima_def_t *animation, const led_strip_t *strip, uint32_t color, int order)
{
  /* Sanity check */
  if(animation == NULL || strip == NULL)
  {
    return;
  }

  if(animation->support.order)
  {
    ((anima_func_wo_t)animation->func)(strip->leds, strip->strip_cnt, strip->led_cnt, color, order);
  }
  else
  {
    ((anima_func_no_t)animation->func)(strip->leds, strip->strip_cnt, strip->led_cnt, color);
  }
}
