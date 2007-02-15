#ifndef __STATES_WRITE_INFO_H
#define __STATES_WRITE_INFO_H

#include "WriteModeTypes.h"

enum VUStateFlags
{
	STATE_FLAG_NO_SHADOW					= 0x0001,
	STATE_FLAG_ENVIRONMENT_MAPPED		= 0x0002,
} ;

typedef struct _StateWriteInfo
{	
	unsigned short			u16VUStateFlags;

	unsigned int			idxState;
	unsigned int			idxPass;

	int						idxRGBAStream;
	int						idxNormalStream;
	int						idxTexCoordStream;
} StateWriteInfo;


enum StatesWriteInfoFlags
{
	STATES_VU_STATE_FLAG_MASK	= 0x0000ffff,
	STATES_DYNAMIC_TEXTURES		= 0x00010000,
} ;

typedef struct _StatesWriteInfo
{
	unsigned int			Flags;

	unsigned int			numStates;

	RGBADataSize			RGBASize;
	RGBAStorageMethod		RGBAStorage;

	NormalDataSize			NormalSize;
	NormalStorageMethod	NormalStorage;

	StateWriteInfo *		pStateWriteInfoArray;

}	StatesWriteInfo;
		

#endif