// FIRResponse.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

/******************************* SOURCE LICENSE *********************************
Copyright (c) 2015 MicroModeler.

A non-exclusive, nontransferable, perpetual, royalty-free license is granted to the Licensee to
use the following Information for academic, non-profit, or government-sponsored research purposes.
Use of the following Information under this License is restricted to NON-COMMERCIAL PURPOSES ONLY.
Commercial use of the following Information requires a separately executed written license agreement.

This Information is distributed WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

******************************* END OF LICENSE *********************************/

// A commercial license for MicroModeler DSP can be obtained at http://www.micromodeler.com/launch.jsp

#include "filter1.h"

#include <stdlib.h> // For malloc/free
#include <string.h> // For memset

float filter1_coefficients[16] =
{
	0.0000000, -0.028778783, -0.047482570, -0.040290297, 0.0000000, 0.067150494,
	0.14244771, 0.20145148, 0.22375634, 0.20145148, 0.14244771, 0.067150494,
	0.0000000, -0.040290297, -0.047482570, -0.028778783
};


filter1Type *filter1_create(void)
{
	filter1Type *result = (filter1Type *)malloc(sizeof(filter1Type));	// Allocate memory for the object
	filter1_init(result);											// Initialize it
	return result;																// Return the result
}

void filter1_destroy(filter1Type *pObject)
{
	free(pObject);
}

void filter1_init(filter1Type * pThis)
{
	filter1_reset(pThis);

}

void filter1_reset(filter1Type * pThis)
{
	memset(&pThis->state, 0, sizeof(pThis->state)); // Reset state to 0
	pThis->pointer = pThis->state;						// History buffer points to start of state buffer
	pThis->output = 0;									// Reset output

}

int filter1_filterBlock(filter1Type * pThis, float * pInput, float * pOutput, unsigned int count)
{
	float *pOriginalOutput = pOutput;	// Save original output so we can track the number of samples processed
	float accumulator;

	for (; count; --count)
	{
		pThis->pointer[filter1_length] = *pInput;						// Copy sample to top of history buffer
		*(pThis->pointer++) = *(pInput++);										// Copy sample to bottom of history buffer

		if (pThis->pointer >= pThis->state + filter1_length)				// Handle wrap-around
			pThis->pointer -= filter1_length;

		accumulator = 0;
		filter1_dotProduct(pThis->pointer, filter1_coefficients, &accumulator, filter1_length);
		*(pOutput++) = accumulator;	// Store the result
	}

	return pOutput - pOriginalOutput;

}

void filter1_dotProduct(float * pInput, float * pKernel, float * pAccumulator, short count)
{
	float accumulator = *pAccumulator;
	while (count--)
		accumulator += ((float)*(pKernel++)) * *(pInput++);
	*pAccumulator = accumulator;

}

#define LOOP	(2)
#define PERIOD	(50)

int _tmain(int argc, _TCHAR* argv[])
{
	filter1Type* filter = filter1_create();

	printf("Sample\tInput\tOutput\n");
	int count = 0;
	for (int i = 0; i < LOOP; i++) {
		float in = 1.0f;
		for (int j = 0; j < PERIOD / 2; j++) {
			filter1_writeInput(filter, in);
			float out = filter1_readOutput(filter);
			printf("%d\t%f\t%f\n", count, in, out);
			count++;
		}
		in = 0.0f;
		for (int j = 0; j < PERIOD / 2; j++) {
			filter1_writeInput(filter, in);
			float out = filter1_readOutput(filter);
			printf("%d\t%f\t%f\n", count, in, out);
			count++;
		}
	}

	return 0;
}


