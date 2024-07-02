/*
 * TLC5947.h
 *
 *  Created on: May 22, 2024
 *      Author: Ben Francis
 */

#ifndef __TLC5947_H_
#define __TLC5947_H_

#include <stdlib.h>
#include <string.h>
#include "stm32l5xx_hal.h"

/** Bit Manipulation Macros */
#define SHIFT_4    			    	   4
#define SHIFT_8     				   8
#define LOW_HALF_BYTE_MASK  		   0x000F
#define HIGH_HALF_BYTE_MASK 		   0x00F0
#define FIRST_BYTE_MASK 			   0x00FF

/** Limits and Commonly used conversion Macro*/
#define TLC5947_NUM_CHANNELS       	   24
#define TLC5947_MIN_GS_VAL    		   0
#define TLC5947_MAX_GS_VAL    		   4095
#define TLC5947_DC_TO_GS(x)       	   (((long long)(x) * TLC5947_MAX_GS_VAL) / 100)
#define TLC5947_SPI_TIMEOUT			   10 	//10 msec timeout

/**
 * @brief TLC5947 Driver Handle struct
 */
typedef struct
{

    uint8_t *gs_buffer;

    uint16_t num_devices;

    GPIO_TypeDef *XLAT_Port;

    uint16_t XLAT_Pin;

    GPIO_TypeDef *BLANK_Port;

    uint16_t BLANK_Pin;

    SPI_HandleTypeDef *hspi;

}TLC5947_Handle_t;

/**
 * @brief Error mapping for driver functions
 */
typedef enum
{
    TLC5947_OK 						 = 0,
    TLC5947_ERROR_MEMORY       		 = 1,
    TLC5947_ERROR_INVALID_ARG   	 = 2,
    TLC5947_ERROR_SPI 				 = 3

}TLC5947_STATUS;

/**
 * @brief Standard Duty Cycles converted to TLC5947 grayscale values
 */
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

}TLC5947_DC_TO_GS_VALUE;

/**
 * @brief Initializes the TLC5947 driver handle and its variables
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
TLC5947_STATUS TLC5947_init(TLC5947_Handle_t *handle, uint16_t num_devices, GPIO_TypeDef *xlat_port, uint16_t xlat_pin, 
							GPIO_TypeDef *blank_port, uint16_t blank_pin, SPI_HandleTypeDef *hspi);

/**
 * @brief Allocates the space needed for the TLC5947 driver gs_buffer
 *
 * @param handle - handle for TLC5947 driver
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_MEMORY otherwise
 */
TLC5947_STATUS TLC5947_create_GS_buffer(TLC5947_Handle_t *handle);

/**
 * @brief Frees the space allocated for the TLC5947 driver gs_buffer
 *
 * @param handle - handle for TLC5947 driver
 *
 * @return NONE
 */
void TLC5947_remove_GS_buffer(TLC5947_Handle_t *handle);

/**
 * @brief Sets the grayscale value (0 - 4095) for a specific channel
 *
 * @param handle - handle for TLC5947 driver
 * @param chan - channel number in the chain of TLC5947 devices (0 < chan < 24 * (# of devices) - 1)
 * @param gs_val - grayscale value to be assigned
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_INVALID_ARG otherwise
 */
TLC5947_STATUS TLC5947_update_GS_buffer(TLC5947_Handle_t *handle, uint16_t chan, uint16_t gs_val);

/**
 * @brief Gets the grayscale value (0 - 4095) for a specific channel
 *
 * @param handle - handle for TLC5947 driver
 * @param chan - channel number in the chain of TLC5947 devices (0 < chan < 24 * (# of devices) - 1)
 * @param gs_val - grayscale value to be assigned
 *
 * @return the current grayscale value for the correct channel, return UINT16_MAX if invalid channel
 */
uint16_t TLC5947_get_GS_value(TLC5947_Handle_t *handle, uint16_t chan);

/**
 * @brief Writes the current grayscale values to the connected TLC5947 devices through SPI
 *
 * @param handle - handle for TLC5947 driver
 *
 * @return TLC5947_OK in case of success, TLC5947_ERROR_SPI otherwise
 */
TLC5947_STATUS TLC5947_send_GS_data(TLC5947_Handle_t *handle);

/**
 * @brief Enables the TLC5947 output pins
 *
 * @param handle - handle for TLC5947 driver
 *
 * @return NONE
 */
void TLC5947_enable_outputs(TLC5947_Handle_t *handle);


/**
 * @brief Disablea the TLC5947 output pins
 *
 * @param handle - handle for TLC5947 driver
 *
 * @return NONE
 */
void TLC5947_disable_outputs(TLC5947_Handle_t *handle);

#endif /* __TLC5947_H_ */
