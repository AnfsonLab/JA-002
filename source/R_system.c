#include "r_cg_macrodriver.h"
#include "r_cg_adc.h"
#include "r_cg_userdefine.h"
#include "R_system.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
uint16_t	ms_counter;

flag_t	_current_sys_op_status_;

int16_t	COMP_SET_TEMP_ON,COMP_SET_TEMP_OFF,COMP_REAL_TEMP,HEATER_START_TEMP,HEATER_STOP_TEMP;

uint8_t	ERROR_CODE=0,comp_over_heat = RESET,FLOW_SWITCH_PROTECT_ON=0,ANTIFRZ_PROTECT_ON=0;

int16_t	comp_protect_timer;
int16_t	PUMP_DELAY;
int16_t	one_sec_counter,zero_point_one_sec;
int16_t	COMP_on_count,PUMP_on_count;

int16_t	FLOW_SWITCH_PROTECT_DELAY,LP_SENSOR_DELAY,CONDENSER_PROTECT_DELAY;

int8_t 	COMP_TOP_TEMP_PROTECT_DELAY,COMP_OVERLOAD_PROTECT_DELAY,FAN_OVERLOAD_PROTECT_DELAY,
		HP_SENSOR_DELAY,ANTIFRZ_DELAY;
int8_t 	PUMP_OVERLOAD_PROTECT_DELAY,WATERLEVER_LOW_ALARM_DELAY,
		WATERLEVER_HIGH_ALARM_DELAY,WATER_HIGH_TEMP_STOP_DELAY,WATER_LOW_TEMP_STOP_DELAY,
		WATER_HIGH_TEMP_ALARM_DELAY,WATER_LOW_TEMP_ALARM_DELAY;
int8_t	POWER_PHASE_PROTECT_DELAY,HEATER_TEMP_PROTECT_DELAY,REMOTE_DEBOUNCE_DELAY,REMOTE_DEBOUNCE_DELAY_1;
int8_t	CONTROL_TEMP_DELAY,BASE_TEMP_DELAY,CONDENSER_TEMP_DELAY,ANTIFRZ_TEMP_DELAY,COMP_OUT_SENSOR_DELAY;

int8_t	water_lever_error_flag_high=0,water_lever_error_flag_low=0,water_temp_error_flag_high=0,water_temp_error_flag_low=0;
int8_t	CURRENT_DETECT_DELAY;

uint8_t	system_sensor_status_on = 1;
int8_t	system_on_fan_comp_flag;
uint16_t	current_data_temp,voltage_check_data;

void	sensor_operation(void);
void	system_operation(void);
void	sensor_operation_continue(void);
void	system_sensor_status(void);

extern	uint8_t	state_code_backup_count;


/*******************************************************************************
* Function name	: turn_system_on
* Description 	: none
* Argument 	: none
* Return value  : none
*******************************************************************************/
/*******************************************************************************
End of function turn_system_on
*******************************************************************************/
void	turn_system_on(void)
{
	current_sys_op_power = SET;
	if(PUMP_RLY == RESET)
		PUMP_DELAY = (int16_t)eeprom_option_byte[_System_on_PUMP_delay]*step_10_sec;
}
/*******************************************************************************
End of function turn_system_on
*******************************************************************************/



/*******************************************************************************
* Function name	: turn_system_off
* Description 	: none
* Argument 	: none
* Return value  : none
*******************************************************************************/
void	turn_system_off(void)
{
	if(current_sys_op_power == SET)
	{
		current_sys_op_power = RESET;
	}
}

/*******************************************************************************
End of function turn_system_off
*******************************************************************************/
/*******************************************************************************
* Function name : system_sensor_status
* Description	: none
* Argument	: none
* Return value	: none
*******************************************************************************/
void	system_sensor_status(void)
{
	switch(ERROR_CODE)
	{
		case 0x00:
		case 0xd1:	
		case 0xd3:
		case 0xd6:
		case 0xd7:
		case 0xc4:
		case 0xc6:
		case 0xc7:
			system_sensor_status_on = 1;
			break;
		default:
			system_sensor_status_on = 0;
			break;
	}

}
/*******************************************************************************
End of function turn_system_on
*******************************************************************************/


