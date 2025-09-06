#include "ui_oled.h"
#include <string.h>
#include <stdio.h>

// Display layout constants
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 32
#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8
#define MAX_CHARS_PER_LINE (DISPLAY_WIDTH / CHAR_WIDTH)

/**
 * @brief Centers text horizontally on the display.
 * @param text Text to center.
 * @retval uint8_t X-coordinate for centered text.
 */
static uint8_t center_text_x(const char* text)
{
    uint8_t text_width = strlen(text) * CHAR_WIDTH;
    if (text_width >= DISPLAY_WIDTH)
    {
        return 0; // Text too long, start at left edge
    }
    return (DISPLAY_WIDTH - text_width) / 2;
}

/**
 * @brief Shows the splash screen on the OLED display.
 */
void ui_show_splash(const char* title)
{
    ssd1306_clear();
    
    // Show title on first line (centered)
    uint8_t title_x = center_text_x(title);
    ssd1306_draw_text(title_x, 0, title);
    
    // Show subtitle on second line
    const char* subtitle = "Ready to Track!";
    uint8_t subtitle_x = center_text_x(subtitle);
    ssd1306_draw_text(subtitle_x, 1, subtitle);
    
    // Show version or additional info on third line
    const char* info = "v1.0";
    uint8_t info_x = center_text_x(info);
    ssd1306_draw_text(info_x, 2, info);
    
    ssd1306_update();
}

/**
 * @brief Shows the exercise selection screen.
 */
void ui_show_select(const char* exercise_name)
{
    ssd1306_clear();
    
    // Show "Select:" on first line (centered)
    const char* select_text = "Select:";
    uint8_t select_x = center_text_x(select_text);
    ssd1306_draw_text(select_x, 0, select_text);
    
    // Show exercise name on second line (centered)
    uint8_t exercise_x = center_text_x(exercise_name);
    ssd1306_draw_text(exercise_x, 1, exercise_name);
    
    // Show instruction on third line
    const char* instruction = "Auto-advance in 2s";
    uint8_t instruction_x = center_text_x(instruction);
    ssd1306_draw_text(instruction_x, 2, instruction);
    
    ssd1306_update();
}

/**
 * @brief Shows the calibration screen for a specific exercise.
 */
void ui_show_calibrating(const char* exercise_name)
{
    ssd1306_clear();
    
    // Show exercise name on first line (centered)
    uint8_t exercise_x = center_text_x(exercise_name);
    ssd1306_draw_text(exercise_x, 0, exercise_name);
    
    // Show "Calibrating..." on second line (centered)
    const char* calib_text = "Calibrating...";
    uint8_t calib_x = center_text_x(calib_text);
    ssd1306_draw_text(calib_x, 1, calib_text);
    
    // Show instruction on third line
    const char* instruction = "Hold Still...";
    uint8_t instruction_x = center_text_x(instruction);
    ssd1306_draw_text(instruction_x, 2, instruction);
    
    ssd1306_update();
}

/**
 * @brief Shows the status message on the OLED display.
 */
void ui_show_status(const char* status)
{
    ssd1306_clear();
    
    // Show status on second line (centered)
    uint8_t status_x = center_text_x(status);
    ssd1306_draw_text(status_x, 1, status);
    
    // Show instruction on third line
    const char* instruction = "Hold Still...";
    uint8_t instruction_x = center_text_x(instruction);
    ssd1306_draw_text(instruction_x, 2, instruction);
    
    ssd1306_update();
}

/**
 * @brief Shows the exercise name and rep count on the OLED display.
 */
void ui_show_exercise_and_count(const char* exercise_name, uint16_t count)
{
    ssd1306_clear();
    
    // Show exercise name on first line (truncated if too long)
    char exercise_line[22]; // Max 21 chars + null terminator
    strncpy(exercise_line, exercise_name, 21);
    exercise_line[21] = '\0';
    
    // Truncate with ellipsis if too long
    if (strlen(exercise_name) > 21)
    {
        strcpy(exercise_line + 18, "...");
    }
    
    uint8_t exercise_x = center_text_x(exercise_line);
    ssd1306_draw_text(exercise_x, 0, exercise_line);
    
    // Show rep count on second line
    char count_line[22];
    snprintf(count_line, sizeof(count_line), "Reps: %d", count);
    uint8_t count_x = center_text_x(count_line);
    ssd1306_draw_text(count_x, 1, count_line);
    
    // Show status on third line
    const char* status = "Keep Going!";
    uint8_t status_x = center_text_x(status);
    ssd1306_draw_text(status_x, 2, status);
    
    ssd1306_update();
}

/**
 * @brief Shows a two-line display with exercise name and count.
 */
void ui_show_two_lines(const char* line1, const char* line2)
{
    ssd1306_clear();
    
    // Show first line (centered)
    uint8_t line1_x = center_text_x(line1);
    ssd1306_draw_text(line1_x, 1, line1);
    
    // Show second line (centered)
    uint8_t line2_x = center_text_x(line2);
    ssd1306_draw_text(line2_x, 2, line2);
    
    ssd1306_update();
}

/**
 * @brief Clears the display and shows a centered message.
 */
void ui_show_centered_message(const char* message)
{
    ssd1306_clear();
    
    // Show message on second line (centered)
    uint8_t message_x = center_text_x(message);
    ssd1306_draw_text(message_x, 1, message);
    
    ssd1306_update();
}
