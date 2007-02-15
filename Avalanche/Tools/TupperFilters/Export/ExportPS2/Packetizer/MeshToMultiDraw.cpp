#include "ExportPch.h"
#include "MeshToMultiDraw.h"
#include "DMAObjectFactory.h"

int MeshToMultiDraw::sortCompareFunction( void *pElement1, void *pElement2)
{
	assert( pElement1);
	assert( pElement2);

	TempPassInfo &TP1 = * ((TempPassInfo *) pElement1);
	TempPassInfo &TP2 = * ((TempPassInfo *) pElement2);

#define USE_SORT_KEY
#ifdef USE_SORT_KEY
	if ( TP1.pPassInfo->m_sortKey != TP2.pPassInfo->m_sortKey)
	{
		return TP2.pPassInfo->m_sortKey - TP1.pPassInfo->m_sortKey;
	}
#else

	if( TP1.StateFlags != TP2.StateFlags )
		return TP2.StateFlags - TP1.StateFlags;

	if( TP1.StateWrite.uiStateFlags != TP2.StateWrite.uiStateFlags)
		return TP2.StateWrite.uiStateFlags - TP1.StateWrite.uiStateFlags;

	if( TP1.RGBAStorage != TP2.RGBAStorage)
		return TP2.RGBAStorage - TP1.RGBAStorage;

	if( TP1.RGBAStorage != RGBA_STORE_NONE)
		if( TP1.RGBASize != TP2.RGBASize)
			return TP2.RGBASize - TP1.RGBASize;

	if( TP1.NormalStorage != TP2.NormalStorage)
		return TP2.NormalStorage - TP1.NormalStorage;

	if( TP1.NormalStorage != NORMAL_STORE_NONE)
		if( TP1.NormalSize != TP2.NormalSize)
			return TP2.NormalSize - TP1.NormalSize;

	if( TP1.StateWrite.idxState != TP2.StateWrite.idxState)
		return TP2.StateWrite.idxState - TP1.StateWrite.idxState;

	if( TP1.StateWrite.idxPass != TP2.StateWrite.idxPass)
		return TP2.StateWrite.idxPass - TP1.StateWrite.idxPass;
#endif

	return 0;
}

int MeshToMultiDraw::writeCompareFunction( void *pElement1, void *pElement2)
{
	assert( pElement1);
	assert( pElement2);

	TempPassInfo &TP1 = * ((TempPassInfo *) pElement1);
	TempPassInfo &TP2 = * ((TempPassInfo *) pElement2);

	if( TP1.StateFlags != TP2.StateFlags )
		return TP1.StateFlags - TP2.StateFlags;

	if( TP1.RGBAStorage != TP2.RGBAStorage)
		return TP2.RGBAStorage - TP1.RGBAStorage;

	if( TP1.RGBAStorage != RGBA_STORE_NONE)
		if( TP1.RGBASize != TP2.RGBASize)
			return TP2.RGBASize - TP1.RGBASize;

	if( TP1.NormalStorage != TP2.NormalStorage)
		return TP2.NormalStorage - TP1.NormalStorage;

	if( TP1.NormalStorage != NORMAL_STORE_NONE)
		if( TP1.NormalSize != TP2.NormalSize)
			return TP2.NormalSize - TP1.NormalSize;

	return 0;
}


void MeshToMultiDraw::setDefaultPassInfo( TempPassInfo &TempInfo, unsigned int uiState, unsigned int uiPass, MeshPassInfo &PassInfo)
{
	TempInfo.pPassInfo = & PassInfo;

	TempInfo.StateFlags = 0;

	TempInfo.StateWrite.idxState = uiState;
	TempInfo.StateWrite.idxPass = uiPass;

	TempInfo.StateWrite.idxRGBAStream = PassInfo.idxRGBAStream;
	TempInfo.StateWrite.idxNormalStream = PassInfo.idxNormalStream;
	TempInfo.StateWrite.idxTexCoordStream = PassInfo.idxTexCoordStream;

	if( PassInfo.idxRGBAStream >= 0)
	{
		if( PassInfo.mPassFlags & MeshPassInfo::PASS_FLAG_UNCOMPRESSED_COLORS)
		{
			TempInfo.RGBAStorage = RGBA_STORE_UNCOMPRESSED;
			TempInfo.RGBASize = d_DefRGBASize;
		}
		else
		{
			TempInfo.RGBAStorage = RGBA_STORE_COMPRESSED;
			TempInfo.RGBASize = d_DefRGBASize; // just in case we later decide they need to be uncompressed
		}
	}
	else
	{
		TempInfo.RGBAStorage = RGBA_STORE_NONE; // just in case we later decide to store some "fake" RGBAs
		TempInfo.RGBASize = d_DefRGBASize; 
	}

	TempInfo.NormalSize = d_DefNormalSize;

	if( PassInfo.idxNormalStream >= 0)
		TempInfo.NormalStorage = d_DefNormalStorage;
	else
		TempInfo.NormalStorage = NORMAL_STORE_NONE;

}