/*******************************************************************************
* Function name	: power_on_timer_reset
* Description 	: none
* Argument 	: none
* Return value  : none
*******************************************************************************/
void	power_on_timer_reset(void)
{
	
	PUMP_DELAY =0;
	comp_protect_timer = 0;
	FLOW_SWITCH_PROTECT_DELAY =0;
	PUMP_OVERLOAD_PROTECT_DELAY =0;
	WATERLEVER_LOW_ALARM_DELAY =0;
	WATERLEVER_HIGH_ALARM_DELAY = 0;
	WATER_HIGH_TEMP_STOP_DELAY = 0;
	WATER_LOW_TEMP_STOP_DELAY = 0;
	WATER_HIGH_TEMP_ALARM_DELAY = 0;
	WATER_LOW_TEMP_ALARM_DELAY = 0;
	POWER_PHASE_PROTECT_DELAY = 0;
	HEATER_TEMP_PROTECT_DELAY = 0;
	HP_SENSOR_DELAY = 0;
	HP_SENSOR_DELAY =0;
	LP_SENSOR_DELAY =0;
	ANTIFRZ_DELAY =0;
	REMOTE_DEBOUNCE_DELAY = 0;
	deice_on_flag = RESET;
//	current_sys_op_power = RESET;
	remote_on_off_flag = RESET;
	
	one_sec_counter = 1000;
	zero_point_one_sec = 1000;
		
}
/*******************************************************************************
End of functiontimer_counter_down
*******************************************************************************/





/*******************************************************************************
* Function name : comp_protect_operation
* Description	: none
* Argument	: none
* Return value	: none
*******************************************************************************/
void	comp_protect_operation(void)
{
	if(COMP_OUT_TEMP > 1100)
	{
		if(comp_over_heat == RESET)
		{
			error_code_backup = 0Xff;
			COMP_RLY = RESET;
			comp_protect_timer =  (int16_t)eeprom_option_byte[_System_off_COMP_protect_timer_set]*step_10_sec;
			comp_over_heat = SET;
			ERROR_RLY = SET;
			ERROR_CODE =  0xc7;
		}
		else
		{
			if(comp_protect_timer <= 5)
				comp_protect_timer =  5;
		}
		
	}
	else
	{
		if(comp_over_heat == SET)
		{
			if(comp_protect_timer <= 5)
				comp_protect_timer =  5;
		}
		if(COMP_OUT_TEMP  <=950)
		{
			if(ERROR_CODE ==  0xc7)
			{
				ERROR_CODE =  00;
				ERROR_RLY = RESET;
			}
			comp_over_heat = RESET;
		}
		
	}
}




/*******************************************************************************
* Function name : timer_counter_down
* Description	: none
* Argument	: none
* Return value	: none
*******************************************************************************/
void	timer_counter_down(void)
{
/* *** PWM control *********************************************************************** */

	//   ms_counter++;

/* ********  0.1 sec operation ****************************************************************** */
#if 1     
// for testtt system operation with check error
	if(zero_point_one_sec -- <= 0)
	{
		zero_point_one_sec = 100;
		if((production_mode == RESET) && (LED_mode_count != _LED_mode_option))
		{
			system_sensor_status();
			if(system_sensor_status_on)
			{
				sensor_operation_continue();
				sensor_operation(); 
			}
		}
	}
#endif
	
	if( one_sec_counter -- <= 0)
	{
		one_sec_counter = 1000;

		if((production_mode == RESET) && (LED_mode_count != _LED_mode_option))
		{
			system_sensor_status();
			if(system_sensor_status_on)
			{
				system_operation();
			}


			system_sensor_status();
			if(system_sensor_status_on)
			{
				comp_protect_operation();
				if(comp_over_heat == RESET)
				{
					system_operation();
				}
			}
		}

		if(COMP_RLY)
		{
			if(COMP_on_count ++ >= 360)
				COMP_on_count = 360;
		}
		else
		{
			COMP_on_count = 0;
		}
		
		if(PUMP_on_count ++ >= 10)
			PUMP_on_count = 10;
		
		if(error_code_backup_count < ERROR_BACKUP_SET)
		{
			error_code_backup_count++;
		}

		
		if(state_code_backup_count < STATE_BACKUP_SET)
		{
			state_code_backup_count++;
		}

		if(PUMP_DELAY != 0 )
		{
			PUMP_DELAY --;
		}
		
		if(comp_protect_timer != 0 )
		{
			comp_protect_timer--;
		}
	}
}
/*******************************************************************************
End of functiontimer_counter_down
*******************************************************************************/


