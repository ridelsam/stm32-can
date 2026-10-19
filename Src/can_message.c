#include "can_message.h"
#include "led.h"


HAL_StatusTypeDef CAN_SendButtonCount(CAN_HandleTypeDef *hcan, uint8_t count)
{
	CAN_TxHeaderTypeDef TxHeader = {0};
	uint8_t data[8] = {0};
	uint32_t TxMailbox;


	TxHeader.DLC = 2;
	TxHeader.StdId = 0x650;
	TxHeader.IDE   = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;

	

	data[0] = count;
	data[1] = LED_ReadState() ? 1U : 0U;
	return HAL_CAN_AddTxMessage(hcan, &TxHeader, data, &TxMailbox);
    
}