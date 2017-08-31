/*******  user\apps\Audio_Pitch_Detection\inc\FFT_processing.c
*
* Summary:
*		FFT/Pitch detection functions
*
* Dependences:
*		FFT_Processing.h
*		dsp.h
*		
* Inputs:
*		Audio input(fractional)
*		Frame size (int)
*		FFT result array (Complex)
*		Pitch detection result(unsigned long)
*
* Outputs:
*		Detected Pitch of a signal (int)
*		FFT Results (fractcomplex)
*
* Functions:
*		void FFT(int framsize, fractional *audioIN, fractcomplex *compX)		
*		int pitchDetection(fractcomplex *compXFftResults)
*
* Notes:
*		Pitch detector can detect pitches between 10 and 4000 HZ. 
*		Lower then 10 is treated as 4000 HZ due to insuficient resulution on the FFT spectrum, higher then 4000 HZ is treated as 4000 HZ due to the highest freq-pin being taken as the detected pitch if all values are the same in this case 0
*		The error state would only occure in case of a calculation error which returns a value higher then 4000 HZ
*
* Originator: 
*		Sören Schreiber, Student Kingston University, DSP Course, soeren.schreiber@arcor.de
*
* Version:
*		1.1		23/03/2016  --  removed convertInputforFFT function, commented source code
*		1.0		17/03/2016
*
*/
#include "..\inc\FFT_processing.h"
#include <dsp.h>

fractcomplex FFTcompTwidFactors[FFT_FRAME_SIZE]__attribute__ ((space(xmemory),far)); //Twid factors with size "FFT_FRAME_SIZE" stored in the X-memory
fractcomplex FFTcompWorkSpace[FFT_FRAME_SIZE]__attribute__ ((space(ymemory),far)); //Complex workspace for the FFT function with size "FFT_FRAME_SIZE" stored in the Y-memory

int	peakFrequencyBin = 0; //integer store variable, used for to return the detected Pitch back to the main program

/*
*	This function runs the FFT on the given input values and returns the results into the address given with "FFTcombResult"
*/
void FFT(int framesize, fractional *audioIN, fractcomplex *FFTcompResult)
{
	int i; //loop counter 
	
	//copy fractional audio signal into real part of complex fractional data type
	for(i=0;i<framesize;i++)
	{
		FFTcompWorkSpace[i].real = audioIN[i];
		FFTcompWorkSpace[i].imag = 0;
	}	
	
	//generate the first half of the set of twiddle factors required by the DFT
	TwidFactorInit (8,FFTcompTwidFactors,0);

	//generate the DFT of the audio signal
	FFTComplex(8,FFTcompResult,FFTcompWorkSpace,FFTcompTwidFactors,0xFF00);

}

/*
*	This function detects the pitch in a given dataset of fractional complex FFT results
*/
int pitchDetection(fractcomplex *compXFftResults)
{
	int detectedPitch=0; //stores the actual pitch frequency
	int detectedPitchLvl=0; // stores an assigned pitch level to the detected pitch
	fractional FftResults[FFT_FRAME_SIZE]; // stores the results from the function SquareMagnituteCplx

	/* Compute the square magnitude of the complex FFT output array so we have a Real output vetor */
	SquareMagnitudeCplx(FFT_FRAME_SIZE/2, &compXFftResults[0], &FftResults[0]);

	/* Find the frequency Bin ( = index into the SigCmpx[] array) that has the largest energy*/
	/* i.e., the largest spectral component */
	VectorMax(FFT_FRAME_SIZE/2, &FftResults[0], &peakFrequencyBin);

	/* Compute the frequency (in Hz) of the largest spectral component */
	detectedPitch = peakFrequencyBin*(SAMPLING_RATE/FFT_FRAME_SIZE);
	
	if(detectedPitch<=800) //if detected pitch is below or equal to 800 HZ
	{
		detectedPitchLvl=0; //Low pitch level
	}
	else if(detectedPitch<=1600) //if pitch between 801 and 1600 HZ 
	{
		detectedPitchLvl=1; //Low\Medium pitch level
	}
	else if(detectedPitch<=2400) //if pitch between 1601 and 2400 HZ
	{
		detectedPitchLvl=2; //Medium pitch level
	}
	else if(detectedPitch<=3200) //if pitch between 2401 and 3200 HZ
	{
		detectedPitchLvl=3; //Medium\High pitch level
	}
	else if(detectedPitch<=4000) //if pitch between 3201 and 4000 HZ
	{
		detectedPitchLvl=4; //High pitch level
	}
	else //if pitch above 4000 HZ set error state 
	{
		detectedPitchLvl=5; //Error occured
	}

	return detectedPitchLvl; // return detected pitch level
}
