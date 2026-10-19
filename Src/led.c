#include "led.h"
#include "main.h"


bool LED_ReadState(void)
{
    return HAL_GPIO_ReadPin(LED1_PORT, LED1_PIN_NO)
           == GPIO_PIN_SET;
}


bool LED_Manage_Output(uint8_t command)
{
    switch (command)
    {
    case 0x00:
        HAL_GPIO_WritePin(LED1_PORT, LED1_PIN_NO, GPIO_PIN_RESET);
        return true;

    case 0x01:
        HAL_GPIO_WritePin(LED1_PORT, LED1_PIN_NO, GPIO_PIN_SET);
        return true;
    case 0x02:
        HAL_GPIO_WritePin(LED2_PORT, LED2_PIN_NO, GPIO_PIN_RESET);
        return true;

    case 0x03:
        HAL_GPIO_WritePin(LED2_PORT, LED2_PIN_NO, GPIO_PIN_SET);
        return true;
    case 0x04:
        HAL_GPIO_WritePin(LED3_PORT, LED3_PIN_NO, GPIO_PIN_RESET);
        return true;

    case 0x05:
        HAL_GPIO_WritePin(LED3_PORT, LED3_PIN_NO, GPIO_PIN_SET);
        return true;
    case 0x06:
        HAL_GPIO_WritePin(LED4_PORT, LED4_PIN_NO, GPIO_PIN_RESET);
        return true;
    case 0x07:
        HAL_GPIO_WritePin(LED4_PORT, LED4_PIN_NO, GPIO_PIN_SET);
        return true;
    }
    return false;
}