//#define PRINT_PASSES
#ifdef PRINT_PASSES
static void PrintPasses(FILE *pFile,int totalPasses,TempPassInfo *pPassInfoList)
{
	for (int passIndex=0;passIndex<totalPasses;passIndex++)
	{
		TempPassInfo &pass = pPassInfoList[passIndex];
		fprintf(pFile,"State %d Pass %d Flags %d RGBStorage %d RGBSize %d NormalStorage %d NormalSize %d TextureID %d SortKey %d\n",
			pass.StateWrite.idxState,pass.StateWrite.idxPass,pass.StateWrite.uiStateFlags,
			(int)pass.RGBAStorage,pass.RGBASize,(int)pass.NormalStorage,pass.NormalSize,
			pass.pPassInfo->m_textureID,pass.pPassInfo->m_sortKey);
	}
}
#endif

StatesWriteInfo * MeshToMultiDraw::buildStatesWriteInfo( MeshInterface &Protocol, unsigned int &returnPassCount, unsigned int &modelHasFlags)
{
	unsigned int 		currState, currPass;

	unsigned int		totalPasses = countTotalPasses( Protocol);

	TempPassInfo *		pPassInfoList = new TempPassInfo[totalPasses];

	modelHasFlags = 0;
	// build up a temporary array of state information

	unsigned int numStates = Protocol.numStates( );

	StateInfo *			pStateInfoArray = new StateInfo[numStates];
	TempPassInfo *		pNextPassInfo = pPassInfoList;

	for( currState = 0; currState < numStates; currState++)
	{
		unsigned int numPasses = Protocol.numPasses( currState);
		TempPassInfo *		pCurrPassInfo = pNextPassInfo; pNextPassInfo += numPasses;
			
		pStateInfoArray[currState].numPasses = numPasses;
		pStateInfoArray[currState].pPassInfoArray = pCurrPassInfo;

		for( currPass = 0; currPass < numPasses; currPass++)
		{
			pCurrPassInfo[currPass].StateWrite.u16VUStateFlags = 0;
			pCurrPassInfo[currPass].pPassInfo = & Protocol.peekPassInfo( currState, currPass );
			setDefaultPassInfo( pCurrPassInfo[currPass], currState, currPass, *pCurrPassInfo[currPass].pPassInfo);

			if( currPass > 0)
				pCurrPassInfo[currPass].StateWrite.u16VUStateFlags |= STATE_FLAG_NO_SHADOW;

			if( pCurrPassInfo[currPass].pPassInfo->mPassFlags & MeshPassInfo::PASS_FLAG_ENVIRONMENT_MAPPED)
			{
				pCurrPassInfo[currPass].StateWrite.u16VUStateFlags |= STATE_FLAG_ENVIRONMENT_MAPPED;
				modelHasFlags |= DMAObjectFactory::MODEL_HAS_ENVIRONMENT_MAP;
			}

			if( pCurrPassInfo[currPass].pPassInfo->mPassFlags & MeshPassInfo::PASS_FLAG_DYNAMIC_TEXTURE)
			{
				pCurrPassInfo[currPass].StateFlags |= STATES_DYNAMIC_TEXTURES;
				modelHasFlags |= DMAObjectFactory::MODEL_HAS_DYNAMIC_TEXTURES;
			}
			
		}

		checkColorCompression( numPasses, pCurrPassInfo);		
	}

// temp printing code
#ifdef PRINT_PASSES
	FILE *pFile = fopen("c:\\temp.txt","wt");
	fprintf(pFile,"Before sort\n");
	PrintPasses(pFile,totalPasses,pPassInfoList);
#endif

	// old method
	sortSetup( sizeof( TempPassInfo));
	SortElementList( totalPasses, pPassInfoList);

#ifdef PRINT_PASSES
	fprintf(pFile,"\n\nAfter sort\n");
	PrintPasses(pFile,totalPasses,pPassInfoList);
	fclose(pFile);
#endif


	StatesWriteInfo *pReturnPtr = bucketPassInfo( totalPasses, pPassInfoList, returnPassCount );

	delete [] pStateInfoArray;
	delete [] pPassInfoList;

	return pReturnPtr;
}

