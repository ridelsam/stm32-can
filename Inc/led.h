#ifndef LED_H_
#define LED_H_

#include <stdbool.h>
#include <stdint.h>



bool LED_ReadState(void);
bool LED_Manage_Output(uint8_t command);

#endif