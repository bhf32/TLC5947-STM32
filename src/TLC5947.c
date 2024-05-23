/*
 * TLC5947.c
 *
 *  Created on: May 22, 2024
 *      Author: Ben Francis
 */

/* Function to initiliaze the TLC5947_DATA struct and its variables. Space in
 * memory is allocated for the pwm_buffer based on the number of TLC5947 drivers
 * chained together. */
TLC5947_Status TLC5947_data_init(uint16_t num_drivers, GPIO_TypeDef *XLAT_Port, uint16_t XLAT_Pin, GPIO_TypeDef *BLANK_Port, uint16_t BLANK_Pin, SPI_HandleTypeDef *hspi, TLC5947_DATA *TLC5947_data){
    //Set the number of drivers for the TLC5947_DATA instance
    TLC5947_data->num_drivers = num_drivers;

    //Assign the port and pins for XLAT pin
    TLC5947_data->XLAT_Port = XLAT_Port;
    TLC5947_data->XLAT_Pin = XLAT_Pin;

    //Assign the port and pins for BLANK pin
    TLC5947_data->BLANK_Port = BLANK_Port;
    TLC5947_data->BLANK_Pin = BLANK_Pin;

    TLC5947_data->hspi = hspi;

    //Allocate space in memory for the TLC5947_DATA instance pwm buffer
    if(TLC5947_create_buffer(TLC5947_data) == TLC5947_ERROR_MEMORY){
    	return TLC5947_ERROR_MEMORY;
    }

    return TLC5947_OK;
}

/* Function to allocate the space used to hold the pwm information in the
   pwm_buffer */
TLC5947_Status TLC5947_create_buffer(TLC5947_DATA *TLC5947_data){

	const uint32_t pwm_buffer_size = (3 * TLC5947_NUM_CHANNELS * TLC5947_data->num_drivers) / 2;
	TLC5947_data->pwm_buffer = (uint8_t *)malloc(pwm_buffer_size);
	if(!TLC5947_data->pwm_buffer){
		return TLC5947_ERROR_MEMORY;
	}
	memset(TLC5947_data->pwm_buffer, 0, pwm_buffer_size);

	return TLC5947_OK;
}

/* Function to free/deallocate the space used contain the pwm information in the
   pwm_buffer */
void TLC5947_free_buffer(TLC5947_DATA *TLC5947_data){
    //Deallocate space in memory for the TLC5947_DATA instance pwm buffer
    free(TLC5947_data->pwm_buffer);
    TLC5947_data->pwm_buffer = NULL;
}

/* Function to set the PWM DC value (0 - 4095) for a specific channel (chan). PWM
   value of 0 will set a duty cycle (DC) of 0% and 4095 a duty cycle of 99.98%.
   Channel number on each board can be from 0 to 23, so channel 2 for second
   board will be 25 for example.*/
TLC5947_Status TLC5947_set_PWM(uint16_t chan, uint16_t pwm_val, TLC5947_DATA *TLC5947_data){
    //Limit the pwm value to the max allowed grayscale value
    if(pwm_val > MAX_GRAYSCALE_VAL){
        pwm_val = MAX_GRAYSCALE_VAL;
    }
    //Limit the channel to the max number of channels possible
    if(chan >= TLC5947_NUM_CHANNELS * TLC5947_data->num_drivers){
        return TLC5947_ERROR_INVALID_ARG;
    }

    //Set the pwm value for the specified channel
    uint16_t adjusted_chan = (3 * chan) / 2;
    if (chan % 2 == 0){
        TLC5947_data->pwm_buffer[adjusted_chan] = pwm_val & 0x00FF;
        TLC5947_data->pwm_buffer[adjusted_chan + 1] = (TLC5947_data->pwm_buffer[chan + 1] & 0x00F0) | ((pwm_val >> 8) & 0x000F);
    }
    else{
        TLC5947_data->pwm_buffer[adjusted_chan] = (TLC5947_data->pwm_buffer[chan] & 0x000F) | ((pwm_val << 4) & 0x00F0);
        TLC5947_data->pwm_buffer[adjusted_chan + 1] = (pwm_val >> 4) & 0x00FF;
    }

    return TLC5947_OK;
}

/* Function to return the PWM value for a specific channel. */
uint16_t TLC5947_get_PWM(uint16_t chan, TLC5947_DATA *TLC5947_data){
    //Limit the channel to the max number of channels possible
    if(chan >= (TLC5947_NUM_CHANNELS * TLC5947_data->num_drivers)){
        return UINT16_MAX;
    }

	//return the pwm value for the specified channel
    uint16_t adjust_chan = (3 * chan) / 2;
	if (chan % 2 == 0) {
		return (uint16_t)(TLC5947_data->pwm_buffer[adjust_chan] | ((TLC5947_data->pwm_buffer[adjust_chan + 1] & 0x000F) << 8));
	} else {
		return (uint16_t)(((TLC5947_data->pwm_buffer[adjust_chan] & 0x00F0) >> 4) | (TLC5947_data->pwm_buffer[adjust_chan + 1] << 4));
	}
}

/* Function to write the grayscale values for PWM to the SIN pin on the TLC5947
 * drivers. This follows the serial communication protocol specified by the
 * part's datasheet. PWM values are sent from the last channel to the first (e.g.
 * channel 22 for second driver to channel 0 for first driver) and MSB to LSB. */
TLC5947_Status TLC5947_write(TLC5947_DATA *TLC5947_data){
	HAL_GPIO_WritePin(TLC5947_data->XLAT_Port, TLC5947_data->XLAT_Pin, GPIO_PIN_RESET);

	HAL_StatusTypeDef spi_status;
	for(int ind = (3 * TLC5947_NUM_CHANNELS*TLC5947_data->numdrivers) / 2 - 1; ind >= 0; ind--){
		spi_status = HAL_SPI_Transmit(TLC5947_data->hspi, &(TLC5947_data->pwm_buffer[ind]), 1, HAL_MAX_DELAY);
		if (status != HAL_OK) {
			return TLC5947_ERROR_SPI;
		}
	}

	HAL_GPIO_WritePin(TLC5947_data->XLAT_Port, TLC5947_data->XLAT_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(TLC5947_data->XLAT_Port, TLC5947_data->XLAT_Pin, GPIO_PIN_RESET);

	return TLC5947_OK;
}

/* Function to turn on the constant current outputs for all of the drivers'
 * channels*/
void TLC5947_enable_outputs(TLC5947_DATA *TLC5947_data){
	//Set TLC5947 BLANK pins to low
	HAL_GPIO_WritePin(TLC5947_data->BLANK_Port, TLC5947_data->BLANK_Pin, GPIO_PIN_RESET);
}

/* Function to turn off the constant current outputs for all of the drivers'
 * channels*/
void TLC5947_disable_outputs(TLC5947_DATA *TLC5947_data){
	//Set TLC5947 BLANK pins to high
	HAL_GPIO_WritePin(TLC5947_data->BLANK_Port, TLC5947_data->BLANK_Pin, GPIO_PIN_SET);
}
