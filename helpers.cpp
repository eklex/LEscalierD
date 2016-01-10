#include "strips.h"

void fadeIn(CRGB_p *pLeds, unsigned int strip_idx, unsigned int led_idx, CRGB color)
{
  int fade_idx = 0;
  for(fade_idx = 0; fade_idx < 128; fade_idx++)
  {
    pLeds[strip_idx][led_idx] = color;
    pLeds[strip_idx][led_idx].fadeLightBy(255 - fade_idx*2);
    FastLED.show();
  }
  pLeds[strip_idx][led_idx] = color;
}

void fadeOut(CRGB_p *pLeds, unsigned int strip_idx, unsigned int led_idx, CRGB color)
{
  int fade_idx = 0;
  for(fade_idx = 0; fade_idx < 128; fade_idx++)
  {
    pLeds[strip_idx][led_idx] = color;
    pLeds[strip_idx][led_idx].fadeToBlackBy(fade_idx*2);
    FastLED.show();
  }
  pLeds[strip_idx][led_idx] = CRGB::Black;
}

void fadeOut(CRGB_p *pLeds, unsigned int strip_idx, unsigned int led_idx)
{
  CRGB color = pLeds[strip_idx][led_idx];
  fadeOut(pLeds, strip_idx, led_idx, color);
}

void fadeOutAll(CRGB_p *pLeds, unsigned int strip_cnt, unsigned int led_cnt, unsigned int scale)
{
  int  strip_idx = 0;
  int  led_idx   = 0;
  int  fade_idx  = 0;
  CRGB color[strip_cnt][led_cnt];
  /* sanity check */
  if(scale == 0)  scale = 1;
  /* backup original LEDs' color */
  for(strip_idx = 0; strip_idx < strip_cnt; strip_idx++)
  {
    memcpy(color[strip_idx], pLeds[strip_idx], led_cnt * sizeof(CRGB));
  }
  /* fade out each LED */
  for(fade_idx = 0; fade_idx < 256/scale; fade_idx++)
  {
    for(strip_idx = 0; strip_idx < strip_cnt; strip_idx++)
    {
      for(led_idx = 0; led_idx < led_cnt; led_idx++)
      {
        pLeds[strip_idx][led_idx] = color[strip_idx][led_idx];
        pLeds[strip_idx][led_idx].fadeToBlackBy(fade_idx*scale);
      }
    }
    FastLED.show();
  }
  /* force every LEDs to black (optional) */
  for(strip_idx = 0; strip_idx < strip_cnt; strip_idx++)
  {
    memset(pLeds[strip_idx], 0, led_cnt * sizeof(CRGB));
  }
}

#if 0
int main(int argc, char** argv)
{
  int i, j, cnt;
  unsigned long int val;
  char **cmd = NULL;

  for(i=1; i<argc; i++)
  {
    printf("%s\n", argv[i]);
    cmd = strsplit(argv[i], " ", &cnt, &val);
    //val = strnum(argv[i]);
    for(j=0; j<cnt; j++)	printf("[%s]", cmd[j]);
    printf("  (%d) -> %lu\n", cnt, val);
    strfree(cmd, cnt);
  }
  strfree(cmd, cnt);
  return 0;
}
#endif

unsigned long int strnum(char *str)
{
  unsigned long int val   = 0;
  unsigned long int val16 = 0;
  int idx = 0;

  while(*(str + idx))
  {
    if(isxdigit(*(str + idx)) ||
       *(str + 1) == 'x' ||
       *(str + 1) == 'X' )
    {
      idx++;
    }
    else break;
  }
  if(*(str + idx) != 0) return 0;

  val   = strtoul(str, NULL, 0);
  val16 = strtoul(str, NULL, 16);
  if(val != 0)        return val;
  else if(val16 != 0) return val16;
  return val;
}

char** cmdsplit(const char *str, const char *sep, int *argc, unsigned long int *val)
{
  char **result       = NULL;
  int  count          = 0;
  char *str_p         = NULL;
  char *last_sep      = NULL;
  char last_sep_char  = 0;
  int  last_sep_cnt   = 0;
  char *sep_p         = NULL;
  int  token_idx      = 0;
  char *token         = NULL;

  /* count separators */
  str_p = (char*)str;
  while(*str_p)
  {
    sep_p = (char*)sep;
    while(*sep_p)
    {
      if (*sep_p == *str_p)
      {
        count++;
        last_sep = str_p;
        last_sep_char = *last_sep;
        last_sep_cnt = count;
      }
      sep_p++;
    }
    str_p++;
  }
  /* if no separator found, exit */
  if(count == 0) return NULL;
  /* add extra space for trailing token and null char */
  count += last_sep < (str + strlen(str) - 1);
  count++;

  result = (char**)malloc(count * sizeof(char*));

  if(result)
  {
    token = strtok((char*)str, sep);
    while(token)
    {
      /* allocate each token */
      *(result + token_idx) = strdup(token);
      /* process token as a value */
      if(token_idx == last_sep_cnt && last_sep_char == ':' && val != NULL)
      {
        *val = strnum(token);
        /* special case for boolean */
        if(!strcmp(token, "true")) *val = 1;
      }
      else if(val != NULL)
      {
        /* Nullify the value if not used */
        *val = 0;
      }

      token_idx++;
      /* extract next token */
      token = strtok(0, sep);
      }

    *(result + token_idx) = 0;
    if(argc != NULL)  *argc = token_idx;
  }

  return result;
}

void cmdfree(char** str, int str_cnt)
{
  int index;
  if(str != NULL && str_cnt > 0)
  {
    for(index = 0; index < str_cnt; index++)
    {
      if(str[index]) free(str[index]);
    }
    free(str);
  }
}
