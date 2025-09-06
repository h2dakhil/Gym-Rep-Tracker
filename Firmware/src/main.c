#include "main.h"
#include "stm32f1xx_hal.h"
#include "app_config.h"
#include "mcu_pinmap.h"
#include "i2c_bus.h"
#include "mpu6050.h"
#include "ssd1306.h"
#include "systick.h"
#include "log_uart.h"
#include "app_controller.h"
#include "exercise_config.h"

// Global HAL handles
I2C_HandleTypeDef hi2c1;
#ifdef ENABLE_LOG_UART
UART_HandleTypeDef huart2;
#endif

// Function prototypes
static void SystemClock_Config(void);
static void GPIO_Init(void);

/**
 * @brief Main function.
 */
int main(void)
{
    // HAL initialization
    HAL_Init();
    
    // Configure system clock
    SystemClock_Config();
    
    // Initialize GPIO
    GPIO_Init();
    
    // Initialize I2C bus
    if (i2c_bus_init() != HAL_OK)
    {
        Error_Handler();
    }
    
#ifdef ENABLE_LOG_UART
    // Initialize optional UART logging
    log_uart_init();
#endif
    
    // Initialize MPU-6050
    if (mpu6050_init() != HAL_OK)
    {
        Error_Handler();
    }
    
    // Initialize SSD1306 OLED
    if (ssd1306_init() != HAL_OK)
    {
        Error_Handler();
    }
    
    // Initialize system tick
    systick_init();
    
    // Initialize application controller
    app_controller_init();
    
    // Main application loop
    while (1)
    {
        app_controller_loop();
        
        // Small delay to prevent tight loop
        HAL_Delay(1);
    }
}

/**
 * @brief Configures the system clock to 72 MHz.
 */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Configure the main internal regulator output voltage
    // Note: STM32F1 doesn't have PWREx functions, so we skip this step

    // Initializes the RCC Oscillators according to the specified parameters
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    // Initializes the CPU, AHB and APB buses clocks
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief Initializes GPIO pins.
 */
static void GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable GPIO clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Configure I2C1 pins (PB6=SCL, PB7=SDA)
    GPIO_InitStruct.Pin = I2C1_SCL_PIN | I2C1_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(I2C1_SCL_GPIO_PORT, &GPIO_InitStruct);

#ifdef ENABLE_LOG_UART
    // Configure UART2 pins (PA2=TX, PA3=RX)
    GPIO_InitStruct.Pin = UART2_TX_PIN | UART2_RX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(UART2_TX_GPIO_PORT, &GPIO_InitStruct);
#endif
}

/**
 * @brief Error handler function.
 */
void Error_Handler(void)
{
    // User can add his own implementation to report the HAL error return state
    __disable_irq();
    while (1)
    {
        // Error occurred, halt execution
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    // User can add his own implementation to report the file name and line number
    // ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line)
}
#endif
