#include "strips.h"

void closingLine(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color)
{
  int   strip_idx = 0;
  int   led_idx   = 0;
  CRGB *led_p     = NULL;

  for(led_idx = 0; led_idx < led_cnt; led_idx++)
  {
    for(strip_idx = 0; strip_idx < strip_cnt; strip_idx++)
    {
      led_p = (strip_idx % 2) ? &pLeds[strip_idx][led_cnt-led_idx-1] : &pLeds[strip_idx][led_idx];
      *led_p = CRGB::Black;
    }
    FastLED.show();
    delay(10);
  }
}
