#include "button.h"
#include "stm32f4xx_hal.h"
#include "can_message.h"
#include <stdbool.h>


static CAN_HandleTypeDef *button_can;
static uint8_t req_counter = 0;
static volatile uint32_t button_tx_failures = 0;

void Button_Init(CAN_HandleTypeDef *hcan)
{
    button_can = hcan;
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef btngpio = {0};

    btngpio.Pin = GPIO_PIN_13;
    btngpio.Mode = GPIO_MODE_IT_FALLING;
    btngpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &btngpio);

    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static uint32_t last_press_tick = 0;
    static bool have_previous_press = false;

    if (GPIO_Pin != GPIO_PIN_13)
    {
        return;
    }

    uint32_t now = HAL_GetTick();

    if (have_previous_press &&
        (uint32_t)(now - last_press_tick) < 50U)
    {
        return;
    }

    last_press_tick = now;
    have_previous_press = true;
    req_counter++;
	if (CAN_SendButtonCount(button_can, req_counter) != HAL_OK)
	{
		button_tx_failures++;
	}

}