void MeshToMultiDraw::checkColorCompression( unsigned int numPasses, TempPassInfo *pTempPassArray)
{
	unsigned int			currPass;

	bool				  		bUncompressColors = false;
	unsigned int 	  		idxBaseRGBAs = pTempPassArray->StateWrite.idxRGBAStream;
	RGBAStorageMethod		baseStorage = pTempPassArray->RGBAStorage;

	for( currPass = 0; currPass < numPasses; currPass++)
	{
		if( 	(pTempPassArray[currPass].StateWrite.idxRGBAStream != idxBaseRGBAs)  ||
				(pTempPassArray[currPass].pPassInfo->mPassType == MeshPassInfo::PASS_TYPE_TEXTURE_BLEND) ||
				(pTempPassArray[currPass].RGBAStorage != baseStorage) )
		{
			bUncompressColors = true;
			break;
		}
	}

	if( bUncompressColors == true)
	{
		for( currPass = 0; currPass < numPasses; currPass++)
			pTempPassArray[currPass].RGBAStorage = RGBA_STORE_UNCOMPRESSED;
	}
}


void MeshToMultiDraw::freeStatesWriteInfo( StatesWriteInfo *pWriteInfo, unsigned int numStates)
{
	for( unsigned int i = 0; i < numStates; i++)
		delete [] pWriteInfo[i].pStateWriteInfoArray;

	delete [] pWriteInfo;
}

StatesWriteInfo * MeshToMultiDraw::bucketPassInfo( unsigned int totalPasses, TempPassInfo *pAllPasses, unsigned int &ReturnStateCount)
{
	TempPassInfo *			pCurrentBasePass;

	ReturnStateCount = 1;

	pCurrentBasePass = pAllPasses;

	for( unsigned int i = 1; i < totalPasses; i++)
	{
		if( writeCompareFunction( pCurrentBasePass, pAllPasses + i) != 0)
		{
			ReturnStateCount++;
			pCurrentBasePass = pAllPasses + i;
		}
	}

	StatesWriteInfo *pInfo = new StatesWriteInfo[ReturnStateCount];

	pCurrentBasePass = pAllPasses;

	unsigned int passesToGo = totalPasses;

	for( unsigned int j = 0; j < ReturnStateCount; j++)
	{
		unsigned int passesThisState = 1;

		while( (passesThisState < passesToGo)  &&  writeCompareFunction( pCurrentBasePass, pCurrentBasePass + passesThisState) == 0)
			passesThisState++;

		passesToGo -= passesThisState;

		pInfo[j].numStates = passesThisState;
		pInfo[j].Flags = pCurrentBasePass[0].StateFlags;

		pInfo[j].RGBASize = pCurrentBasePass->RGBASize;
		pInfo[j].RGBAStorage = pCurrentBasePass->RGBAStorage;

		pInfo[j].NormalSize = pCurrentBasePass->NormalSize;
		pInfo[j].NormalStorage = pCurrentBasePass->NormalStorage;

		pInfo[j].pStateWriteInfoArray = new StateWriteInfo[passesThisState];

		for( unsigned int k = 0; k < passesThisState; k++)
		{
			pInfo[j].Flags |= pCurrentBasePass[k].StateWrite.u16VUStateFlags & STATES_VU_STATE_FLAG_MASK;
			pInfo[j].pStateWriteInfoArray[k] = pCurrentBasePass[k].StateWrite;
		}

		pCurrentBasePass += passesThisState;
	}

	return pInfo;

}
	




