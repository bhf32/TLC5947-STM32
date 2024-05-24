/*
 * TLC5947_library.c
 *
 *  Created on: May 22, 2024
 *      Author: Ben Francis
 */

#include "TLC5947_library.h"

/**
 * Initialize the TLC5947_DATA driver handle and enable the outputs
 *
 * @param num_devices - Number of TLC5947 devices on chain
 * @param XLAT_Port - XLAT pin GPIO port
 * @param XLAT_Pin - XLAT GPIO pin number
 * @param BLANK_Port - BLANK pin GPIO port
 * @param BLANK_Pin - BLANK GPIO pin number
 * @param hspi - STM32x SPI peripheral handle
 * @param TLC5947_data - TLC5947 driver data
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_MEMORY otherwise
 */
TLC5947_STATUS init_TLC5947_devices(uint16_t num_devices, GPIO_TypeDef *XLAT_Port, uint16_t XLAT_Pin, GPIO_TypeDef *BLANK_Port, uint16_t BLANK_Pin, SPI_HandleTypeDef *hspi, TLC5947_DATA *TLC5947_data){
	TLC5947_STATUS status = TLC5947_data_init(num_devices, XLAT_Port, XLAT_Pin, BLANK_Port, BLANK_Pin, hspi, TLC5947_data);
	if(status != TLC5947_OK){
		return status;
	}
	TLC5947_enable_outputs(TLC5947_data);
	return status;
}

/**
 * Update the grayscale values for the buffer and send the update to the devices
 *
 * @param TLC5947_data - TLC5947 driver data
 * @param LED_chan - TLC5947 channel to be updated
 * @param gs_val - grayscale value to be assigned
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_INVALID_ARG or TLC5947_ERROR_SPI if error
 */
TLC5947_STATUS adjust_LED_brightness(TLC5947_DATA *TLC5947_data, uint16_t LED_chan, uint16_t gs_val){
	TLC5947_STATUS status = TLC5947_update_GS_buffer(LED_chan, gs_val, TLC5947_data);
	if(status != TLC5947_OK){
		return status;
	}
	status = TLC5947_send_GS_data(TLC5947_data);
	return status;
}

/**
 * Turn on LED by setting to max brightness
 *
 * @param TLC5947_data - TLC5947 driver data
 * @param LED_chan - TLC5947 channel to be updated
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_INVALID_ARG or TLC5947_ERROR_SPI if error
 */
TLC5947_STATUS set_LED(TLC5947_DATA *TLC5947_data, uint16_t LED_chan){
	TLC5947_STATUS status = adjust_LED_brightness(TLC5947_data, LED_chan, DC_100);
	return status;
}

/**
 * Turn off LED by setting to min brightness
 *
 * @param TLC5947_data - TLC5947 driver data
 * @param LED_chan - TLC5947 channel to be updated
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_INVALID_ARG or TLC5947_ERROR_SPI if error
 */
TLC5947_STATUS reset_LED(TLC5947_DATA *TLC5947_data, uint16_t LED_chan){
	TLC5947_STATUS status = adjust_LED_brightness(TLC5947_data, LED_chan, DC_0);
	return status;
}

/**
 * Blink LED by turning on then off w/ a delay between
 *
 * @param TLC5947_data - TLC5947 driver data
 * @param LED_chan - TLC5947 channel to be updated
 * @param delay - time delay between LED turning on and off
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_INVALID_ARG or TLC5947_ERROR_SPI if error
 */
TLC5947_STATUS blink_LED(TLC5947_DATA *TLC5947_data, uint16_t LED_chan, uint32_t delay){
	TLC5947_STATUS status = set_LED(TLC5947_data, LED_chan);
	if(status != TLC5947_OK){
		return status;
	}
	HAL_Delay(delay);
	status = reset_LED(TLC5947_data, LED_chan);
	return status;
}

