
#ifndef CAN_MESSAGES_H_
#define CAN_MESSAGES_H_

#include "stm32f4xx_hal.h"

HAL_StatusTypeDef CAN_SendButtonCount(CAN_HandleTypeDef *hcan, uint8_t count);

#endif