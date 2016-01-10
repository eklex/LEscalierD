#include "strips.h"

uint16_t opening_mode = 11;
uint32_t opening_color = 0x0;
uint32_t opening_delay = 3;
bool     opening_temperature = true;


int setOpeningMode(String cmd)
{
  char *pstr = NULL;
  if(cmd == "black")        opening_mode = 0;
  else if(cmd == "epic" ||
          cmd == "epic1")   opening_mode = 2;
  else if(cmd == "epic2")   opening_mode = 3;
  else if(cmd == "center" ||
          cmd == "center1") opening_mode = 4;
  else if(cmd == "center2") opening_mode = 5;
  else if(cmd == "cone" ||
          cmd == "cone1")   opening_mode = 6;
  else if(cmd == "cone2")   opening_mode = 7;
  else if(cmd == "star")    opening_mode = 8;
  else if(cmd == "const")   opening_mode = 9;
  else if(cmd == "white")   opening_mode = 10;
  else if(cmd == "acc")     opening_mode = 11;
  else if(cmd == "random")  opening_mode = 0x100;
  else if(cmd == "reset")   System.reset();
  else if(strstr(cmd, "delay:") != NULL)
  {
    pstr = strchr(cmd, ':') + 1;
    opening_delay = (uint32_t)strtol(pstr, NULL, 0);
  }
  else if(strstr(cmd, "temp:") != NULL)
  {
    if(strstr(cmd, "true") != NULL) opening_temperature = true;
    else                            opening_temperature = false;
  }
  else
  {
    opening_color = (uint32_t)strtol(cmd, NULL, 0);
    opening_mode = 1;
  }

#ifdef SERIAL_DEBUG
  //Serial.print("%s = %d\n", cmd, opening_mode);
#endif
  return opening_mode;
}

static light_mode_t light_mode[] =
{
  { "star", openingStar, SUPP_OPEN, RAND_ONCE, 0xFFFFFF },
  { "acc", acc_display, SUPP_CONTINOUS, 0, 0xFFFFFF },
  { "const", constellation, SUPP_CONTINOUS, 0, 0xFFFFFF },
};
