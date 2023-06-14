/*
 * iap_gpio.c
 *
 *  Created on: 2023 Mar 2
 *      Author: ruffman
 */


#include "at32f415_board.h"

void init_iap_gpio(){
	init_gpio_input(
			IAP_CRM_CLOCK,
			IAP_PORT,
			IAP_PIN,
			GPIO_PULL_DOWN);
}

//uint8_t get_iap_gpio_state(){
//
//}