/*******************************************************************************
* Function name	: system_operation
* Description 	: none
* Argument 	: none
* Return value  : none
*******************************************************************************/
void	system_operation(void)
{
	/*****************   system on operation  ****************/

	if(TEMP_SELECT_SWITCH == FIX_TYPE_SENSOR)
	{
		COMP_REAL_TEMP = CONTROL_TEMP + (int16_t)eeprom_option_byte[_Control_sensor_offset];
		COMP_SET_TEMP_OFF = SET_TEMP;
		if(COMP_SET_TEMP_OFF < 0)
			COMP_SET_TEMP_OFF = 0;
		COMP_SET_TEMP_ON = SET_TEMP + (int16_t)eeprom_option_byte[_COMP_start_temp_offset];
	}
	else
	{
		COMP_REAL_TEMP = (CONTROL_TEMP + (int16_t)eeprom_option_byte[_Control_sensor_offset])
							- (BASE_TEMP + (int16_t)eeprom_option_byte[_Base_sensor_offset]);
		
		COMP_SET_TEMP_OFF = SET_TEMP;
		COMP_SET_TEMP_ON = SET_TEMP + (int16_t)eeprom_option_byte[_COMP_start_temp_offset];
		
	}
	 
	/*****************	 system on operation  ****************/

	if(current_sys_op_power)     
	{
		if(PUMP_RLY == RESET)
		{
			if(PUMP_DELAY  <= 0 )
			{
				PUMP_RLY = SET; 
				PUMP_on_count = 0;
				PUMP_DELAY = 0;
				if(comp_protect_timer <= 0)
					comp_protect_timer = eeprom_option_byte[_System_on_COMP_start_delay]*step_10_sec;
			}
		}
		else 
		{
			comp_turn_on();
		}
	}
	else     /*****************   system off operation  ****************/
	{
		if(ERROR_RLY == SET)
			ERROR_RLY = RESET;
		comp_turn_off();
	}
	
}
/*******************************************************************************
End of function system_operation
*******************************************************************************/


/*******************************************************************************
* Function name	: comp_turn_on
* Description 	: none
* Argument 	: none
* Return value  : none
*******************************************************************************/
void	comp_turn_on(void)
{
	if(COMP_RLY == RESET)
	{ 
		if(COMP_REAL_TEMP >= COMP_SET_TEMP_ON)
		{
			if(comp_protect_timer  <= 0)      
       		{
       			comp_protect_timer = 0;
    			COMP_RLY = SET;
				COMP_on_count = 0; 
			}
		}
	}
	else
	{
		if(comp_protect_timer <= 0)
		{
			comp_protect_timer = 0;
			if(COMP_REAL_TEMP <= COMP_SET_TEMP_OFF)    
           	{
        		COMP_RLY = RESET;
				comp_protect_timer = (int16_t)eeprom_option_byte[_Temp_control_COMP_protect_timer_set]*step_10_sec;
            }
		}
	}
}


/*******************************************************************************
* Function name	: com_turn_off
* Description 	: none
* Argument 	: none
* Return value  : none
*******************************************************************************/

void	comp_turn_off(void)
{
	if(COMP_RLY == SET)
	{
		COMP_RLY = RESET;
		comp_protect_timer =  (int16_t)eeprom_option_byte[_System_off_COMP_protect_timer_set]*step_10_sec;
		PUMP_DELAY = (int16_t)eeprom_option_byte[_System_off_PUMP_delay]*step_10_sec;
	}
	else
	{
		if(PUMP_RLY == SET)
		{
			if(PUMP_DELAY == 0)
			{
				PUMP_RLY= RESET;
			}
		}
	}
	
}
/*******************************************************************************
* Function name	: error_system_off
* Description 	: none
* Argument 	: none
* Return value  : none
*******************************************************************************/
void	error_system_off(void)
{
	COMP_RLY = RESET;
	ERROR_RLY = SET;
	error_code_backup = 0Xff;
	turn_system_off();

}

