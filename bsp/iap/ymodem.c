/*
 * ymodem.c
 *
 *  Created on: 2023 Feb 28
 *      Author: ruffman
 */

#include "ymodem.h"
#include "at32f415_board.h"
#include "crc16.h"
#include "iap.h"

#define     USE_DEBUG_YMODEM

uint16_t aPacketSize = 0;
uint8_t aPacketData[PACKET_1K_SIZE +
						   PACKET_DATA_INDEX +
						   PACKET_TRAILER_SIZE];
static uint8_t file_name[FILE_NAME_LENGTH];
static uint8_t file_size[FILE_SIZE_LENGTH];
static uint8_t eeprom_buf[SECTOR_SIZE];
//uint8_t* eeprom[16];



uint16_t packet_size;
PacketTypeDef  packet_type;

// printRXBuf
void printRXBuf(uint8_t *buffer, int len) {
//	char str[64];
//	char temp[8];
//    printf("printRxBuf: %d\r\n" , len);
//    int upper;
//
//
//    for (int i = 0; i < len; i += 16) {
//      printf("%02x: ", i);
//
//      upper = (len < i + 16) ? len : i + 16;
//
//      for (int j = i; j < upper; j++) {
//          printf("%02x ", buffer[j]);
//      }
//      printf("\r\n");
//    }
}

COM_StatusTypeDef UART_ReceiveEx(uint8_t *pData, uint16_t size){
	uint8_t val;
	uint16_t index=0, crc , crc2;
	uint16_t counter = UART_RX_COUNTER;

	while(counter--){
		//delay_ms(UART_DELAY_DUR);
		if(aPacketSize > 0){
			break;
		}
		delay_ms(UART_DELAY_DUR);
	}
	if(counter <= 0){
		return COM_TIMEOUT;
	}
//	printf("\r\n<--\r\n");
	printf("\r\n< -- aPacketSize: %d\r\n", aPacketSize);
	printRXBuf(pData, aPacketSize);

	val = pData[0];

	switch(val){
	case SOH: packet_size = 128;
		packet_type = PACKET_SOH;
		printf("PACKET_SOH\r\n");
		break;
	case STX: packet_size = 1024;
		packet_type = PACKET_STX;
		printf("PACKET_STK\r\n");
		break;
	case EOT: packet_size = 1;
		packet_type = PACKET_EOT;
		printf("PACKET_EOT\r\n");
		break;
	case CA:
		if(pData[1] == CA){
			packet_size = 2;
			packet_type = PACKET_CA;
			printf("PACKET_CA\r\n");
		}else{
			return COM_ERROR;
		}
		break;
	case ABORT1:
	case ABORT2:
		return COM_BUSY;
		break;
	default:
		return COM_ERROR;
		break;
	}

	if(packet_size >= 128){
		if(pData[1] != (0xFF - pData[2])){
			printf("packet id error\r\n");
			packet_size = 0;
			return COM_ERROR;
		}else{// compute CRC
			crc = pData[packet_size + 3] << 8;
			crc += pData[packet_size + 4];
			crc2 = Cal_CRC16(&pData[3], packet_size);
			//printf("crc: %02x, crc2: %02x\r\n", crc, crc2);

			if(crc != crc2){
				printf("crc calc not match!\r\n");
				packet_size = 0;
				return COM_ERROR;
			}else{
				//printf("crc match!\r\n");
			}
		}
	}

	return COM_OK;

}
void extract_file_name_size(uint8_t *buffer){
	  uint16_t index = 3;

	  uint16_t i = 0;

	  while(buffer[index] != 0){
	    file_name[i++] = buffer[index++];
	  }
	  file_name[i] = 0;

	  i = 0;
	  index++;
	  while(buffer[index] != 0){
	    file_size[i++] = buffer[index++];
	  }
	  file_size[i] = 0;

//	  printf("file name, file size:\r\n");
//	  printf("%s\r\n", file_name);
//	  printf("%s\r\n", file_size);
}

void send_response(uint8_t ch, int len){
	printf("resp: -->\r\n");
	uint8_t temp[3];
	int i;
	for(i=0;i<len;i++){
		temp[i] = ch;
//		printf("%02x ", temp[i]);
	}
//	printf("\r\n");
	data_send(temp,len);
}

