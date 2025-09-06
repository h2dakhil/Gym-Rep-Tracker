#ifndef APP_STATE_H
#define APP_STATE_H

typedef enum
{
    APP_STATE_BOOT = 0,
    APP_STATE_SELECTING_EXERCISE,    // Exercise selection screen
    APP_STATE_CALIBRATING_EXERCISE,  // Exercise-specific calibration
    APP_STATE_DETECTING,             // Warm-up detection window
    APP_STATE_RUNNING                // Live rep counting
} AppState_t;

#endif // APP_STATE_H
