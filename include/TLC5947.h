/*
 * TLC5947.h
 *
 *  Created on: May 22, 2024
 *      Author: Ben Francis
 */

#ifndef __TLC5947_H_
#define __TLC5947_H_

#define TLC5947_MAX_NUM_CHANNELS     24
#define TLC5947_MIN_GRAYSCALE_VAL    0
#define TLC5947_MAX_GRAYSCALE_VAL    4095
#define TLC5947_DC_TO_GS_FACTOR      ((float)MAX_GRAYSCALE_VAL/100)

typedef struct{

    uint8_t *pwm_buffer;

    uint16_t num_drivers;

    GPIO_TypeDef *XLAT_Port;

    uint16_t XLAT_Pin;

    GPIO_TypeDef *BLANK_Port;

    uint16_t BLANK_Pin;

    SPI_HandleTypeDef *hspi;

}TLC5947_DATA;

typedef enum {
    TLC5947_OK 						 = 0,
    TLC5947_ERROR_GENERIC 			 = 1,
    TLC5947_ERROR_MEMORY       		 = 2,
    TLC5947_ERROR_INVALID_ARG   	 = 3,
    TLC5947_ERROR_SPI 				 = 4
}TLC5947_Status;

typedef enum
{
    DC_0                             = (uint16_t)(0*DC_TO_GS_FACTOR),
    DC_10                            = (uint16_t)(10*DC_TO_GS_FACTOR),
    DC_20                            = (uint16_t)(20*DC_TO_GS_FACTOR),
    DC_30                            = (uint16_t)(30*DC_TO_GS_FACTOR),
    DC_40                            = (uint16_t)(40*DC_TO_GS_FACTOR),
    DC_50                            = (uint16_t)(50*DC_TO_GS_FACTOR),
    DC_60                            = (uint16_t)(60*DC_TO_GS_FACTOR),
    DC_70                            = (uint16_t)(70*DC_TO_GS_FACTOR),
    DC_80                            = (uint16_t)(80*DC_TO_GS_FACTOR),
    DC_90                            = (uint16_t)(90*DC_TO_GS_FACTOR),
    DC_100                           = (uint16_t)(100*DC_TO_GS_FACTOR)
}DUTY_CYCLE;

TLC5947_Status TLC5947_data_init(uint16_t num_drivers, GPIO_TypeDef *XLAT_Port, uint16_t XLAT_Pin, GPIO_TypeDef *BLANK_Port, uint16_t BLANK_Pin, SPI_HandleTypeDef *hspi, TLC5947_DATA *TLC5947_data);

TLC5947_Status TLC5947_create_buffer(TLC5947_DATA *TLC5947_data);

void TLC5947_free_buffer(TLC5947_DATA *TLC5947_data);

TLC5947_Status TLC5947_set_PWM(uint16_t chan, uint16_t pwm_val, TLC5947_DATA *TLC5947_data);

uint16_t TLC5947_get_PWM(uint16_t chan, TLC5947_DATA *TLC5947_data);

TLC5947_Status TLC5947_write(TLC5947_DATA *TLC5947_data);

void TLC5947_enable_outputs(TLC5947_DATA *TLC5947_data);

void TLC5947_disable_outputs(TLC5947_DATA *TLC5947_data);

#endif /* __TLC5947_H_ */
