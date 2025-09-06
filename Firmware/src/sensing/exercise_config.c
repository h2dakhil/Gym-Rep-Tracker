#include "exercise_config.h"

// Exercise configuration table with sensible defaults
const exercise_cfg_t EX_CFG[EX_COUNT] = {
    [EX_BICEP_CURL] = {
        .name = "Bicep Curl",
        .thresh_k = 2.0f,           // 2 sigma threshold
        .min_prominence_g = 0.5f,   // Minimum 0.5g peak prominence
        .refractory_ms = 800,        // 800ms refractory period
        .detect_warmup_ms = 1000     // 1 second warm-up
    },
    [EX_SHOULDER_PRESS] = {
        .name = "Shoulder Press",
        .thresh_k = 2.5f,           // 2.5 sigma threshold
        .min_prominence_g = 0.7f,   // Minimum 0.7g peak prominence
        .refractory_ms = 1000,       // 1 second refractory period
        .detect_warmup_ms = 1200     // 1.2 second warm-up
    },
    [EX_BENCH_PRESS] = {
        .name = "Bench Press",
        .thresh_k = 3.0f,           // 3 sigma threshold
        .min_prominence_g = 1.0f,   // Minimum 1.0g peak prominence
        .refractory_ms = 1200,       // 1.2 second refractory period
        .detect_warmup_ms = 1500     // 1.5 second warm-up
    }
};

// Runtime context for each exercise
rep_ctx_t REP_CTX[EX_COUNT] = {0};
