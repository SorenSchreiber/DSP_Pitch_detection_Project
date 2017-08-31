/*******  user\apps\Audio_Pitch_Detection\inc\LED_control.h
*
* Summary:
*		LED Control function for different programm states
*
* Dependences:
*		ex_timer.h
*
* Inputs:
*		State of the programm, LEDs		int, defined States
*
* Outputs:
*		LEDs output
*
* Functions:
*		void displayState(int state)
*		void displayLED(int led_red, int led_yellow, int led_green)
*
* Notes:
*		none
*
* Originator: 
*		Sören Schreiber, Student Kingston University, DSP Course, soeren.schreiber@arcor.de
*
* Version:
*		1.0		15/03/2016
*		1.1		16/03/2016		--		working implementations in the source file, tested said functions
*		1.1		24/03/2016		--		added comments to header file
*
*/

#ifndef __LED_CONTROL__
#define __LED_CONTROL__		//define __LED_CONTROL__

#define STATE_READY 0 //ready state
#define STATE_RUN 1 //run state
#define STATE_ERROR 2 //error state

#define LED_ON 1 //led on
#define LED_OFF 0 //led off

#define TRUE 1 //bool true
#define FALSE 0 //bool false

#define clock_frequency 40e6 // standart clock frequency
#define timeout 0.3 //wait time for the timer 

int displayState(int state); //switch between different running states[0=READY, 1=RUN, 2=ERROR]

void readyState(); //display ready state on leds

void errorState(); //display error state on leds

void displayLED(int led_red, int led_yellow, int led_green); //display user selected leds[1=ON, 0=OFF]

#endif
