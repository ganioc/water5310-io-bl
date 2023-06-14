/*
 * crc16.h
 *
 *  Created on: 2023 Feb 28
 *      Author: ruffman
 */

#ifndef INCLUDE_IAP_CRC16_H_
#define INCLUDE_IAP_CRC16_H_

#include "at32f415_board.h"

uint16_t Cal_CRC16(const uint8_t* data, uint32_t size);

uint32_t Str2Int(uint8_t *p_inputstr, uint32_t *p_intnum);

#endif /* INCLUDE_IAP_CRC16_H_ */
