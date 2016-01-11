#ifndef __STRIPS_H__
#define __STRIPS_H__

#include "application.h"
#include "FastLED.h"
#include "MMA_7455.h"
FASTLED_USING_NAMESPACE;

#undef RAINBOW
#define SERIAL_DEBUG
#undef ACC_PULL

/* Number of strips */
#define STRIP_CNT   (8)
/* Number of LEDs per strip */
#define LED_CNT     (45)

/* Pin assigned per strip */
#define STRIP_1     (D6)
#define STRIP_2     (D7)
#define STRIP_3     (D4)
#define STRIP_4     (D3)
#define STRIP_5     (D2)
#define STRIP_6     (D1)
#define STRIP_7     (D5)
#define STRIP_8     (D0)

/* Accelerometer count */
#define ACC_CNT     (2)
/* Accelerometer #1 Chip Select */
#define ACC_1_CS    (A2)
/* Accelerometer #1 Interrupt */
#define ACC_1_ISR   (A6)
/* Accelerometer #2 Chip Select */
#define ACC_2_CS    (A1)
/* Accelerometer #2 Interrupt */
#define ACC_2_ISR   (A7)

/* Display rules */
#define TOP2BOTTOM  (-1)
#define BOTTOM2TOP  (1)

typedef CRGB CRGB_p[LED_CNT];
typedef void (*isr_func_t)(void);
typedef void (*led_func_wo_t)(CRGB_p*, unsigned int, unsigned int, int);
typedef void (*led_func_no_t)(CRGB_p*, unsigned int, unsigned int);


extern uint16_t opening_mode;
extern uint32_t opening_color;
extern uint32_t opening_delay;
extern bool     opening_temperature;

#define SUPP_CONTINOUS  (1 << 0)
#define SUPP_OPEN       (1 << 1)
#define SUPP_CLOSE      (1 << 2)
#define SUPP_ORDER      (1 << 3)
#define RAND_EN         (1 << 0)
#define RAND_ONCE       (1 << 1)

typedef struct _light_mode_t
{
  String name;
  union {
    //led_func_wo_t funcw;
    led_func_no_t func;
  };
  union {
    uint8_t raw;
    struct {
      uint8_t continous:1;
      uint8_t open     :1;
      uint8_t close    :1;
      uint8_t order    :1;
    };
  } support;
  struct {
    union {
      uint8_t raw;
      struct {
        uint8_t enable:1;
        uint8_t once  :1;
      };
    } random;
    uint32_t value:24;
  } color;
} light_mode_t;

typedef struct _trigger_mode_t
{
  uint32_t timer_timeout;
  uint8_t  manual      :1;
  uint8_t  timer_enable:1;
  uint8_t  modified    :1;
} trigger_mode_t;

typedef struct _main_mode_t
{
  uint8_t      brightness:7;
  uint8_t      modified  :1;
  uint32_t     color;
  uint32_t     temperature;
  uint32_t     delay;
  light_mode_t mode;
} main_mode_t;

typedef struct _cloudcmd_t
{
  char *rawcmd;
  char **argv;
  int  argc;
} cloudcmd_t;

//extern          MMA_7455 acc[ACC_CNT];
extern volatile bool     acc1_drdy;
extern volatile bool     acc2_drdy;
extern volatile uint8_t  acc_detect_flag;
extern volatile uint8_t  acc_read_flag;
extern          Timer    acc_timer;
extern          Timer    bkgnd_timer;

int acc_setup(void);
void acc_display(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt);

void fadeIn(CRGB_p *pLeds, unsigned int strip_idx, unsigned int led_idx, CRGB color);
void fadeOut(CRGB_p *pLeds, unsigned int strip_idx, unsigned int led_idx, CRGB color);
void fadeOut(CRGB_p *pLeds, unsigned int strip_idx, unsigned int led_idx);
void fadeOutAll(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, unsigned int scale = 2);
char** cmdsplit(const char *str, const char *sep, int *argc, unsigned long int *val);
void cmdfree(char** str, int str_cnt);
unsigned long int strnum(char *str);

void solidblack(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt);
void opening(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt);
void openingEpic(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, int order);
void openingCenter(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, int order);
void openingCone(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, int order);
void openingStar(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt);
void constellation(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt);
void solidwhite(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt);
void solidcolor(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, CRGB color);


int setOpeningMode(String cmd);

#endif /* __STRIPS_H__ */
