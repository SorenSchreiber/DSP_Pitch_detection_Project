/*******  user\apps\Audio_Pitch_Detection\inc\LED_control.c
*
* Summary:
*		LED Control function for different programm states
*
* Inputs:
*		State of the programm, LEDs			int, define states
*
* Outputs:
*		LEDs output
*
* Functions:
*		int displayState(int state)
*		void displayLED(int led_red, int led_yellow, int led_green)
*		void readyState()
*		void errorState()
*
* Notes:
*		none
*
* Originator: 
*		Sören Schreiber, Student Kingston University, DSP Course, soeren.schreiber@arcor.de
*
* Version:
*		1.0		15/03/2016
*		1.1		16/03/2016		--		working implementations for all functions + testing of said implementations
*		1.1		24/03/2016		--		added comments to source code
*
*/
#include <board\h\sask.h>
#include <peripherals\timers\inc\ex_timer.h>

#include "..\inc\LED_control.h"

int currentState; //Variable containing the programs current state -> returned to calling program code for further use

float instruction_clock_frequency, instruction_cycle_time;	//variables needed for timer functions -> types taken from the needs of the timer functions


int displayState(int state)	//hub function for state selection of the program
{
	switch(state)
	{
		case 0:
				currentState=0;	//set currentState to 0
				readyState();	//display ready state on the LEDs
				break;
		case 1:
				currentState=1;	//set current state to 1
				break;
		default:
				currentState=2;	//set current state to 2
				errorState();	//display error state on the LEDs
	}

	return currentState;	//return current state to the calling program code
}

void displayLED(int led_red, int led_yellow, int led_green)	//function for selective LED control
{
	if(led_red==1)	
	{
		RED_LED=SASK_LED_ON;	//turn on red LED
	}
	else if(led_red==0)
	{
		RED_LED=SASK_LED_OFF;	//turn off red LED
	}

	if(led_yellow==1)
	{
		YELLOW_LED=SASK_LED_ON;	//turn on yellow LED
	}
	else if(led_yellow==0)
	{
		YELLOW_LED=SASK_LED_OFF;//turn off yellow LED
	}

	if(led_green==1)
	{
		GREEN_LED=SASK_LED_ON;	//turn on green LED
	}
	else if(led_green==0)
	{
		GREEN_LED=SASK_LED_OFF;	//turn off green LED
	}
}

void readyState()	//function to display ready state using the LEDs
{
	instruction_clock_frequency = clock_frequency;				//set up variable needed for fo the timer function
	instruction_cycle_time = 1.0 / instruction_clock_frequency;	//set up variable needed for fo the timer function

	ex_timer_init( instruction_cycle_time );			//init timer 

	int led_selector=0;	//determins which LED will be turned on in the current cycle
	while(currentState==0)	//while program is in ready state
	{
		if((SWITCH_S1==0) && (SWITCH_S2==1))	//checking if Switch 1 is pressed and switch 2 is unpressed (direct read of the buttons to bypass a debouncer problem)
		{
			currentState=1;	//set current state to 1
		}
		else
		{
			switch(led_selector)
			{
				case 0:			//turn on red LED, turn off yellow and green LEDs
					RED_LED=SASK_LED_ON;
					YELLOW_LED=SASK_LED_OFF;
					GREEN_LED=SASK_LED_OFF;
					led_selector=led_selector+1;
					break;
				case 1:			//turn on yellow LED, turn off red and green LEDs
					RED_LED=SASK_LED_OFF;
					YELLOW_LED=SASK_LED_ON;
					GREEN_LED=SASK_LED_OFF;
					led_selector=led_selector+1;
					break;
				case 2:			//turn on green LED, turn off red and yellow LEDs
					RED_LED=SASK_LED_OFF;
					YELLOW_LED=SASK_LED_OFF;
					GREEN_LED=SASK_LED_ON;
					led_selector=0;
					break;
				default:		//if an error occurse set program into error state
					errorState();
			}
		
			ex_timer_wait(timeout); //wait for the time[in seconds] set in timeout(defined in header file)
		}
	}
}

void errorState()		//display error state on the LEDs
{
	instruction_clock_frequency = clock_frequency;					//set up variable needed for fo the timer function
	instruction_cycle_time = 1.0 / instruction_clock_frequency;		//set up variable needed for fo the timer function

	ex_timer_init( instruction_cycle_time );		//init timer

	int current_led_state=FALSE; 	//set LED status to OFF

	while(1)
	{
		if((SWITCH_S1==0) && (SWITCH_S2==0))	//check if switch 1 and 2 are pressed
		{
			current_led_state=FALSE;	//set LED status to OFF
			currentState=0;			//set current state to ready
			break;
		}
		else if(current_led_state==FALSE) //if LEDs are off turn them on
		{
			RED_LED=SASK_LED_ON;
			YELLOW_LED=SASK_LED_ON;
			GREEN_LED=SASK_LED_ON;

			current_led_state=TRUE;		//invert current LED status
		}
		else if(current_led_state==TRUE) //if LEDs are on turn them off
		{

			RED_LED=SASK_LED_OFF;
			YELLOW_LED=SASK_LED_OFF;
			GREEN_LED=SASK_LED_OFF;

			current_led_state=FALSE;	//invert current LED status
		}

		ex_timer_wait(timeout);	//wait for the time[in seconds] set in timeout(defined in header file)
	}
}
