/*
 * eeprom.c
 *
 *  Created on: 2023 Feb 28
 *      Author: ruffman
 */
#include "iap.h"

void read_io_param(){
	uint16_t io_param;
	flash_read(IO_PARAM_START, &io_param, 1);
	printf("io_param:%02x\r\n", io_param);

}

void save_io_param(){
	uint16_t io_param = 18;
	flash_write(IO_PARAM_START, &io_param, 1);
	printf("save io_param\r\n");
}
