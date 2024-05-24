/*
 * TLC5947.c
 *
 *  Created on: May 22, 2024
 *      Author: Ben Francis
 */

#include "TLC5947.h"

/**
 * Initialize the TLC5947_DATA struct and its variables
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
TLC5947_STATUS TLC5947_data_init(uint16_t num_devices, GPIO_TypeDef *XLAT_Port, uint16_t XLAT_Pin, GPIO_TypeDef *BLANK_Port, uint16_t BLANK_Pin, SPI_HandleTypeDef *hspi, TLC5947_DATA *TLC5947_data){
    //Set the number of drivers for the TLC5947_DATA instance
    TLC5947_data->num_devices = num_devices;

    //Assign the port and pins for XLAT pin
    TLC5947_data->XLAT_Port = XLAT_Port;
    TLC5947_data->XLAT_Pin = XLAT_Pin;

    //Assign the port and pins for BLANK pin
    TLC5947_data->BLANK_Port = BLANK_Port;
    TLC5947_data->BLANK_Pin = BLANK_Pin;

    TLC5947_data->hspi = hspi;

    //Allocate space in memory for the TLC5947_DATA instance pwm buffer
    if(TLC5947_create_GS_buffer(TLC5947_data) == TLC5947_ERROR_MEMORY){
    	return TLC5947_ERROR_MEMORY;
    }

    return TLC5947_OK;
}

/**
 * Allocate the space needed for the TLC5947 driver gs_buffer
 *
 * @param TLC5947_data - TLC5947 driver data
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_MEMORY otherwise
 */
TLC5947_STATUS TLC5947_create_GS_buffer(TLC5947_DATA *TLC5947_data){

	const uint32_t gs_buffer_size = (3 * TLC5947_NUM_CHANNELS * TLC5947_data->num_devices) / 2;
	TLC5947_data->gs_buffer = (uint8_t *)malloc(gs_buffer_size);
	if(!TLC5947_data->gs_buffer){
		return TLC5947_ERROR_MEMORY;
	}
	memset(TLC5947_data->gs_buffer, DC_0, gs_buffer_size);

	return TLC5947_OK;
}

/**
 * Free the space needed for the TLC5947 driver gs_buffer
 *
 * @param TLC5947_data - TLC5947 driver data
 *
 * @return NONE
 */
void TLC5947_remove_GS_buffer(TLC5947_DATA *TLC5947_data){
    //Deallocate space in memory for the TLC5947_DATA instance pwm buffer
    free(TLC5947_data->gs_buffer);
    TLC5947_data->gs_buffer = NULL;
}

/**
 * Set the grayscale value (0 - 4095) for a specific channel
 *
 * @param chan - TLC5947 channel to be updated
 * @param gs_val - grayscale value to be assigned
 * @param TLC5947_data - TLC5947 driver data
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_INVALID_ARG otherwise
 */
TLC5947_STATUS TLC5947_update_GS_buffer(uint16_t chan, uint16_t gs_val, TLC5947_DATA *TLC5947_data){
    //Limit the pwm value to the max allowed grayscale value
    if(gs_val > TLC5947_MAX_GS_VAL){
        gs_val = TLC5947_MAX_GS_VAL;
    }
    //Limit the channel to the max number of channels possible
    if(chan >= TLC5947_NUM_CHANNELS * TLC5947_data->num_devices){
        return TLC5947_ERROR_INVALID_ARG;
    }

    //Set the pwm value for the specified channel
    uint16_t adjusted_chan = (3 * chan) / 2;
    if (chan % 2 == 0){
        TLC5947_data->gs_buffer[adjusted_chan] = gs_val & FIRST_BYTE_MASK;
        TLC5947_data->gs_buffer[adjusted_chan + 1] = (TLC5947_data->gs_buffer[chan + 1] & HIGH_HALF_BYTE_MASK) | ((gs_val >> SHIFT_8) & LOW_HALF_BYTE_MASK);
    }
    else{
        TLC5947_data->gs_buffer[adjusted_chan] = (TLC5947_data->gs_buffer[chan] & LOW_HALF_BYTE_MASK) | ((gs_val << SHIFT_4) & HIGH_HALF_BYTE_MASK);
        TLC5947_data->gs_buffer[adjusted_chan + 1] = (gs_val >> SHIFT_4) & FIRST_BYTE_MASK;
    }

    return TLC5947_OK;
}

/**
 * Get the grayscale value (0 - 4095) for a specific channel
 *
 * @param chan - TLC5947 channel to be updated
 * @param gs_val - grayscale value to be assigned
 * @param TLC5947_data - TLC5947 driver data
 *
 * @return the current grayscale value for the correct channel
 */
uint16_t TLC5947_get_GS_value(uint16_t chan, TLC5947_DATA *TLC5947_data){
    //Limit the channel to the max number of channels possible
    if(chan >= (TLC5947_NUM_CHANNELS * TLC5947_data->num_devices)){
        return UINT16_MAX;
    }

	//return the pwm value for the specified channel
    uint16_t adjust_chan = (3 * chan) / 2;
	if (chan % 2 == 0) {
		return (uint16_t)(TLC5947_data->gs_buffer[adjust_chan] | ((TLC5947_data->gs_buffer[adjust_chan + 1] & LOW_HALF_BYTE_MASK) << SHIFT_8));
	} else {
		return (uint16_t)(((TLC5947_data->gs_buffer[adjust_chan] & HIGH_HALF_BYTE_MASK) >> SHIFT_4) | (TLC5947_data->gs_buffer[adjust_chan + 1] << SHIFT_4));
	}
}

/**
 * Write the current grayscale values to the connected TLC5947 devices through SPI
 *
 * @param TLC5947_data - TLC5947 driver data
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_SPI otherwise
 */
TLC5947_STATUS TLC5947_send_GS_data(TLC5947_DATA *TLC5947_data){
	HAL_GPIO_WritePin(TLC5947_data->XLAT_Port, TLC5947_data->XLAT_Pin, GPIO_PIN_RESET);

	HAL_StatusTypeDef spi_status;
	for(int ind = (3 * TLC5947_NUM_CHANNELS*TLC5947_data->num_devices) / 2 - 1; ind >= 0; ind--){
		spi_status = HAL_SPI_Transmit(TLC5947_data->hspi, &(TLC5947_data->gs_buffer[ind]), 1, HAL_MAX_DELAY);
		if(spi_status != HAL_OK) {
			return TLC5947_ERROR_SPI;
		}
	}

	HAL_GPIO_WritePin(TLC5947_data->XLAT_Port, TLC5947_data->XLAT_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(TLC5947_data->XLAT_Port, TLC5947_data->XLAT_Pin, GPIO_PIN_RESET);

	return TLC5947_OK;
}

/**
 * Enable the TLC5947 output pins
 *
 * @param TLC5947_data - TLC5947 driver data
 *
 * @return NONE
 */
void TLC5947_enable_outputs(TLC5947_DATA *TLC5947_data){
	//Set TLC5947 BLANK pins to low
	HAL_GPIO_WritePin(TLC5947_data->BLANK_Port, TLC5947_data->BLANK_Pin, GPIO_PIN_RESET);
}

/**
 * Disable the TLC5947 output pins
 *
 * @param TLC5947_data - TLC5947 driver data
 *
 * @return NONE
 */
void TLC5947_disable_outputs(TLC5947_DATA *TLC5947_data){
	//Set TLC5947 BLANK pins to high
	HAL_GPIO_WritePin(TLC5947_data->BLANK_Port, TLC5947_data->BLANK_Pin, GPIO_PIN_SET);
}
