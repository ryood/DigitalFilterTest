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

// Begin header file, filter1.h

#ifndef FILTER1_H_ // Include guards
#define FILTER1_H_

static const int filter1_length = 8;

typedef struct
{
	float * pointer;
	float state[8];
	float output;
	float accumulator;
} filter1Type;


filter1Type *filter1_create(void);
void filter1_destroy(filter1Type *pObject);
void filter1_init(filter1Type * pThis);
void filter1_reset(filter1Type * pThis);
#define filter1_writeInput( pThis, input )  \
	filter1_filterBlock( pThis, &input, &pThis->output, 1 );

#define filter1_readOutput( pThis )  \
	pThis->output

int filter1_filterBlock(filter1Type * pThis, float * pInput, float * pOutput, unsigned int count);
#define filter1_outputToFloat( output )  \
	(output)

#define filter1_inputFromFloat( input )  \
	(input)

#endif // FILTER1_H_

