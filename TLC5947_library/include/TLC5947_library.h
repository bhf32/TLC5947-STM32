/*
 * TLC5947_library.h
 *
 *  Created on: May 22, 2024
 *      Author: Ben Francis
 */

#ifndef __TLC5947_LIBRARY_H_
#define __TLC5947_LIBRARY_H_

#include "TLC5947.h"

TLC5947_STATUS init_TLC5947_devices(TLC5947_Handle_t *handle, uint16_t num_devices, GPIO_TypeDef *XLAT_Port, uint16_t XLAT_Pin, GPIO_TypeDef *BLANK_Port, uint16_t BLANK_Pin, SPI_HandleTypeDef *hspi);

TLC5947_STATUS adjust_LED_brightness(TLC5947_Handle_t *handle, uint16_t LED_chan, uint16_t gs_val);

TLC5947_STATUS set_LED(TLC5947_Handle_t *handle, uint16_t LED_chan);

TLC5947_STATUS reset_LED(TLC5947_Handle_t *handle, uint16_t LED_chan);

TLC5947_STATUS blink_LED(TLC5947_Handle_t *handle, uint16_t LED_chan, uint32_t delay);

#endif /* __TLC5947_LIBRARY_H_ */
