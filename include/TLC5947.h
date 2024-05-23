/*
 * TLC5947.h
 *
 *  Created on: May 22, 2024
 *      Author: Ben Francis
 */

#ifndef __TLC5947_H_
#define __TLC5947_H_

#include <stdlib.h>
#include "main.h"

//Bit Manipulation Macros
#define SHIFT_4    			    	   4
#define SHIFT_8     				   4
#define LOW_HALF_BYTE_MASK  		   0x000F
#define HIGH_HALF_BYTE_MASK 		   0x00F0
#define FIRST_BYTE_MASK 			   0x00FF

#define TLC5947_NUM_CHANNELS       	   24
#define TLC5947_MIN_GS_VAL    		   0
#define TLC5947_MAX_GS_VAL    		   4095
#define TLC5947_DC_TO_GS(x)       	   (x*TLC5947_MAX_GS_VAL)/100

typedef struct{

    uint8_t *gs_buffer;

    uint16_t num_devices;

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
}TLC5947_STATUS;

typedef enum
{
    DC_0                             = (uint16_t)TLC5947_DC_TO_GS(0),
    DC_10                            = (uint16_t)TLC5947_DC_TO_GS(10),
    DC_20                            = (uint16_t)TLC5947_DC_TO_GS(20),
    DC_30                            = (uint16_t)TLC5947_DC_TO_GS(30),
    DC_40                            = (uint16_t)TLC5947_DC_TO_GS(40),
    DC_50                            = (uint16_t)TLC5947_DC_TO_GS(50),
    DC_60                            = (uint16_t)TLC5947_DC_TO_GS(60),
    DC_70                            = (uint16_t)TLC5947_DC_TO_GS(70),
    DC_80                            = (uint16_t)TLC5947_DC_TO_GS(80),
    DC_90                            = (uint16_t)TLC5947_DC_TO_GS(90),
    DC_100                           = (uint16_t)TLC5947_DC_TO_GS(100)
}TLC5947_GS_VALUE;

TLC5947_STATUS TLC5947_data_init(uint16_t num_devices, GPIO_TypeDef *XLAT_Port, uint16_t XLAT_Pin, GPIO_TypeDef *BLANK_Port, uint16_t BLANK_Pin, SPI_HandleTypeDef *hspi, TLC5947_DATA *TLC5947_data);

TLC5947_STATUS TLC5947_create_GS_buffer(TLC5947_DATA *TLC5947_data);

void TLC5947_remove_GS_buffer(TLC5947_DATA *TLC5947_data);

TLC5947_STATUS TLC5947_update_GS_buffer(uint16_t chan, uint16_t gs_val, TLC5947_DATA *TLC5947_data);

uint16_t TLC5947_get_GS_value(uint16_t chan, TLC5947_DATA *TLC5947_data);

TLC5947_STATUS TLC5947_send_GS_data(TLC5947_DATA *TLC5947_data);

void TLC5947_enable_outputs(TLC5947_DATA *TLC5947_data);

void TLC5947_disable_outputs(TLC5947_DATA *TLC5947_data);

#endif /* __TLC5947_H_ */
