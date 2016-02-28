#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "application.h"

#define SUPPORT_CONTINOUS  (1 << 0)
#define SUPPORT_OPEN       (1 << 1)
#define SUPPORT_CLOSE      (1 << 2)
#define SUPPORT_ORDER      (1 << 3)
#define SUPPORT_RAINBOW    (1 << 4)

typedef void (*led_func_wo_t)(CRGB_p*, unsigned int, unsigned int, CRGB, int);
typedef void (*led_func_no_t)(CRGB_p*, unsigned int, unsigned int, CRGB);

typedef struct _light_mode_t {
  String        name;
  led_func_wo_t func;
  union {
    uint8_t raw;
    struct {
      uint8_t continous:1;
      uint8_t open     :1;
      uint8_t close    :1;
      uint8_t order    :1;
      uint8_t rainbow  :1;
    };
  } support;
} light_mode_t;

typedef struct _trigger_mode_t {
  uint32_t timer_timeout;
  uint8_t  manual      :1;
  uint8_t  timer_enable:1;
  uint8_t  modified    :1;
} trigger_mode_t;

typedef struct _main_mode_t {
  uint8_t       brightness:7;
  uint8_t       modified  :1;
  uint32_t      color     :31;
  uint32_t      random    :1;
  uint32_t      temperature;
  uint32_t      delay;
  const light_mode_t *mode;
} main_mode_t;

extern main_mode_t main_mode;
extern trigger_mode_t trigger_mode;

void runLightMode(const light_mode_t *mode, const led_strip_t strip, uint32_t color, int order);

#endif /* __LIGHT_H__ */
