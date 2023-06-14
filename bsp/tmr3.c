/*
 * tmr3.c
 *
 *  Created on: 2023 Feb 28
 *      Author: ruffman
 */

#include "at32f415_board.h"

void init_tmr3(void)
{
  crm_clocks_freq_type crm_clocks_freq_struct = {0};

  crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);

  /* enable the tmr3 global interrupt */
  nvic_irq_enable(TMR3_GLOBAL_IRQn, 1, 0);

  /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);

  /* time base configuration */
  tmr_base_init(TMR3, 10000, crm_clocks_freq_struct.ahb_freq / 10000);
  tmr_cnt_dir_set(TMR3, TMR_COUNT_UP);

  /* overflow interrupt enable */
  tmr_interrupt_enable(TMR3, TMR_OVF_INT, TRUE);

  /* enable tmr */
  tmr_counter_enable(TMR3, TRUE);
}

// To check error periodically
void TMR3_GLOBAL_IRQHandler(void)
{
  if(tmr_flag_get(TMR3, TMR_OVF_FLAG) == SET)
  {
    tmr_flag_clear(TMR3, TMR_OVF_FLAG);
    at32_led_toggle(LED4);

//    if(get_data_from_usart_flag)
//    {
//      if((++time_ira_cnt) == 0x00)
//        time_ira_cnt = 0xFF;
//      if(time_ira_cnt > 2)
//        back_err();
//      if(time_ira_cnt > 5)
//        crm_periph_clock_enable(LED1_GPIO_CRM_CLK, FALSE);
//    }
  }
}
