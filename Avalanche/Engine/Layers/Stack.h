#ifndef __STACK_H
#define __STACK_H

/*-----------------8/7/98 5:02PM--------------------
 * Includes
 * --------------------------------------------------*/

/* system includes */

/* engine includes */

#include "platform/BaseType.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*-----------------8/7/98 5:02PM--------------------
 * Defines
 * --------------------------------------------------*/

struct ts_Stack
{
	unsigned short		usItems;
	unsigned short 	usCurrent;
	unsigned short		usItemSize;

	unsigned char		*pData;
};


ts_Stack *			stkNew( unsigned short usItems, unsigned short usItemSize);

bool				stkPush( ts_Stack *pStack, void *pData);
bool				stkPop( ts_Stack *pStack, void *pData);
bool				stkPeek( ts_Stack *pStack, void *pData);

void					stkFree( ts_Stack *pStack);

#if defined(__cplusplus)
}
#endif

#endif
