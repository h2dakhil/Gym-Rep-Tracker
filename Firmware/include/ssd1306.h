#ifndef SSD1306_H
#define SSD1306_H

#include "stm32f1xx_hal.h"

#define SSD1306_I2C_ADDR    (0x3C << 1) // 0x78 or 0x7A
#define SSD1306_WIDTH       128
#define SSD1306_HEIGHT      64

/**
 * @brief Initializes the SSD1306 OLED display.
 * @retval HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef ssd1306_init(void);

/**
 * @brief Clears the entire display.
 */
void ssd1306_clear(void);

/**
 * @brief Draws a character string at the specified (x, y) position.
 * @param x X-coordinate (column).
 * @param y Y-coordinate (page, 0-3 for 32px height).
 * @param text Pointer to the string to draw.
 */
void ssd1306_draw_text(uint8_t x, uint8_t y, const char* text);

/**
 * @brief Updates the display with the content of the framebuffer (if used).
 *        For 128x32 display, this might directly write to GRAM.
 */
void ssd1306_update(void);

#endif // SSD1306_H

