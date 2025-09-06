#ifndef EXERCISE_CONFIG_H
#define EXERCISE_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

// Exercise types
typedef enum {
    EX_BICEP_CURL = 0,
    EX_SHOULDER_PRESS = 1,
    EX_BENCH_PRESS = 2,
    EX_COUNT
} exercise_t;

// Exercise configuration structure
typedef struct {
    const char* name;
    float thresh_k;
    float min_prominence_g;
    uint16_t refractory_ms;
    uint16_t detect_warmup_ms;
} exercise_cfg_t;

// Per-exercise runtime context
typedef struct {
    float baseline_mu;
    float baseline_sigma;
    float rolling_mu;
    float rolling_sigma;
    uint32_t last_peak_ms;
    bool calibrated;
} rep_ctx_t;

// Exercise configuration table with sensible defaults
extern const exercise_cfg_t EX_CFG[EX_COUNT];

// Runtime context for each exercise
extern rep_ctx_t REP_CTX[EX_COUNT];

// Exercise selection timing
#define EXERCISE_SELECT_TIMEOUT_MS 2000  // Auto-advance every 2 seconds
#define CALIBRATION_MS_EX 2000           // Exercise-specific calibration duration
#define DETECT_WARMUP_MS 1000            // Warm-up window before running

// Minimum sigma floor to avoid zero threshold
#define MIN_SIGMA_FLOOR_G 0.05f

#endif // EXERCISE_CONFIG_H

