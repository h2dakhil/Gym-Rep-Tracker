#ifndef REP_DETECT_H
#define REP_DETECT_H

#include <stdint.h>
#include <stdbool.h>
#include "exercise_config.h"

// Constants
#define MIN_PEAK_INTERVAL_MS 200  // Minimum time between peaks to count as separate reps

// Rep detection state structure
typedef struct {
    uint32_t last_rep_time_ms;
    uint32_t last_peak_time_ms;
    float last_peak_value;
    float threshold;
    float mean;
    float std_dev;
    uint16_t sample_count;
    bool in_peak;
    uint32_t peak_start_time;
} RepDetectState_t;

// Function declarations
void rep_detect_init(void);
void rep_detect_begin_calibration(exercise_t ex);
void rep_detect_accumulate_calibration(exercise_t ex, float sample);
void rep_detect_end_calibration(exercise_t ex, float *out_mu, float *out_sigma);
bool rep_detect_update(exercise_t ex, float sample, uint32_t now_ms);
uint16_t rep_detect_get_count(exercise_t ex);
void rep_detect_reset_count(exercise_t ex);
void rep_detect_get_state(exercise_t ex, RepDetectState_t *state);

#endif // REP_DETECT_H
