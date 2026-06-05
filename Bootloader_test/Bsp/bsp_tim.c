#include "bsp_tim.h"

void TMR2_Configuration(confirm_state sate)
{
	tmr_cnt_dir_set(TMR2, TMR_COUNT_UP);
	tmr_clock_source_div_set(TMR2, TMR_CLOCK_DIV1);
	tmr_period_buffer_enable(TMR2, FALSE);
	tmr_base_init(TMR2, 4999, 191);
	
	tmr_sub_sync_mode_set(TMR2, FALSE);
	tmr_primary_mode_select(TMR2, TMR_PRIMARY_SEL_RESET);
	
	tmr_counter_enable(TMR2, sate);
	
	tmr_interrupt_enable(TMR2,TMR_OVF_INT, sate);
}

//void TMR2_GLOBAL_IRQHandler(void)
//{
//	if(tmr_interrupt_flag_get(TMR2, TMR_OVF_FLAG) != RESET)
//	{
//		
//		tmr_flag_clear(TMR2, TMR_OVF_FLAG);
//	}
//}
