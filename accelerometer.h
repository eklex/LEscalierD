#ifndef __ACCELERO_H__
#define __ACCELERO_H__

/* Auto-covariance constantes */
static const int   delta = 1;
static const float th    = 3.5;
static const int   w     = 100;
static const int   N     = 50;


typedef struct _acc_detect_t {
  int          sample_buff[w + delta];
  float        cov_buff[N];
  int          sum_sample_last;
  int          sum_sample_delta;
  int          sum_sample_prod;
  float        sum_cov_sqrt;
  unsigned int sample_idx;
  unsigned int sample_last_idx;
  unsigned int sample_delta_idx;
  unsigned int cov_idx;
  float        threshold;
  bool         initialized;
} acc_detect_t;

typedef struct _acc_bundle_t {
    MMA_7455     *device;
    acc_detect_t  detect;
    bool          initialized;
    int           pin_drdy;
    uint16_t      timeout_drdy;
} acc_bundle_t;

#endif /* __ACCELERO_H__ */
