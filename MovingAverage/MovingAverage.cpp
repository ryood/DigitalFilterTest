// MovingAverage.cpp : Defines the entry point for the console application.
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
#include <stdio.h>		// For test case I/O

float filter1_impulseInput[9] =
{
	1.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
	0.00000000
};

float filter1_impulseOutput[9] =
{
	0.11111111, 0.11111111, 0.11111111, 0.11111111, 0.11111111, 0.11111111, 0.11111111, 0.11111111,
	0.11111111
};

float filter1_noiseInput[31] =
{
	1.00000000, 1.00000000, 1.00000000, 1.00000000, 1.00000000, -1.00000000, -1.00000000, -1.00000000,
	1.00000000, 1.00000000, -1.00000000, 1.00000000, 1.00000000, 1.00000000, -1.00000000, 1.00000000,
	-1.00000000, 1.00000000, -1.00000000, -1.00000000, -1.00000000, -1.00000000, 1.00000000, -1.00000000,
	-1.00000000, 1.00000000, -1.00000000, 1.00000000, 1.00000000, -1.00000000, -1.00000000
};

float filter1_noiseOutput[31] =
{
	0.11111111, 0.22222222, 0.33333333, 0.44444444, 0.55555556, 0.44444444, 0.33333333, 0.22222222,
	0.33333333, 0.33333333, 0.11111111, 0.11111111, 0.11111111, 0.11111111, 0.11111111, 0.33333333,
	0.33333333, 0.33333333, 0.11111111, 0.11111111, -0.11111111, -0.33333333, -0.33333333, -0.33333333,
	-0.55555556, -0.33333333, -0.55555556, -0.33333333, -0.11111111, -0.11111111, -0.11111111
};

float filter1_multiSineInput[18] =
{
	0.43085087, 0.50908471, -0.09629260, -0.24784774, 0.22567024, 0.19844673, -1.00000000, 0.40295920,
	0.09768124, -0.35019536, 0.05522338, 0.50698533, 0.21635038, -0.55971426, 0.67063363, -0.04293300,
	-0.66336379, 0.01118638
};

float filter1_multiSineOutput[18] =
{
	0.04787232, 0.10443729, 0.09373811, 0.06619947, 0.09127394, 0.11332358, 0.00221247, 0.04698571,
	0.05783918, -0.02894373, -0.07937277, -0.01234189, 0.03923568, -0.04802926, 0.00443595, 0.11077673,
	-0.00770360, -0.01731414
};

float filter1_overflowInput[9] =
{
	1.00000000, 1.00000000, 1.00000000, 1.00000000, 1.00000000, 1.00000000, 1.00000000, 1.00000000,
	1.00000000
};

float filter1_overflowOutput[9] =
{
	0.11111111, 0.22222222, 0.33333333, 0.44444444, 0.55555556, 0.66666667, 0.77777778, 0.88888889,
	1.00000000
};

float filter1_testOutput[31];

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
	pThis->accumulator = 0; // Reset accumulator

}

int filter1_filterBlock(filter1Type * pThis, float * pInput, float * pOutput, unsigned int count)
{
	int originalCount = count;
	float accumulator = pThis->accumulator;

	while (count--)
	{
		accumulator += *pInput;					// Add the new sample
		*(pOutput++) = accumulator * 0.1111111111111111;	// Write the output. (Divide by $filter.N})
		accumulator -= *(pThis->pointer);		// Subtract the oldest sample

		*(pThis->pointer++) = *(pInput++);	// Store the new sample in the circular history buffer
		if (pThis->pointer >= pThis->state + filter1_length)		// Handle wrap-around
			pThis->pointer -= filter1_length;
	}

	pThis->accumulator = accumulator;	// Store the accumulator
	return originalCount;

}

