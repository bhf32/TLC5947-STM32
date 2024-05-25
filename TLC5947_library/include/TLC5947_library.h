/*
 * TLC5947_library.h
 *
 *  Created on: May 22, 2024
 *      Author: Ben Francis
 */

#ifndef __TLC5947_LIBRARY_H_
#define __TLC5947_LIBRARY_H_

#include <stdbool.h>
#include "TLC5947.h"

TLC5947_STATUS init_TLC5947_devices(TLC5947_Handle_t *handle, uint16_t num_devices, GPIO_TypeDef *xlat_port, uint16_t xlat_pin, GPIO_TypeDef *blank_port, uint16_t blank_pin, SPI_HandleTypeDef *hspi);

TLC5947_STATUS update_LEDs_brightness(TLC5947_Handle_t *handle, uint16_t *gs_vals);

TLC5947_STATUS update_LED_brightness(TLC5947_Handle_t *handle, uint16_t led_chan, uint16_t gs_val);

TLC5947_STATUS set_LEDs(TLC5947_Handle_t *handle, bool *set_leds);

TLC5947_STATUS set_LED(TLC5947_Handle_t *handle, uint16_t led_chan);

TLC5947_STATUS reset_LEDs(TLC5947_Handle_t *handle, bool *reset_leds);

TLC5947_STATUS reset_LED(TLC5947_Handle_t *handle, uint16_t led_chan);

TLC5947_STATUS toggle_LEDs(TLC5947_Handle_t *handle, bool *toggle_leds);

TLC5947_STATUS toggle_LED(TLC5947_Handle_t *handle, uint16_t led_chan);

TLC5947_STATUS blink_LEDs(TLC5947_Handle_t *handle, bool *blink_leds, uint32_t delay);

TLC5947_STATUS blink_LED(TLC5947_Handle_t *handle, uint16_t led_chan, uint32_t delay);

#endif /* __TLC5947_LIBRARY_H_ */
