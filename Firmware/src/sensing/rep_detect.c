#include "rep_detect.h"
#include "exercise_config.h"
#include <math.h>

// Static variables for each exercise
static RepDetectState_t rep_state[EX_COUNT];
static uint16_t rep_count[EX_COUNT] = {0};

// Rolling statistics buffer for each exercise
#define ROLLING_BUFFER_SIZE 100
static float sample_buffer[EX_COUNT][ROLLING_BUFFER_SIZE];
static uint16_t buffer_index[EX_COUNT] = {0};
static bool buffer_filled[EX_COUNT] = {0};

// Calibration accumulators
static float calib_sum[EX_COUNT] = {0};
static float calib_sum_sq[EX_COUNT] = {0};
static uint16_t calib_count[EX_COUNT] = {0};

/**
 * @brief Initializes the rep detection system.
 */
void rep_detect_init(void)
{
    for (int ex = 0; ex < EX_COUNT; ex++)
    {
        // Initialize state
        rep_state[ex].last_rep_time_ms = 0;
        rep_state[ex].last_peak_time_ms = 0;
        rep_state[ex].last_peak_value = 0.0f;
        rep_state[ex].threshold = 0.0f;
        rep_state[ex].mean = 0.0f;
        rep_state[ex].std_dev = 0.0f;
        rep_state[ex].sample_count = 0;
        rep_state[ex].in_peak = false;
        rep_state[ex].peak_start_time = 0;
        
        // Initialize buffer
        for (int i = 0; i < ROLLING_BUFFER_SIZE; i++)
        {
            sample_buffer[ex][i] = 0.0f;
        }
        buffer_index[ex] = 0;
        buffer_filled[ex] = false;
        
        // Reset counter
        rep_count[ex] = 0;
        
        // Initialize calibration
        calib_sum[ex] = 0.0f;
        calib_sum_sq[ex] = 0.0f;
        calib_count[ex] = 0;
        
        // Mark as not calibrated
        REP_CTX[ex].calibrated = false;
    }
}

/**
 * @brief Begins calibration for a specific exercise.
 */
void rep_detect_begin_calibration(exercise_t ex)
{
    if (ex >= EX_COUNT) return;
    
    // Reset calibration accumulators
    calib_sum[ex] = 0.0f;
    calib_sum_sq[ex] = 0.0f;
    calib_count[ex] = 0;
    
    // Reset rep detection state
    rep_state[ex].sample_count = 0;
    rep_state[ex].in_peak = false;
}

/**
 * @brief Accumulates calibration samples for a specific exercise.
 */
void rep_detect_accumulate_calibration(exercise_t ex, float sample)
{
    if (ex >= EX_COUNT) return;
    
    calib_sum[ex] += sample;
    calib_sum_sq[ex] += sample * sample;
    calib_count[ex]++;
}

/**
 * @brief Ends calibration and stores baseline for a specific exercise.
 */
void rep_detect_end_calibration(exercise_t ex, float *out_mu, float *out_sigma)
{
    if (ex >= EX_COUNT || calib_count[ex] == 0) return;
    
    // Compute mean and standard deviation
    float mu = calib_sum[ex] / calib_count[ex];
    float variance = (calib_sum_sq[ex] / calib_count[ex]) - (mu * mu);
    float sigma = sqrtf(fmaxf(variance, 0.0f));
    
    // Store in runtime context
    REP_CTX[ex].baseline_mu = mu;
    REP_CTX[ex].baseline_sigma = fmaxf(sigma, MIN_SIGMA_FLOOR_G);
    REP_CTX[ex].calibrated = true;
    
    // Output values
    if (out_mu) *out_mu = mu;
    if (out_sigma) *out_sigma = sigma;
}

/**
 * @brief Updates rolling statistics (mean and standard deviation).
 */