/*******************************************************************************
* Function name	: error_system_off
* Description 	: none
* Argument 	: none
* Return value  : none
*******************************************************************************/
void	error_system_off_all(void)
{
	COMP_RLY = RESET;
	PUMP_RLY = RESET;
	ERROR_RLY = SET;
	error_code_backup = 0Xff;
	turn_system_off();

}

/*******************************************************************************
* Function name	: sensor_operation
* Description 	: none
* Argument 	: none
* Return value  : none
*******************************************************************************/
void	sensor_operation(void)
{

	if(TEMP_SELECT_SWITCH == FIX_TYPE_SENSOR)
	{
		COMP_REAL_TEMP = CONTROL_TEMP + (int16_t)eeprom_option_byte[_Control_sensor_offset];
		COMP_SET_TEMP_OFF = SET_TEMP;
		if(COMP_SET_TEMP_OFF < 0)
			COMP_SET_TEMP_OFF = 0;
	}
	else
	{
		COMP_REAL_TEMP = (CONTROL_TEMP + (int16_t)eeprom_option_byte[_Control_sensor_offset])
							- (BASE_TEMP + (int16_t)eeprom_option_byte[_Base_sensor_offset]);
		COMP_SET_TEMP_OFF = SET_TEMP;
	}

	/*****************   COMP_OVERLOAD_PROTECT operation  detect 1 sec ****************/
	//if(COMP_RLY == SET)
	{
		if(COMP_OVERLOAD_PROTECT == 0)
			COMP_OVERLOAD_PROTECT_DELAY = 0;
		else
			if(COMP_OVERLOAD_PROTECT_DELAY ++ >= SENSOR_DEBUNSE)
			{
				COMP_OVERLOAD_PROTECT_DELAY  = SENSOR_DEBUNSE;
				ERROR_CODE = 0xC1;
				error_system_off();
			}
	}
	/*****************   COMP_OVERLOAD_PROTECT_DELAY operation  ****************/

	/*****************   FAN_OVERLOAD_PROTECT operation  detect 1 sec   ****************/
	//if(COMP_RLY == SET)
	{
		if(FAN_OVERLOAD_PROTECT == 0)
			FAN_OVERLOAD_PROTECT_DELAY = 0;
		else
			if(FAN_OVERLOAD_PROTECT_DELAY ++ >= SENSOR_DEBUNSE)
			{
				FAN_OVERLOAD_PROTECT_DELAY = SENSOR_DEBUNSE;
				ERROR_CODE = 0xC2;
				error_system_off();
			}
	}
	/*****************   FAN_OVERLOAD_PROTECT sensor operation  ****************/
#if 0
	/*****************   HP sensor operation detect 1 sec   ****************/
	//if(COMP_RLY == SET)
	{
		if(HP_PROTECT == 0)
			HP_SENSOR_DELAY = 0;
		else
			if(HP_SENSOR_DELAY ++ >= SENSOR_DEBUNSE)
			{
				HP_SENSOR_DELAY  = SENSOR_DEBUNSE;
				ERROR_CODE = 0xC3;
				error_system_off();
			}	
	}
	/*****************   HP sensor operation  ****************/

	/*****************   LP sensor operation  detect after comp on  hold for 10s ~ 600s ****************/
	if((COMP_RLY == SET) && (COMP_on_count >= 180))
	{
		if(LP_PROTECT == 0)
		{
			LP_SENSOR_DELAY = 0;
		}
		else
		{
			if(LP_SENSOR_DELAY ++ >= (int16_t)eeprom_option_byte[_LP_start_check_time]*100)
			{
				LP_SENSOR_DELAY  = (int16_t)eeprom_option_byte[_LP_start_check_time]*100;
				ERROR_CODE = 0xC4;
				error_code_backup = 0Xff;
				COMP_RLY = RESET;
				comp_protect_timer =  (int16_t)eeprom_option_byte[_System_off_COMP_protect_timer_set]*step_10_sec;
				//error_system_off();
			}
		}	
	}
	else
	{
		if(ERROR_CODE == 0xC4)
		{
			comp_protect_timer =  (int16_t)eeprom_option_byte[_System_off_COMP_protect_timer_set]*step_10_sec;
		}
		if(LP_PROTECT == 0)
		{
			LP_SENSOR_DELAY = 0;
			if(ERROR_CODE == 0xC4)
			{
				ERROR_CODE = 00;
				comp_protect_timer =  (int16_t)eeprom_option_byte[_System_off_COMP_protect_timer_set]*step_10_sec;
			}
		}
	}
	/*****************   LP sensor operation  ****************/
#endif	
	/*****************  condenser high temp protect  check for 1 sec ****************/
	if(CONDENSER_TEMP   > (int16_t)eeprom_option_byte[_Condenser_temp_protect_value]*10 )
	{
		if(CONDENSER_PROTECT_DELAY ++ >= SENSOR_DEBUNSE)
		{
			CONDENSER_PROTECT_DELAY  = SENSOR_DEBUNSE;
			ERROR_CODE = 0xC5;
			error_system_off();
		}
	}
	else
	{
		CONDENSER_PROTECT_DELAY  = 0;
	}
	/*****************  condenser high temp protect  check for 1 sec  ****************/

	/*****************   PUMP overload  operation  check for hold 0.1sec ****************/
	//if(PUMP_RLY == SET) 
	{
		if(PUMP_OVERLOAD_PROTECT == 0)
			PUMP_OVERLOAD_PROTECT_DELAY = 0;
		else
			if(PUMP_OVERLOAD_PROTECT_DELAY++  >= SENSOR_DEBUNSE)     /* old 1*/
			{
				PUMP_OVERLOAD_PROTECT_DELAY  =SENSOR_DEBUNSE;
				ERROR_CODE = 0xd2;
				error_system_off_all();
			}
	}
	/*****************   PUMP overload    operation  ****************/

#if 0
	/*****************   WATER_HIGH_TEMP_STOP operation check for 10 sec 0-85 degree ****************/
	if((CONTROL_TEMP + (int16_t)eeprom_option_byte[_Control_sensor_offset]) > 
								(uint16_t)eeprom_option_byte[_Liquid_high_temp_stop]*5)
	{
		if(WATER_HIGH_TEMP_STOP_DELAY ++ >= 100)
		{
			WATER_HIGH_TEMP_STOP_DELAY  = 100;
			ERROR_CODE = 0xd4;
			error_system_off();
		}
	}
	else
	{
		WATER_HIGH_TEMP_STOP_DELAY  = 0;
	}
	/*****************  WATER_HIGH_TEMP_STOP operation  ****************/

	/*****************   WATER_LOW_TEMP_STOP operation check for 1 sec 0-85 degree c ****************/
	if((CONTROL_TEMP + (int16_t)eeprom_option_byte[_Control_sensor_offset])  <
								(uint16_t)eeprom_option_byte[_Liquid_low_temp_stop]*5)
	{
		if(WATER_LOW_TEMP_STOP_DELAY ++ >= SENSOR_DEBUNSE)
		{
			WATER_LOW_TEMP_STOP_DELAY  = SENSOR_DEBUNSE;
			ERROR_CODE = 0xd5;
			error_system_off();
		}
	}
	else
	{
		WATER_LOW_TEMP_STOP_DELAY  = 0;
	}
	/*****************  WATER_LOW_TEMP_STOP operation  ****************/

#endif

	/*****************   POWER_PHASE_PROTECT operation  ****************/
	if(POWER_PHASE_PROTECT == 0)
		POWER_PHASE_PROTECT_DELAY= 0;
	else
		if(POWER_PHASE_PROTECT_DELAY++ >= SENSOR_DEBUNSE)		/* old 1*/
		{
			POWER_PHASE_PROTECT_DELAY= SENSOR_DEBUNSE;
			ERROR_CODE = 0xe1;
			error_system_off_all();
		}
	/*****************   POWER_PHASE_PROTECT operation  ****************/
	/*****************   CONTROL SENSOR PROTECT  ****************/
	if((CONTROL_TEMP  <= -150) || (CONTROL_TEMP >= 900))
	{
		if(CONTROL_TEMP_DELAY ++ >= SENSOR_DEBUNSE)
		{
			CONTROL_TEMP_DELAY = SENSOR_DEBUNSE;
			ERROR_CODE = 0xe2;
			error_system_off();
		}
	}
	else
	{
		CONTROL_TEMP_DELAY = 0;
	}
	/*****************   CONTROL SENSOR PROTECT  ****************/
	/*****************   IW BASE SENSOR ERROR  ****************/
	if(TEMP_SELECT_SWITCH != FIX_TYPE_SENSOR)
	{
		if((BASE_TEMP <= -150) || (BASE_TEMP >= 900))
		{
			if(BASE_TEMP_DELAY ++ >= SENSOR_DEBUNSE)
			{
				BASE_TEMP_DELAY = SENSOR_DEBUNSE;
				ERROR_CODE = 0xe3;
				error_system_off();
			}
		}
		else
		{
			BASE_TEMP_DELAY = 0;
		}	
	}
	
	/*****************   IW BASE SENSOR ERROR  ****************/

	/*****************   IW CONDENSER SENSOR ERROR  ****************/
	if((CONDENSER_TEMP <= -150) || (CONDENSER_TEMP >= 900))
	{
		if(CONDENSER_TEMP_DELAY ++ >= SENSOR_DEBUNSE)
		{
			CONDENSER_TEMP_DELAY = SENSOR_DEBUNSE;
			ERROR_CODE = 0xe4;
			error_system_off();
		}
	}
	else
	{
		CONDENSER_TEMP_DELAY = 0;
	}	
	/*****************   IW BASE SENSOR ERROR  ****************/	
#if 0
	/*****************   IW ANTIFRZ SENSOR ERROR  ****************/
	if((ANTIFRZ_TEMP <= -150) || (ANTIFRZ_TEMP >= 900))
	{
		if(ANTIFRZ_TEMP_DELAY ++ >= SENSOR_DEBUNSE)
		{
			ANTIFRZ_TEMP_DELAY = SENSOR_DEBUNSE;
			ERROR_CODE = 0xe5;
			error_system_off();
		}
	}
	else
	{
		ANTIFRZ_TEMP_DELAY = 0;
	}	
	/*****************   IW ANTIFRZ SENSOR ERROR  ****************/
#endif
	/*****************	slim-type over current error  ****************/
	
	if((COMP_RLY == SET) && (COMP_on_count > 9) && (current_data_max != default_0A)) 
	{
		current_data_temp = current_data_max - default_0A ; 
		current_data_temp = current_data_temp * 7;
		current_data_temp = current_data_temp / 10;

		current_data_temp = current_data_temp +8;
//		if(current_data_temp < 380) 							// if current over 19A no need to wait for 1 sec
		{
			if(current_data_temp > (current_detect_data *2))	   // dip switch select current value
			{
				if(CURRENT_DETECT_DELAY++ >= 20)
				{
					CURRENT_DETECT_DELAY = 20;
					ERROR_CODE = 0xe8;
					error_system_off_all();
				}
			}
			else
			{
				CURRENT_DETECT_DELAY  = 0;
			}
		}
	}
	/*****************	 slim-type over current  ERROR	****************/
}
/*******************************************************************************
End of function system_operation
*******************************************************************************/



