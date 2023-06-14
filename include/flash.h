/*
 * flash.h
 *
 *  Created on: 2023 Feb 28
 *      Author: ruffman
 */

#ifndef INCLUDE_FLASH_H_
#define INCLUDE_FLASH_H_

#include "at32f415_board.h"

#define FLASH_SIZE    (*((uint32_t*)0x1FFFF7E0))
/* read from at32 flash capacity register(unit:kbyte) */
#define SRAM_SIZE                       224
/* sram size, unit:kbyte */

#define SECTOR_SIZE                     2048





void flash_2kb_write(uint32_t write_addr, uint8_t *pbuffer);
flag_status flash_upgrade_flag_read(void);

void flash_read(uint32_t read_addr, uint16_t *p_buffer, uint16_t num_read);

error_status flash_write_nocheck(uint32_t write_addr, uint16_t *p_buffer, uint16_t num_write);

error_status flash_write(uint32_t write_addr,uint16_t *p_Buffer, uint16_t num_write);


#endif /* INCLUDE_FLASH_H_ */
