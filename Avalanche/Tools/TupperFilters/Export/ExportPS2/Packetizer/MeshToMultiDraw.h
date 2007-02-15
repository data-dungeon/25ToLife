#ifndef __MESH_TO_MULTI_DRAW_H
#define __MESH_TO_MULTI_DRAW_H

#include "MeshToWriteInfo.h"

typedef struct _TempPassInfo
{
	unsigned int			StateFlags;
	
	MeshPassInfo *			pPassInfo;

	StateWriteInfo			StateWrite;
			
	// this stuff is temporary
	RGBAStorageMethod		RGBAStorage;
	RGBADataSize			RGBASize;

	NormalStorageMethod	NormalStorage;
	NormalDataSize			NormalSize;
} TempPassInfo;

typedef struct _StateInfo
{
	unsigned int		numPasses;
	TempPassInfo *		pPassInfoArray;
} StateInfo;


class MeshToMultiDraw : public MeshToWriteInfo
{
	void 								setDefaultPassInfo( TempPassInfo &TempInfo, unsigned int uiState, unsigned int uiPass, MeshPassInfo &PassInfo);
	void 								checkColorCompression( unsigned int numPasses, TempPassInfo *pTempPassArray);
	StatesWriteInfo * 			bucketPassInfo( unsigned int totalPasses, TempPassInfo *pAllPasses, unsigned int &ReturnStateCount);

	int								sortCompareFunction( void *pElement1, void *pElement2);
	int								writeCompareFunction( void *pElement1, void *pElement2);

public:
	MeshToMultiDraw( ) : MeshToWriteInfo( ) { }

	void								freeStatesWriteInfo( StatesWriteInfo *pWriteInfo, unsigned int numStates);
	StatesWriteInfo *				buildStatesWriteInfo( MeshInterface &Protocol, unsigned int &returnStateCount, unsigned int &modelHasFlags);

} ;



#endif