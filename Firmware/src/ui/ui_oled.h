#ifndef UI_OLED_H
#define UI_OLED_H

#include "ssd1306.h"

/**
 * @brief Shows the splash screen on the OLED display.
 * @param title Title text to display (e.g., "Gym Rep Tracker").
 */
void ui_show_splash(const char* title);

/**
 * @brief Shows the exercise selection screen.
 * @param exercise_name Name of the currently selected exercise.
 */
void ui_show_select(const char* exercise_name);

/**
 * @brief Shows the calibration screen for a specific exercise.
 * @param exercise_name Name of the exercise being calibrated.
 */
void ui_show_calibrating(const char* exercise_name);

/**
 * @brief Shows the status message on the OLED display.
 * @param status Status text to display (e.g., "Detecting...").
 */
void ui_show_status(const char* status);

/**
 * @brief Shows the exercise name and rep count on the OLED display.
 * @param exercise_name Name of the exercise (e.g., "Bicep Curl").
 * @param count Current rep count.
 */
void ui_show_exercise_and_count(const char* exercise_name, uint16_t count);

/**
 * @brief Shows a two-line display with exercise name and count.
 * @param line1 First line text.
 * @param line2 Second line text.
 */
void ui_show_two_lines(const char* line1, const char* line2);

/**
 * @brief Clears the display and shows a centered message.
 * @param message Message to display.
 */
void ui_show_centered_message(const char* message);

#endif // UI_OLED_H
