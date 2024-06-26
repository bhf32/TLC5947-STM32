/*
 * TLC5947.c
 *
 *  Created on: May 22, 2024
 *      Author: Ben Francis
 */
#include "TLC5947.h"

TLC5947_STATUS TLC5947_init(TLC5947_Handle_t *handle, uint16_t num_devices, GPIO_TypeDef *xlat_port, uint16_t xlat_pin, GPIO_TypeDef *blank_port, uint16_t blank_pin, SPI_HandleTypeDef *hspi){
    //Set the number of devices for the TLC5947_Handle_t instance
	handle->num_devices = num_devices;

    //Assign the port and pins for XLAT pin
	handle->XLAT_Port = xlat_port;
	handle->XLAT_Pin = xlat_pin;

    //Assign the port and pins for BLANK pin
	handle->BLANK_Port = blank_port;
	handle->BLANK_Pin = blank_pin;

	handle->hspi = hspi;

    //Allocate space in memory for the TLC5947_Handle_t instance pwm buffer
    if(TLC5947_create_GS_buffer(handle) == TLC5947_ERROR_MEMORY){
    	return TLC5947_ERROR_MEMORY;
    }

    return TLC5947_OK;
}

TLC5947_STATUS TLC5947_create_GS_buffer(TLC5947_Handle_t *handle){

	const uint32_t gs_buffer_size = (3 * TLC5947_NUM_CHANNELS * handle->num_devices) / 2;
	handle->gs_buffer = (uint8_t *)malloc(gs_buffer_size);
	if(!handle->gs_buffer){
		return TLC5947_ERROR_MEMORY;
	}
	memset(handle->gs_buffer, DC_0, gs_buffer_size);

	return TLC5947_OK;
}

void TLC5947_remove_GS_buffer(TLC5947_Handle_t *handle){
    //Deallocate space in memory for the TLC5947_Handle_t instance's pwm buffer
    free(handle->gs_buffer);
    handle->gs_buffer = NULL;
}

TLC5947_STATUS TLC5947_update_GS_buffer(TLC5947_Handle_t *handle, uint16_t chan, uint16_t gs_val){
    //Limit the pwm value to the max allowed grayscale value
    if(gs_val > TLC5947_MAX_GS_VAL){
        gs_val = TLC5947_MAX_GS_VAL;
    }
    //Limit the channel to the max number of channels possible
    if(chan >= TLC5947_NUM_CHANNELS * handle->num_devices){
        return TLC5947_ERROR_INVALID_ARG;
    }

    //Set the pwm value for the specified channel
    uint16_t adjusted_chan = (3 * chan) / 2;
    if (chan % 2 == 0){
    	handle->gs_buffer[adjusted_chan] = gs_val & FIRST_BYTE_MASK;
    	handle->gs_buffer[adjusted_chan + 1] = (handle->gs_buffer[adjusted_chan + 1] & HIGH_HALF_BYTE_MASK) | ((gs_val >> SHIFT_8) & LOW_HALF_BYTE_MASK);
    }
    else{
    	handle->gs_buffer[adjusted_chan] = (handle->gs_buffer[adjusted_chan] & LOW_HALF_BYTE_MASK) | ((gs_val << SHIFT_4) & HIGH_HALF_BYTE_MASK);
    	handle->gs_buffer[adjusted_chan + 1] = (gs_val >> SHIFT_4) & FIRST_BYTE_MASK;
    }

    return TLC5947_OK;
}

uint16_t TLC5947_get_GS_value(TLC5947_Handle_t *handle, uint16_t chan){
    //Limit the channel to the max number of channels possible
    if(chan >= (TLC5947_NUM_CHANNELS * handle->num_devices)){
        return UINT16_MAX;
    }

	//return the pwm value for the specified channel
    uint16_t adjust_chan = (3 * chan) / 2;
	if (chan % 2 == 0) {
		return (uint16_t)(handle->gs_buffer[adjust_chan] | ((handle->gs_buffer[adjust_chan + 1] & LOW_HALF_BYTE_MASK) << SHIFT_8));
	} else {
		return (uint16_t)(((handle->gs_buffer[adjust_chan] & HIGH_HALF_BYTE_MASK) >> SHIFT_4) | (handle->gs_buffer[adjust_chan + 1] << SHIFT_4));
	}
}

TLC5947_STATUS TLC5947_send_GS_data(TLC5947_Handle_t *handle){
	HAL_GPIO_WritePin(handle->XLAT_Port, handle->XLAT_Pin, GPIO_PIN_RESET);

	HAL_StatusTypeDef spi_status;
	for(int ind = (3 * TLC5947_NUM_CHANNELS*handle->num_devices)/2 - 1; ind >= 0; ind--){
		//Transmit PWM Grayscale data to device over SPI
		spi_status = HAL_SPI_Transmit(handle->hspi, &(handle->gs_buffer[ind]), 1, TLC5947_SPI_TIMEOUT);
		if(spi_status != HAL_OK) {
			return TLC5947_ERROR_SPI;
		}
	}

	//Shift grayscale data from shift register to data latch
	HAL_GPIO_WritePin(handle->XLAT_Port, handle->XLAT_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(handle->XLAT_Port, handle->XLAT_Pin, GPIO_PIN_RESET);

	return TLC5947_OK;
}

void TLC5947_enable_outputs(TLC5947_Handle_t *handle){
	//Set TLC5947 BLANK pins to low
	HAL_GPIO_WritePin(handle->BLANK_Port, handle->BLANK_Pin, GPIO_PIN_RESET);
}

void TLC5947_disable_outputs(TLC5947_Handle_t *handle){
	//Set TLC5947 BLANK pins to high
	HAL_GPIO_WritePin(handle->BLANK_Port, handle->BLANK_Pin, GPIO_PIN_SET);
}
