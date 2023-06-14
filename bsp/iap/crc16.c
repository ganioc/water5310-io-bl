/*
 * crc16.c
 *
 *  Created on: 2023 Feb 28
 *      Author: ruffman
 */


/**
 * @brief  Update CRC16 for input byte
 * @param  CRC input value
 * @param  input byte
 * @retval None
 */

#include "crc16.h"


static uint16_t UpdateCRC16(uint16_t crcIn, uint8_t byte) {
	uint32_t crc = crcIn;
	uint32_t in = byte | 0x100;
	do {
		crc <<= 1;
		in <<= 1;
		if (in & 0x100)
			++crc;
		if (crc & 0x10000)
			crc ^= 0x1021;
	} while (!(in & 0x10000));
	return crc & 0xffffu;
}
/**
 * @brief  Cal CRC16 for YModem Packet
 * @param  data
 * @param  length
 * @retval None
 */
uint16_t Cal_CRC16(const uint8_t* data, uint32_t size) {
	uint32_t crc = 0;
	const uint8_t* dataEnd = data + size;
	while (data < dataEnd)
		crc = UpdateCRC16(crc, *data++);

	crc = UpdateCRC16(crc, 0);
	crc = UpdateCRC16(crc, 0);
	return crc & 0xffffu;
}


/**
 * @brief  Convert a string to an integer
 * @param  p_inputstr: The string to be converted
 * @param  p_intnum: The integer value
 * @retval 1: Correct
 *         0: Error
 */
uint32_t Str2Int(uint8_t *p_inputstr, uint32_t *p_intnum) {
	uint32_t i = 0U, res = 0U;
	uint32_t val = 0U;

	if ((p_inputstr[0] == '0')
			&& ((p_inputstr[1] == 'x') || (p_inputstr[1] == 'X'))) {
		i = 2U;
		while ((i < 11U) && (p_inputstr[i] != '\0')) {
			if (ISVALIDHEX(p_inputstr[i])) {
				val = (val << 4U) + CONVERTHEX(p_inputstr[i]);
			} else {
				/* Return 0, Invalid input */
				res = 0U;
				break;
			}
			i++;
		}

		/* valid result */
		if (p_inputstr[i] == '\0') {
			*p_intnum = val;
			res = 1U;
		}
	} else /* max 10-digit decimal input */
	{
		while ((i < 11U) && (res != 1U)) {
			if (p_inputstr[i] == '\0') {
				*p_intnum = val;
				/* return 1 */
				res = 1U;
			} else if (((p_inputstr[i] == 'k') || (p_inputstr[i] == 'K'))
					&& (i > 0U)) {
				val = val << 10U;
				*p_intnum = val;
				res = 1U;
			} else if (((p_inputstr[i] == 'm') || (p_inputstr[i] == 'M'))
					&& (i > 0U)) {
				val = val << 20U;
				*p_intnum = val;
				res = 1U;
			} else if (ISVALIDDEC(p_inputstr[i])) {
				val = val * 10U + CONVERTDEC(p_inputstr[i]);
			} else {
				/* return 0, Invalid input */
				res = 0U;
				break;
			}
			i++;
		}
	}
	return res;
}
