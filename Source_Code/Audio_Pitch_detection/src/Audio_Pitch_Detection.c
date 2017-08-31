/*******        user\apps\audio\Audio_Pipe\Audio_Pitch_detection.c
* Usage:
*  		Tested for use on the DsPIC starer kit 1
* Summary:
*         Automatic pitch detection of a input signal sampled at 8kHz (displaying frequencies between 10Hz and 4kHz)
*
* Inputs:
*       Audio codec input
* Outputs:
*       LED output
*
* Dependencies:
*      Sask_Init()
*
* Notes:
*		This pitch detector can identify a pitch set between 10 Hz and 4kHz. any frequency lower or higher will be treated as a high [red LED] pitch.
*		This is due to the resolution not being able to pick up smaller frequencies, higher ones will show 0s in all pins and therefore the highest occurence will be displayed, 
*		thereby setting the pitch to 4kHz.
*
*		The resolution is also responsiable for small errors in display occuring in a small window surrounding the switch points at 800, 1600, 2400, 3200 Hz[+- ~16 Hz]
*
* ToDo:
*     none
*
* Originator:
*		Sören Schreiber, Student Kingston University, DSP Course, soeren.schreiber@arcor.de
*
* History:
*		Version 1.0     15/03/2016
*		Version 1.1		18/03/2016		--		changed framesize to match the framesize of the fft
*		Version 1.1		24/03/2016		--		added comments tot he source code
*****/

#include <p33FJ256GP506.h>
#include <dsp\h\dsp.h>
#include <board\h\sask.h>
#include <peripherals\adc\h\ADCChannelDrv.h>
#include <peripherals\pwm\h\OCPWMDrv.h>

#include <board\inc\ex_sask_generic.h>
#include <board\inc\ex_sask_led.h>
#include <peripherals\timers\inc\ex_timer.h>

#include "..\inc\ex_audio_process.h"
#include "..\inc\LED_control.h"
#include "..\inc\FFT_processing.h"

#define FRAME_SIZE 				256		//defined framesize for the signal[resolution]

int		adcBuffer		[ADC_CHANNEL_DMA_BUFSIZE] 	__attribute__((space(dma)));	//buffer for the audio input, stored in the direct access memory

fractional 		AudioIn[FRAME_SIZE];	//variable storing the audio input signal

int 		state;		//program state
int			pitchResult;	//detected pitch level

fractcomplex FFTcompResults[FRAME_SIZE]		__attribute__ ((space(ymemory),far)); //Fractional complex variable containing the results generated from the fft function (fractcomplex datatype is defined by the FFT function requirements)

ADCChannelHandle adcChannelHandle;	//handler for the audio input

ADCChannelHandle *pADCChannelHandle 	= &adcChannelHandle;

int main(void)		//main program
{
	ex_sask_init( );	//init sask

	ADCChannelInit	(pADCChannelHandle,adcBuffer);	//init audio input handler

	ADCChannelStart	(pADCChannelHandle);	//start audio input handler

	while(1)
	{
		if(state==0)	//Program is in READY state
		{
			state=0; //set state to 0[READY]
			state=displayState(STATE_READY); //call ready state function and read new state back
			displayLED(LED_OFF, LED_OFF, LED_OFF);	//turn off all LEDs when leaving ready state
		}
		else if(state==1)		//Program is in RUN state
		{
			while(ADCChannelIsBusy(pADCChannelHandle)); //read audio input
			ADCChannelRead	(pADCChannelHandle,AudioIn,FRAME_SIZE);
			
			FFT(FRAME_SIZE, &AudioIn, &FFTcompResults); //FFT function used on audio input, using FRAME_SIZE and returning the results in FFTcompResults
			pitchResult=pitchDetection(&FFTcompResults);//detect the pitch of the FFT result and write the pitch level into pitch result

			switch(pitchResult)
			{
				case 0: //if pitch level is 0 [between 10 and 800 Hz]
					displayLED(LED_OFF, LED_OFF, LED_ON);	//turn on green LED
					break;
				case 1:	//if pitch level is 0 [between 801 and 1600 Hz]
					displayLED(LED_OFF, LED_ON, LED_ON);	//turn on green and yellow LEDs
					break;
				case 2:	//if pitch level is 0 [between 1601 and 2400 Hz]
					displayLED(LED_OFF, LED_ON, LED_OFF);	//turn on yellow LED
					break;				
				case 3:	//if pitch level is 0 [between 2401 and 3200 Hz]
					displayLED(LED_ON, LED_ON, LED_OFF);	//turn on yellow and red LEDs
					break;
				case 4:	//if pitch level is 0 [between 3201 and 4000 Hz]
					displayLED(LED_ON, LED_OFF, LED_OFF);	//turn on red LED
					break;
				default:	//if an error occurs
					state=displayState(STATE_ERROR);	//display ready state
					break;
			}
		}
		else if(state==2)		//Program is in ERROR state
		{
			state=displayState(STATE_ERROR);	//show error state on LEDs and read new state
		}
	}
}
