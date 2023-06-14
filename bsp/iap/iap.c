/*
 * iap.c
 *
 *  Created on: 2023 Feb 28
 *      Author: ruffman
 */

#include "at32f415_board.h"
#include "iap.h"
#include "flash.h"

iapfun jump_to_app;

uint16_t get_update_param(){
	uint16_t val;
	flash_read(IAP_UPGRADE_FLAG_ADDR, &val, 1);
	return val;
}
// 0 is failure
error_status set_update_param(uint16_t flag){
	return flash_write(IAP_UPGRADE_FLAG_ADDR, &flag, 1);
}

error_status write_to_app_store(uint8_t index, uint8_t* pbuffer){
	// APP_STORE_ADDR
	// return flash_write(APP_STORE_ADDR + index*SECTOR_SIZE, (uint16_t *)buf, SECTOR_SIZE/2);
	flash_2kb_write(APP_STORE_ADDR + index*SECTOR_SIZE , pbuffer);

	return SUCCESS;
}


error_status write_to_app_start(uint8_t index, uint8_t* pbuffer){
	// APP_STORE_ADDR
	// return flash_write(APP_START_ADDR + index*SECTOR_SIZE, (uint16_t *)buf, SECTOR_SIZE/2);
	flash_2kb_write(APP_START_ADDR + index*SECTOR_SIZE , pbuffer);

	return SUCCESS;
}

/**
  * @brief  app load.
  * @param  app_addr
  *         app code starting address
  * @retval none
  */
void app_load(uint32_t app_addr)
{

	printf("app_addr - %0X\r\n", (*(uint32_t*)app_addr));

  /* check the address of stack */
  if(((*(uint32_t*)app_addr) - 0x20000000) < (SRAM_SIZE * 1024))
  {
	  printf("in app_load\r\n");

    /* disable periph clock */
    crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, FALSE);
    crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, FALSE);
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, FALSE);

    /* disable nvic irq and clear pending */
    nvic_irq_disable(USART1_IRQn);
    nvic_irq_disable(TMR3_GLOBAL_IRQn);
    __NVIC_ClearPendingIRQ(USART1_IRQn);
    __NVIC_ClearPendingIRQ(TMR3_GLOBAL_IRQn);

    jump_to_app = (iapfun)*(uint32_t*)(app_addr + 4);        /* code second word is reset address */
    __set_MSP(*(uint32_t*)app_addr);                        /* init app stack pointer(code first word is stack address) */
    jump_to_app();                                          /* jump to user app */
  }
}
