#include "main.h"

extern CAN_HandleTypeDef hcan1;
extern void CAN1_Tx(uint8_t remote);
/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler (void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}


/**
  * @brief This function handles CAN_TX interrupts.
  */
void CAN1_TX_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&hcan1);

}

/**
  * @brief This function handles CAN_RX0 interrupts.
  */
void CAN1_RX0_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&hcan1);
}

/**
  * @brief This function handles CAN RX1 interrupt.
  */
/* void CAN1_RX1_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&hcan1);
} */

/**
  * @brief This function handles CAN SCE interrupt.
  */
void CAN1_SCE_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&hcan1);
}


/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}