COM_StatusTypeDef SerialDownload(){
	uint8_t session_done = 0;
	COM_StatusTypeDef  result = COM_OK, pkt_result;
	uint8_t session_begin = 0;
	uint16_t blocks = 0;
	uint8_t  eeprom_index = 0;
	uint8_t  eeprom_pos_index = 0;
	error_status eeprom_status;
	uint16_t  block_length = 0;


	while (session_done == 0 && result == COM_OK) {
		uint16_t packets_received = 0;
		uint16_t file_done = 0;
		uint16_t inRx_block0 = 0;
		uint16_t inEOT = 0;
		uint16_t errors = 0;

		while (file_done == 0 && result == COM_OK) {
			aPacketSize = 0;

			pkt_result = UART_ReceiveEx(aPacketData, 1024);
			//printf("pkt_result: %d\r\n", pkt_result);

			if (pkt_result == COM_OK) {
				//printf("RX valid packet\r\n");
				errors = 0;

				switch(packet_type){
				case PACKET_CA:
					send_response(ACK,1);
					result = COM_ABORT;
					break;
				case PACKET_EOT:
					if(inEOT == 0){
						send_response(NAK,1);
						inEOT = 1;
					}else if(inEOT == 1){
						send_response(ACK, 1);
						delay_ms(50);
						send_response(CRC16, 1);
					}
					break;
				default:
//					printf("\r\nNormal packet: %02x\r\n", aPacketData[1]);
//					printf("packets received: %d\r\n", packets_received);

					if(packet_type == PACKET_SOH){
						block_length = 128;
					}else{
						block_length = 1024;
					}


					if((aPacketData[1] != (packets_received%256)) && inEOT == 0){
						printf("Packet id not matching, %d != %d\r\n",
								aPacketData[1], packets_received%256);
						send_response(NAK, 1);
					}else{
						// first block , block 0
						if(packets_received == 0 && inRx_block0 == 0){
							// file name extraction
							// file size extraction
							printf("Received 1st block 0");
							//printRXBuf(aPacketData, 128 + 5);
							extract_file_name_size(aPacketData);

							// printf("erase the blank sector\r\n");
							send_response(ACK, 1);
							send_response(CRC16	, 1);
							inRx_block0 = 1;
						}else if(packets_received != 0 && inRx_block0 == 1 && inEOT == 1){
							printf("Received last block 0, meaning END\r\n");
							printf("Copy eeprom to flash: %d\r\n", eeprom_pos_index);
							eeprom_status =  write_to_app_start(eeprom_pos_index, eeprom_buf);
							printf("eeprom_status :%d\r\n", eeprom_status);

							send_response(ACK, 1);
							file_done = 1;
							session_done = 1;
							return COM_OK;

						}else{ // Other blocks , data packets
							// 128 bytes block
							//1024 bytes block
							// printf("write to EEPROM\r\n");
							//printRXBuf(aPacketData, 128 + 5);
							blocks++;
							printf("blocks:%d\r\n", blocks);

							// copy block *4 into eeprom_buf,
							// and then save it to the flash;
							printf("eeprom_index: %d\r\n", eeprom_index);

							for(int i = 0; i< block_length; i++){
								eeprom_buf[eeprom_index*block_length + i] = aPacketData[3+i];
							}

							eeprom_index++;

							if(eeprom_index >= (SECTOR_SIZE/block_length)){
								// save to eeprom
								LED_INDICATION();
								printf("copy eeprom to flash: %d\r\n", eeprom_pos_index);

								eeprom_status = write_to_app_start(eeprom_pos_index, eeprom_buf);

								printf("eeprom_status :%d\r\n", eeprom_status);

								eeprom_index = 0;
								eeprom_pos_index++;

							}

							send_response(ACK, 1);
						}
						packets_received++;
						printf("received: %d\r\n", packets_received);
						session_begin = 1;
					}
					break;
				}

			} else if (pkt_result == COM_BUSY) {
				send_response(CA, 2);
				result = COM_ABORT;
			} else {
				if (session_begin > 0) {
					errors++;
				}
				if (errors > MAX_ERRORS) {
					send_response(CA, 2);
					result = COM_ABORT;
					return result;
				} else {
					printf("Wait for host response ...\r\n");
					send_response(CRC16, 1);
				}
			}
		}
	}
	return result;
}