void filter1_runTests(void)
{
	float actual;
	int samplesProcessed;

	filter1Type *filter = filter1_create();	// Create an instance of the filter

	printf("\n\nRunning tests for: filter1\n");

	printf("\nimpulse test\n");
	filter1_reset(filter);
	samplesProcessed = filter1_filterInChunks(filter, filter1_impulseInput, filter1_testOutput, 9);		// Filter the input test signal
	filter1_compareResult(filter1_testOutput, filter1_impulseOutput, samplesProcessed, 0.000001, 0.000001);	// Compare with the pre-computed output signal

	printf("\nnoise test\n");
	filter1_reset(filter);
	samplesProcessed = filter1_filterInChunks(filter, filter1_noiseInput, filter1_testOutput, 31);		// Filter the input test signal
	filter1_compareResult(filter1_testOutput, filter1_noiseOutput, samplesProcessed, 0.000001, 0.000001);	// Compare with the pre-computed output signal

	printf("\nmultiSine test\n");
	filter1_reset(filter);
	samplesProcessed = filter1_filterInChunks(filter, filter1_multiSineInput, filter1_testOutput, 18);		// Filter the input test signal
	filter1_compareResult(filter1_testOutput, filter1_multiSineOutput, samplesProcessed, 0.000001, 0.000001);	// Compare with the pre-computed output signal

	printf("\noverflow test\n");
	filter1_reset(filter);
	samplesProcessed = filter1_filterInChunks(filter, filter1_overflowInput, filter1_testOutput, 9);		// Filter the input test signal
	filter1_compareResult(filter1_testOutput, filter1_overflowOutput, samplesProcessed, 0.000001, 0.000001);	// Compare with the pre-computed output signal
	actual = filter1_outputToFloat(filter1_testOutput[8]);

	printf("Overflow test = (%f). Expected = 1.000. ratio=%f. %s\n",
		actual, actual / 1.000f, actual / 1.000f > 0.95f && actual / 1.000f < 1.05f ? "PASS" : "FAIL");

	filter1_destroy(filter);

}

void filter1_compareResult(float * pInput, float * pReference, int count, float maxThreshold, float msThreshold)
{
	float ms = 0, mx = 0, actual, delta;
	printf("int	float	expected	delta\n");
	while (count--)
	{
		actual = filter1_outputToFloat(*pInput);
		delta = actual - *pReference;
		printf("%li	%f	%f	%f\n", (long)*(pInput), actual, *pReference, delta);
		++pInput;
		++pReference;

		ms += delta * delta;
		if (delta > mx) mx = delta;
		if (-delta > mx) mx = -delta;
	}

	printf("Maximum Error: %f. Threshold = %f. Ratio = %f, %s\n", mx, maxThreshold, mx / maxThreshold, mx < maxThreshold ? "PASS" : "FAIL");
	printf("Mean Squared Error: %f. Threshold = %f. Ratio = %f, %s\n", ms, msThreshold, ms / msThreshold, ms < msThreshold ? "PASS" : "FAIL");

}

int filter1_filterInChunks(filter1Type * pThis, float * pInput, float * pOutput, int length)
{
	int processedLength = 0;
	unsigned int chunkLength, outLength;
	static long random = 0x6428734; // Use pseudo-random number generator to split input into small random length chunks.
	while (length > 0)
	{
		chunkLength = random & 0xf;											// Choose random chunkLength from 0 - 15
		if (chunkLength > length) chunkLength = length;					// Limit chunk length to the number of remaining samples
		outLength = filter1_filterBlock(pThis, pInput, pOutput, chunkLength);		// Filter the block and determine the number of returned samples
		pOutput += outLength;												// Update the output pointer
		processedLength += outLength;										// Update the total number of samples output
		pInput += chunkLength;												// Update the input pointer
		length -= chunkLength;												// Update the number of samples remaining
		random = random + 0x834f4527;										// Cycle the simple random number generator
	}
	return processedLength;													// Return the number of samples processed

}

int _tmain(int argc, _TCHAR* argv[])
{
	filter1_runTests();
	return 0;
}