static void update_rolling_stats(exercise_t ex, float new_sample)
{
    if (ex >= EX_COUNT) return;
    
    // Add new sample to buffer
    sample_buffer[ex][buffer_index[ex]] = new_sample;
    buffer_index[ex] = (buffer_index[ex] + 1) % ROLLING_BUFFER_SIZE;
    
    if (buffer_index[ex] == 0)
    {
        buffer_filled[ex] = true;
    }
    
    // Compute mean
    float sum = 0.0f;
    uint16_t count = buffer_filled[ex] ? ROLLING_BUFFER_SIZE : buffer_index[ex];
    
    for (uint16_t i = 0; i < count; i++)
    {
        sum += sample_buffer[ex][i];
    }
    rep_state[ex].mean = sum / count;
    
    // Compute standard deviation
    float sum_sq = 0.0f;
    for (uint16_t i = 0; i < count; i++)
    {
        float diff = sample_buffer[ex][i] - rep_state[ex].mean;
        sum_sq += diff * diff;
    }
    rep_state[ex].std_dev = sqrtf(sum_sq / count);
    
    // Update rolling context
    REP_CTX[ex].rolling_mu = rep_state[ex].mean;
    REP_CTX[ex].rolling_sigma = rep_state[ex].std_dev;
    
    // Update dynamic threshold using exercise-specific configuration
    const exercise_cfg_t *cfg = &EX_CFG[ex];
    float sigma_floor = fmaxf(REP_CTX[ex].baseline_sigma, MIN_SIGMA_FLOOR_G);
    float rolling_sigma = fmaxf(rep_state[ex].std_dev, sigma_floor);
    
    rep_state[ex].threshold = REP_CTX[ex].baseline_mu + cfg->thresh_k * rolling_sigma;
}

/**
 * @brief Updates the rep detection with a new sample.
 */
bool rep_detect_update(exercise_t ex, float sample, uint32_t now_ms)
{
    if (ex >= EX_COUNT || !REP_CTX[ex].calibrated) return false;
    
    bool rep_detected = false;
    
    // Update rolling statistics
    update_rolling_stats(ex, sample);
    
    // Check if we have enough samples for reliable statistics
    if (rep_state[ex].sample_count < ROLLING_BUFFER_SIZE)
    {
        rep_state[ex].sample_count++;
        return false;
    }
    
    // Check refractory period using exercise-specific configuration
    const exercise_cfg_t *cfg = &EX_CFG[ex];
    if ((now_ms - rep_state[ex].last_rep_time_ms) < cfg->refractory_ms)
    {
        return false;
    }
    
    // Peak detection logic
    if (!rep_state[ex].in_peak)
    {
        // Look for start of peak (signal crosses above threshold)
        if (sample > rep_state[ex].threshold)
        {
            rep_state[ex].in_peak = true;
            rep_state[ex].peak_start_time = now_ms;
            rep_state[ex].last_peak_value = sample;
        }
    }
    else
    {
        // We're in a peak, track the maximum value
        if (sample > rep_state[ex].last_peak_value)
        {
            rep_state[ex].last_peak_value = sample;
        }
        
        // Check if peak has ended (signal drops below threshold)
        if (sample < rep_state[ex].threshold)
        {
            rep_state[ex].in_peak = false;
            
            // Check if this peak meets the criteria for a rep
            uint32_t peak_duration = now_ms - rep_state[ex].peak_start_time;
            
            if (peak_duration >= MIN_PEAK_INTERVAL_MS && 
                rep_state[ex].last_peak_value >= (rep_state[ex].threshold + cfg->min_prominence_g))
            {
                // Check minimum interval between peaks
                if ((now_ms - rep_state[ex].last_peak_time_ms) >= MIN_PEAK_INTERVAL_MS)
                {
                    rep_detected = true;
                    rep_count[ex]++;
                    rep_state[ex].last_rep_time_ms = now_ms;
                }
            }
            
            rep_state[ex].last_peak_time_ms = now_ms;
        }
    }
    
    return rep_detected;
}

/**
 * @brief Gets the current rep count for a specific exercise.
 */
uint16_t rep_detect_get_count(exercise_t ex)
{
    if (ex >= EX_COUNT) return 0;
    return rep_count[ex];
}

/**
 * @brief Resets the rep counter for a specific exercise.
 */
void rep_detect_reset_count(exercise_t ex)
{
    if (ex >= EX_COUNT) return;
    
    rep_count[ex] = 0;
    rep_state[ex].last_rep_time_ms = 0;
    rep_state[ex].last_peak_time_ms = 0;
}

/**
 * @brief Gets the current detection state for debugging.
 */
void rep_detect_get_state(exercise_t ex, RepDetectState_t *state)
{
    if (ex >= EX_COUNT || state == NULL) return;
    *state = rep_state[ex];
}
