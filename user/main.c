/**
 **************************************************************************
 * @file     main.c
 * @version  v2.0.5
 * @date     2022-05-20
 * @brief    main program
 **************************************************************************
 *                       Copyright notice & Disclaimer
 *
 * The software Board Support Package (BSP) that is made available to
 * download from Artery official website is the copyrighted work of Artery.
 * Artery authorizes customers to use, copy, and distribute the BSP
 * software and its related documentation for the purpose of design and
 * development in conjunction with Artery microcontrollers. Use of the
 * software is governed by this copyright notice and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
 * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
 * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
 * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
 * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
 *
 **************************************************************************
 */

#include "at32f415_board.h"
#include "at32f415_clock.h"

#include "flash.h"
#include "iap.h"
#include "ymodem.h"

/** @addtogroup AT32F415_periph_template
 * @{
 */

/** @addtogroup 415_LED_toggle LED_toggle
 * @{
 */

#define DELAY                            100
#define FAST                             1
#define SLOW                             4

uint8_t g_speed = FAST;

extern usart_group_type usart_group_struct;

void button_exint_init(void);
void button_isr(void);

/**
 * @brief  configure button exint
 * @param  none
 * @retval none
 */
void button_exint_init(void) {
	exint_init_type exint_init_struct;

	crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
	gpio_exint_line_config(GPIO_PORT_SOURCE_GPIOA, GPIO_PINS_SOURCE0);

	exint_default_para_init(&exint_init_struct);
	exint_init_struct.line_enable = TRUE;
	exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
	exint_init_struct.line_select = EXINT_LINE_0;
	exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
	exint_init(&exint_init_struct);

	nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
	nvic_irq_enable(EXINT0_IRQn, 0, 0);
}

/**
 * @brief  button handler function
 * @param  none
 * @retval none
 */
void button_isr(void) {
	/* delay 5ms */
	delay_ms(5);

	/* clear interrupt pending bit */
	exint_flag_clear(EXINT_LINE_0);

	/* check input pin state */
	if (SET == gpio_input_data_bit_read(USER_BUTTON_PORT, USER_BUTTON_PIN)) {
		if (g_speed == SLOW)
			g_speed = FAST;
		else
			g_speed = SLOW;
	}
}

/**
 * @brief  exint0 interrupt handler
 * @param  none
 * @retval none
 */
void EXINT0_IRQHandler(void) {
	button_isr();
}


/**
 * @brief  main function.
 * @param  none
 * @retval none
 */
int main(void) {
	uint16_t tag;
	COM_StatusTypeDef result;
	int target_delay_times = 3;

	system_clock_config();

	at32_board_init();

	print_welcome();

	printf("flash size :%d\r\n", FLASH_SIZE);

	delay_ms(500);

	while (target_delay_times > 0) {

		if (RESET == gpio_input_data_bit_read(IAP_PORT, IAP_PIN)){
			printf("iap port: 0\r\n");
			break;

		}else{
			target_delay_times--;
			printf("iap port: 1\r\n");
			delay_ms(10);
		}
	}

	if(target_delay_times == 0){
		result = SerialDownload();
		printf("result of donwload serial\r\n", result);
		if (result == COM_OK) {
			printf("Download succeed\r\n");
		}else{
			printf("Download failed\r\n");
		}
	}

	/* check app starting address whether 0x08xxxxxx */
	if (((*(uint32_t*) (APP_START_ADDR + 4)) & 0xFF000000) == 0x08000000){
		printf("Jump to app\r\n");
		app_load(APP_START_ADDR);
	}else{
		printf("Can not jump to app\r\n");
	}

	while (1) {
		printf("Still in bootloader\r\n");
		at32_led_toggle(LED4);
		delay_ms(1000);
	}
}

/**
 * @}
 */

/**
 * @}
 */
