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

/**
 * @brief Initializes the TLC5947 driver handle and enable the outputs
 *
 * @param handle - handle for TLC5947 driver
 * @param num_devices - Number of TLC5947 devices on chain
 * @param xlat_port - XLAT pin GPIO port
 * @param xlat_pin - XLAT GPIO pin number
 * @param blank_port - BLANK pin GPIO port
 * @param blank_pin - BLANK GPIO pin number
 * @param hspi - STM32x SPI peripheral handle
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_MEMORY otherwise
 */
TLC5947_STATUS init_TLC5947_devices(TLC5947_Handle_t *handle, uint16_t num_devices, GPIO_TypeDef *xlat_port, uint16_t xlat_pin, GPIO_TypeDef *blank_port, uint16_t blank_pin, SPI_HandleTypeDef *hspi);

/**
 * @brief Updates all grayscale values for the buffer and sends the update to the devices
 *
 * @param handle - handle for TLC5947 driver
 * @param gs_val - pointer to list of grayscale values to be assigned to each LED channel
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_INVALID_ARG or TLC5947_ERROR_SPI if error
 */
TLC5947_STATUS update_LEDs_brightness(TLC5947_Handle_t *handle, uint16_t *gs_vals);

/**
 * @brief Updates the grayscale value for one of the LEDs and sends the update to the devices
 *
 * @param handle - handle for TLC5947 driver
 * @param led_chan - TLC5947 channel to be updated (0 < LED_chan < 24 * (# of devices) - 1)
 * @param gs_val - grayscale value to be assigned
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_INVALID_ARG or TLC5947_ERROR_SPI if error
 */
TLC5947_STATUS update_LED_brightness(TLC5947_Handle_t *handle, uint16_t led_chan, uint16_t gs_val);

/**
 * @brief Turns on multiple LEDs by setting to max brightness
 *
 * @param handle - handle for TLC5947 driver
 * @param set_leds - boolean list of LED channels to set (true -> set LED)
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_INVALID_ARG or TLC5947_ERROR_SPI if error
 */
TLC5947_STATUS set_LEDs(TLC5947_Handle_t *handle, bool *set_leds);

/**
 * @brief Turns on one LED by setting to max brightness
 *
 * @param handle - handle for TLC5947 driver
 * @param led_chan - TLC5947 channel to be updated (0 < LED_chan < 24 * (# of devices) - 1)
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_INVALID_ARG or TLC5947_ERROR_SPI if error
 */
TLC5947_STATUS set_LED(TLC5947_Handle_t *handle, uint16_t led_chan);

/**
 * @brief Turns off multiple LEDs by setting to min brightness
 *
 * @param handle - handle for TLC5947 driver
 * @param set_leds - boolean list of LED channels to reset (true -> reset LED)
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_INVALID_ARG or TLC5947_ERROR_SPI if error
 */
TLC5947_STATUS reset_LEDs(TLC5947_Handle_t *handle, bool *reset_leds);

/**
 * @brief Turns off one of LEDs by setting to min brightness
 *
 * @param handle - handle for TLC5947 driver
 * @param led_chan - TLC5947 channel to be updated (0 < LED_chan < 24 * (# of devices) - 1)
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_INVALID_ARG or TLC5947_ERROR_SPI if error
 */
TLC5947_STATUS reset_LED(TLC5947_Handle_t *handle, uint16_t led_chan);

/**
 * @brief Toggles multiple LEDs (off -> on; on -> off)
 *
 * @param handle - handle for TLC5947 driver
 * @param toggle_leds - boolean list of LED channels to toggle (true -> toggle LED)
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_INVALID_ARG or TLC5947_ERROR_SPI if error
 */
TLC5947_STATUS toggle_LEDs(TLC5947_Handle_t *handle, bool *toggle_leds);

/**
 * @brief Toggles one LED (off -> on; on -> off)
 *
 * @param handle - handle for TLC5947 driver
 * @param led_chan - TLC5947 channel to be updated (0 < LED_chan < 24 * (# of devices) - 1)
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_INVALID_ARG or TLC5947_ERROR_SPI if error
 */
TLC5947_STATUS toggle_LED(TLC5947_Handle_t *handle, uint16_t led_chan);

/**
 * @brief Blinks multiple LEDs by turning on then off w/ a delay between
 *
 * @param handle - handle for TLC5947 driver
 * @param blink_leds - boolean list of LED channels to blink (true -> blink LED)
 * @param delay - time delay between LEDs turning on and off
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_INVALID_ARG or TLC5947_ERROR_SPI if error
 */
TLC5947_STATUS blink_LEDs(TLC5947_Handle_t *handle, bool *blink_leds, uint32_t delay);

/**
 * @brief Blinks one LED by turning on then off w/ a delay between
 *
 * @param handle - handle for TLC5947 driver
 * @param led_chan - TLC5947 channel to be updated (0 < LED_chan < 24 * (# of devices) - 1)
 * @param delay - time delay between LED turning on and off
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_INVALID_ARG or TLC5947_ERROR_SPI if error
 */
TLC5947_STATUS blink_LED(TLC5947_Handle_t *handle, uint16_t led_chan, uint32_t delay);

#endif /* __TLC5947_LIBRARY_H_ */