/*******************************************************************************
* Function name	: sensor_operation_continue
* Description 	: none
* Argument 	: none
* Return value  : none
*******************************************************************************/
void	sensor_operation_continue(void)
{

	if(TEMP_SELECT_SWITCH == FIX_TYPE_SENSOR)
	{
		COMP_REAL_TEMP = CONTROL_TEMP + (int16_t)eeprom_option_byte[_Control_sensor_offset];
		COMP_SET_TEMP_OFF = SET_TEMP;
		if(COMP_SET_TEMP_OFF < 0)
			COMP_SET_TEMP_OFF = 0;
	}
	else
	{
		COMP_REAL_TEMP = (CONTROL_TEMP + (int16_t)eeprom_option_byte[_Control_sensor_offset])
							- (BASE_TEMP + (int16_t)eeprom_option_byte[_Base_sensor_offset]);
		COMP_SET_TEMP_OFF = SET_TEMP;
	}

	/*****************   IW RESERVE_1  ****************/
//	if(REMOTE_SW_FUNCTION == RESET)
	{
		if(REMOTE_ON_OFF_SW == 0)
		{
			REMOTE_DEBOUNCE_DELAY_1 = 0;
			if(REMOTE_DEBOUNCE_DELAY ++ >= 10)
			{
				REMOTE_DEBOUNCE_DELAY = 10;
				if(remote_on_off_flag == 0)
				{
					if(current_sys_op_power==RESET)
		       			turn_system_on();
					remote_on_off_flag = 1;
				}
			}
		}
		else
		{
			REMOTE_DEBOUNCE_DELAY = 0;
			if(REMOTE_DEBOUNCE_DELAY_1 ++ >= 10)
			{
				REMOTE_DEBOUNCE_DELAY_1 = 10;
				if(remote_on_off_flag == 1)
				{
					if(current_sys_op_power==SET)
						turn_system_off();
					remote_on_off_flag = 0;
				}
			}
		}
	}
#if 0	
	/*****************   WATERLEVER_LOW_ALARM  operation  after 10 sec check for hold 5 sec ****************/
	if((PUMP_RLY == SET) && (PUMP_on_count >= 10) && (current_sys_op_power))
	{
		if(WATERLEVER_LOW_ALARM == 0)
		{
			WATERLEVER_LOW_ALARM_DELAY = 0;
			if ( water_lever_error_flag_low)
			{
				water_lever_error_flag_low = 0;
				ERROR_CODE = 0x00;
				ERROR_RLY = RESET;
			}
		}
		else
		{
			if(WATERLEVER_LOW_ALARM_DELAY++  >= 50)
			{
				WATERLEVER_LOW_ALARM_DELAY  =50;
				if ( water_lever_error_flag_low == 0)
					error_code_backup = 0Xff;
				ERROR_CODE = 0xd3;
#if ERROR_RLY_NORMAL
				ERROR_RLY = SET;
#endif
				// PMV_close();				// modify for V13 bug 
				water_lever_error_flag_low = 1;
			}
		}
	}
	/*****************   WATERLEVER_LOW_ALARM  operation  ****************/

	/*****************   WATER_HIGH_TEMP_ALARM operation check for 10 sec  ****************/
	if(current_sys_op_power)     
	{
		if(COMP_REAL_TEMP  >	(COMP_SET_TEMP_OFF +	(int16_t)eeprom_option_byte[_Liquid_high_temp_alarm]*10))
		{
			if(WATER_HIGH_TEMP_ALARM_DELAY ++ >= 100)
			{
				WATER_HIGH_TEMP_ALARM_DELAY  = 100;
				if ( water_temp_error_flag_high == 0)
					error_code_backup = 0Xff;
				ERROR_CODE = 0xd6;
#if ERROR_RLY_NORMAL				
				ERROR_RLY = SET;
#endif
				// PMV_close();				// modify for V13 bug 
				water_temp_error_flag_high = 1;
			}
		}
		else
		{
			if(COMP_REAL_TEMP  < (COMP_SET_TEMP_OFF +	(int16_t)eeprom_option_byte[_Liquid_high_temp_alarm]*10 - 5))
			{
				if ( water_temp_error_flag_high)
				{
					water_temp_error_flag_high = 0;
					ERROR_RLY = RESET;
					ERROR_CODE = 0x00;
				}	
			}
			WATER_HIGH_TEMP_ALARM_DELAY  = 0;
		}
	}
	/*****************  WATER_HIGH_TEMP_ALARM operation  ****************/

	/*****************   WATER_LOW_TEMP_ALARM operation check for 5 sec  ****************/
	if(current_sys_op_power)     
	{
		if(COMP_REAL_TEMP  <  (COMP_SET_TEMP_OFF + (int16_t)eeprom_option_byte[_Liquid_low_temp_alarm]*10))
		{
			if(WATER_LOW_TEMP_ALARM_DELAY ++ >= 50)
			{
				WATER_LOW_TEMP_ALARM_DELAY  = 50;
				if ( water_temp_error_flag_low == 0)
					error_code_backup = 0Xff;
				ERROR_CODE = 0xd7;
#if ERROR_RLY_NORMAL				
				ERROR_RLY = SET;
#endif
				// PMV_close();				// modify for V13 bug 
				water_temp_error_flag_low = 1;
			}
		}
		else
		{
			if(COMP_REAL_TEMP  >  (COMP_SET_TEMP_OFF + (int16_t)eeprom_option_byte[_Liquid_low_temp_alarm]*10 + 5))
			{
				WATER_LOW_TEMP_ALARM_DELAY  = 0;
				if ( water_temp_error_flag_low)
				{
					water_temp_error_flag_low = 0;
					ERROR_CODE = 0x00;
					ERROR_RLY = RESET;
				}
			}
		}
	}
	/*****************  WATER_LOW_TEMP_ALARM operation  ****************/
	/*****************   Antifrz  operation  Antifrz temp range 0-20, for 1 sec ****************/
	if(ANTIFRZ_PROTECT_ON != 1)
	{
		if(ANTIFRZ_TEMP <= 	(int16_t)eeprom_option_byte[_Antifrz_protect_stop]*10)
		{
			if(ANTIFRZ_DELAY ++ >= SENSOR_DEBUNSE)
			{
				ANTIFRZ_DELAY  = SENSOR_DEBUNSE;
				if ( ANTIFRZ_PROTECT_ON == 0)
					error_code_backup = 0Xff;
				ERROR_CODE = 0xC6;
				COMP_RLY = RESET;
				comp_protect_timer =  (int16_t)eeprom_option_byte[_System_off_COMP_protect_timer_set]*step_10_sec;
				ERROR_RLY = SET;
				ANTIFRZ_PROTECT_ON = 1;
				
			}
		}
		else
		{
			ANTIFRZ_DELAY  = 0;
		}
	}
	else
	{
		if(comp_protect_timer <= 5)
			comp_protect_timer =  5;
		if(ANTIFRZ_TEMP  >= (int16_t)eeprom_option_byte[_Antifrz_protect_stop]*10 + 20 )       // Antifrz protect off = _Antifrz_protect_stop + 2 degree
		{
			ERROR_CODE = 0x00;
			ERROR_RLY = RESET;
			ANTIFRZ_PROTECT_ON = 0;
		}
	}
	
	/*****************   Antifrz  operation  ****************/
#endif
	/*****************	 FLOW switch  operation  after 10 sec check for hold 0 - 40 s ****************/
#if 1
	if(FLOW_SWITCH_PROTECT_ON != 1)
	{
		if((PUMP_RLY == SET) && (PUMP_on_count >= 10))
		{
			if(FLOW_SWITCH_PROTECT == 0)
				FLOW_SWITCH_PROTECT_DELAY = 0;
			else
				if(FLOW_SWITCH_PROTECT_DELAY++ >= (int16_t)eeprom_option_byte[_Flow_switch_start_check_time]*10)
				{
					FLOW_SWITCH_PROTECT_DELAY  = (int16_t)eeprom_option_byte[_Flow_switch_start_check_time]*10;
					ERROR_CODE = 0xd1;
					//error_system_off();
					error_code_backup = 0Xff;
					COMP_RLY = RESET;
					comp_protect_timer =  (int16_t)eeprom_option_byte[_System_off_COMP_protect_timer_set]*step_10_sec;
					PUMP_RLY = RESET;
					PUMP_DELAY = (int16_t)eeprom_option_byte[_System_off_PUMP_delay]*step_10_sec;
					ERROR_RLY = SET;
					FLOW_SWITCH_PROTECT_ON = 1;
				}
		}
	}	
	else
	{
		if(FLOW_SWITCH_PROTECT == 0)
		{
			if(PUMP_RLY == RESET)
			{
				PUMP_RLY = SET;
				PUMP_on_count = 0;
			}
			else
			{
				if(PUMP_on_count >= 10)
				{
					ERROR_RLY = RESET;
					ERROR_CODE =  00;
					FLOW_SWITCH_PROTECT_ON = 0;
				}	
			}
		}
		else
		{
			
			PUMP_DELAY = (int16_t)eeprom_option_byte[_System_off_PUMP_delay]*step_10_sec;
			comp_protect_timer =  (int16_t)eeprom_option_byte[_System_off_COMP_protect_timer_set]*step_10_sec;
		}
		
	}
	/*****************	 FLOW switch  operation  ****************/
#endif

}
/*******************************************************************************
End of function sensor_operation_continue
*******************************************************************************/


/*******************************************************************************
* Function name : sensor_operation_continue
* Description	: none
* Argument	: none
* Return value	: none
*******************************************************************************/
void	clear_flag_for_sensor_continue(void)
{
	water_lever_error_flag_low = 0;
	water_temp_error_flag_low = 0;
	water_temp_error_flag_high = 1;
	ANTIFRZ_PROTECT_ON = 0;
	FLOW_SWITCH_PROTECT_ON = 0;
	comp_over_heat = RESET;
	ERROR_CODE = 0x00;
	ERROR_RLY = RESET;
}




	

