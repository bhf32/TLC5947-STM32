/*
 * TLC5947_library.c
 *
 *  Created on: May 22, 2024
 *      Author: Ben Francis
 */
#include "TLC5947_library.h"

TLC5947_STATUS init_TLC5947_devices(TLC5947_Handle_t *handle, uint16_t num_devices, GPIO_TypeDef *xlat_port, uint16_t xlat_pin, GPIO_TypeDef *blank_port, uint16_t blank_pin, SPI_HandleTypeDef *hspi){
	TLC5947_STATUS status = TLC5947_init(handle, num_devices, xlat_port, xlat_pin, blank_port, blank_pin, hspi);
	if(status != TLC5947_OK){
		return status;
	}
	TLC5947_enable_outputs(handle);
	return status;
}

TLC5947_STATUS update_LEDs_brightness(TLC5947_Handle_t *handle, uint16_t *gs_vals){
	TLC5947_STATUS status;
	for(size_t LED_chan = 0; LED_chan < TLC5947_NUM_CHANNELS * handle->num_devices; LED_chan++){
		status = TLC5947_update_GS_buffer(handle, LED_chan, gs_vals[LED_chan]);
		if(status != TLC5947_OK){
			return status;
		}
	}
	return TLC5947_send_GS_data(handle);
}

TLC5947_STATUS update_LED_brightness(TLC5947_Handle_t *handle, uint16_t led_chan, uint16_t gs_val){
	TLC5947_STATUS status = TLC5947_update_GS_buffer(handle, led_chan, gs_val);
	if(status != TLC5947_OK){
		return status;
	}
	return TLC5947_send_GS_data(handle);
}

TLC5947_STATUS set_LEDs(TLC5947_Handle_t *handle, bool *set_leds){
	TLC5947_STATUS status;
	for(size_t LED_chan = 0; LED_chan < TLC5947_NUM_CHANNELS * handle->num_devices; LED_chan++){
		if(set_leds[LED_chan]){
			status = TLC5947_update_GS_buffer(handle, LED_chan, DC_100);
			if(status != TLC5947_OK){
				return status;
			}
		}
	}
	return TLC5947_send_GS_data(handle);
}

TLC5947_STATUS set_LED(TLC5947_Handle_t *handle, uint16_t led_chan){
	TLC5947_STATUS status = update_LED_brightness(handle, led_chan, DC_100);
	return status;
}

TLC5947_STATUS reset_LEDs(TLC5947_Handle_t *handle, bool *reset_leds){
	TLC5947_STATUS status;
	for(size_t LED_chan = 0; LED_chan < TLC5947_NUM_CHANNELS * handle->num_devices; LED_chan++){
		if(reset_leds[LED_chan]){
			status = TLC5947_update_GS_buffer(handle, LED_chan, DC_0);
			if(status != TLC5947_OK){
				return status;
			}
		}
	}
	return TLC5947_send_GS_data(handle);
}

TLC5947_STATUS reset_LED(TLC5947_Handle_t *handle, uint16_t led_chan){
	TLC5947_STATUS status = update_LED_brightness(handle, led_chan, DC_0);
	return status;
}

TLC5947_STATUS toggle_LEDs(TLC5947_Handle_t *handle, bool *toggle_leds){
	TLC5947_STATUS status;
	uint16_t curr_gs_val;
	for(size_t LED_chan = 0; LED_chan < TLC5947_NUM_CHANNELS * handle->num_devices; LED_chan++){
		if(toggle_leds[LED_chan]){
			curr_gs_val = TLC5947_get_GS_value(handle, LED_chan);
			if(curr_gs_val == UINT16_MAX){
				return TLC5947_ERROR_INVALID_ARG;
			}
			else if(curr_gs_val > DC_0){
				status = TLC5947_update_GS_buffer(handle, LED_chan, DC_0);
			}
			else{
				status = TLC5947_update_GS_buffer(handle, LED_chan, DC_100);
			}
			if(status != TLC5947_OK){
				return status;
			}
		}
	}
	return TLC5947_send_GS_data(handle);
}

TLC5947_STATUS toggle_LED(TLC5947_Handle_t *handle, uint16_t led_chan){
	uint16_t curr_gs_val = TLC5947_get_GS_value(handle, led_chan);
	TLC5947_STATUS status;
	if(curr_gs_val == UINT16_MAX){
		return TLC5947_ERROR_INVALID_ARG;
	}
	else if(curr_gs_val > DC_0){
		status = update_LED_brightness(handle, led_chan, DC_0);
	}
	else{
		status = update_LED_brightness(handle, led_chan, DC_100);
	}
	return status;
}

TLC5947_STATUS blink_LEDs(TLC5947_Handle_t *handle, bool *blink_leds, uint32_t delay){
	TLC5947_STATUS status = set_LEDs(handle, blink_leds);
	if(status != TLC5947_OK){
		return status;
	}
	HAL_Delay(delay);
	return reset_LEDs(handle, blink_leds);
}

TLC5947_STATUS blink_LED(TLC5947_Handle_t *handle, uint16_t led_chan, uint32_t delay){
	TLC5947_STATUS status = set_LED(handle, led_chan);
	if(status != TLC5947_OK){
		return status;
	}
	HAL_Delay(delay);
	return reset_LED(handle, led_chan);
}

