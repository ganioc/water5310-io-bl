/*
 * iap.h
 *
 *  Created on: 2023 Feb 28
 *      Author: ruffman
 */

#ifndef INCLUDE_IAP_IAP_H_
#define INCLUDE_IAP_IAP_H_

#include "at32f415_board.h"
#include "flash.h"

#define PARMETER_SIZE     10
#define FLASH_END         0x08040000
#define PARAMETER_START   (FLASH_END - (0x800*PARMETER_SIZE))

#define IO_PARAM_START     PARAMETER_START
#define ADC_PARAM_START     (PARAMETER_START + SECTOR_SIZE)

#define UPDATE_PARAM_START  (PARAMETER_START + 2 * SECTOR_SIZE)

/* app starting address */
#define APP_START_ADDR          0x08008000

/* the previous sector of app starting address is iap upgrade flag */
#define IAP_UPGRADE_FLAG_ADDR    (APP_START_ADDR - 0x800)

/* when app received cmd 0x5aa5 from pc-tool, will set up the flag,
indicates that an app upgrade will follow, see iap application note for more details */
// ATK8
#define IAP_UPGRADE_FLAG         0x41544B38

#define HALF_APP_SIZE            (102*1024)

/*  APP STORAGE ADDR   */
#define APP_STORE_ADDR           (APP_START_ADDR + HALF_APP_SIZE)


#define UPDATE_UNNECESSARY      0x0000
#define UPDATE_TO_APP_START     0xFFFF
#define UPDATE_TO_APP_STORE     0xFFF0
#define UPDATE_TO_APP_COPY      0x0001


typedef void (*iapfun)(void);

uint16_t get_update_param();


error_status write_to_app_store(uint8_t index, uint8_t* buf);
error_status write_to_app_start(uint8_t index, uint8_t* buf);
void app_load(uint32_t app_addr);


#endif /* INCLUDE_IAP_IAP_H_ */
