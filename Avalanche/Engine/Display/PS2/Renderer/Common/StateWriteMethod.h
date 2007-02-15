#ifndef __STATES_WRITE_INFO_H
#define __STATES_WRITE__H

#include "WriteModeTypes.h"

typedef struct _StateWriteInfo
{	
	unsigned int			uiStateIdx;
	unsigned int			uiPassIdx;

	unsigned int			uiStateFlags;

	int						iRGBAStreamIdx;
	int						iNormalStreamIdx;
	int						iTexCoordStreamIdx;
} StateWriteInfo;

typedef struct _StatesWriteInfo
{
	unsigned int			numStates;

	RGBADataFormat			RGBAFormat;
	NormalDataFormat		NormalFormat;

	StateWriteInfo *		pStateWriteArray;

}	StatesWriteInfo;
		

#endif