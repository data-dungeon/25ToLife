#include "ExportPch.h"
#include <assert.h>

#include <strstream>

#include "DMAObjectFactory.h"

#include "DMAUtilities.h"

#include "MeshToMultiDraw.h"
#include "StatesWriteInfo.h"

#include "VSTtoANMDblBuffer.h"
#include "VSTtoDblVSTandZ32.h"
#include "VSTtoDblSTPColorSets.h"

DMAObjectFactory::DMAObjectFactory( )
{
	p_MemManager = 0;

	d_VUMemAvailable = 1024;

	d_StaticVUMemAllocated = 32;
	d_BonedVUMemAllocated = 32;
}

void DMAObjectFactory::setMemManager( MemManager &Manager)
{
	p_MemManager = &Manager;
}

void DMAObjectFactory::setVUMemUsage( unsigned int _MemAvailable)
{
//	d_VUMemAvailable = _MemAvailable;
}

unsigned int DMAObjectFactory::buildHasFlags( MeshInterface &Protocol )
{
	unsigned int			modelHasFlags = 0;

	for( unsigned int state = 0; state < Protocol.numStates( ); state++)
	{
		for( unsigned int pass = 0; pass < Protocol.numPasses( state); pass++)
		{
			MeshPassInfo &passInfo = Protocol.peekPassInfo( state, pass);

			if( passInfo.mPassFlags & MeshPassInfo::PASS_FLAG_DYNAMIC_TEXTURE)
				modelHasFlags |= MODEL_HAS_DYNAMIC_TEXTURES;

			if( passInfo.mPassFlags & MeshPassInfo::PASS_FLAG_ENVIRONMENT_MAPPED)
				modelHasFlags |= MODEL_HAS_ENVIRONMENT_MAP;
		}
	}

	return modelHasFlags;
}

VSTtoDMABlock * DMAObjectFactory::getConvertor( MeshInterface &Protocol )
{
	VSTtoDMABlock *Convertor;

	if( Protocol.numColorSets( ) > 1)
	{
		static VSTtoDblSTPColorSets colorSetConverter;
		Convertor = &colorSetConverter;
	}
	else
	{
		static VSTtoDblVSTandZ32 StaticConverter;
		Convertor = &StaticConverter;
	}

	return Convertor;
}

MemBlock ** DMAObjectFactory::buildStaticDMABlockArray( MeshInterface &Protocol, unsigned int flagsCreate, unsigned int &numMemBlocks)
{
	unsigned int					hasFlags;
	unsigned int					allocedVUMemory;

	static MeshToMultiDraw		MD;

	assert( p_MemManager);

	setCreateFlags( flagsCreate);

	VSTtoDMABlock * p_Convertor = getConvertor( Protocol );

	if( !p_Convertor)
		return 0;

	StatesWriteInfo *pStatesWriteInfoArray = MD.buildStatesWriteInfo( Protocol, numMemBlocks, hasFlags);

	if( hasFlags & MODEL_HAS_ENVIRONMENT_MAP)
		allocedVUMemory = 48;
	else
		allocedVUMemory = d_StaticVUMemAllocated;

	p_Convertor->adjustVUMemUsage( allocedVUMemory, d_VUMemAvailable - allocedVUMemory);

	MemBlock **ppArray = (MemBlock **) malloc( numMemBlocks * sizeof( MemBlock *) );

	for( unsigned int currState = 0; currState < numMemBlocks; currState++)
	{
		ppArray[currState] = buildStaticDMABlock( Protocol, *p_Convertor, pStatesWriteInfoArray[currState] );
		
		addObjectInfo( * (DMAModelHdr *) ppArray[currState]->buffer( ), allocedVUMemory);
	}
	

	MD.freeStatesWriteInfo( pStatesWriteInfoArray, numMemBlocks);

	return ppArray;
}

MemBlock ** DMAObjectFactory::buildBonedDMABlockArray( MeshInterface &Protocol, unsigned int flagsCreate, unsigned int &numMemBlocks)
{
	unsigned int					allocatedVUMemory;
	unsigned int					hasFlags = buildHasFlags( Protocol);

	VSTtoANMDblBuffer *bonedConvertor = new VSTtoANMDblBuffer( true);
	

	if( hasFlags & MODEL_HAS_ENVIRONMENT_MAP)
		allocatedVUMemory = 48;
	else
		allocatedVUMemory = d_BonedVUMemAllocated;

	bonedConvertor->adjustVUMemUsage( allocatedVUMemory, d_VUMemAvailable - allocatedVUMemory);

	assert( p_MemManager);

	numMemBlocks = 1;
	
	MemBlock **ppArray = (MemBlock **) malloc( numMemBlocks * sizeof( MemBlock *) );

	ppArray[0] = buildBonedDMABlock( Protocol, *bonedConvertor, 0, Protocol.numStates( ), hasFlags );

	addObjectInfo( * (DMAModelHdr *) ppArray[0]->buffer( ), allocatedVUMemory );

	delete bonedConvertor;
	
	return ppArray;
}

MemBlock* DMAObjectFactory::buildStaticDMABlock( MeshInterface &Protocol, VSTtoDMABlock &Convert, StatesWriteInfo &WriteInfo)
{
unsigned int			writeBytes;

unsigned int			d_DMABufferWords, d_DMABufferBytes;

ostream *				p_DMAStream;
MemBlock *				p_DMAMemBlock;

	assert( p_MemManager);

	d_DMABufferWords = Convert.calcDMAModelDataSize( Protocol, WriteInfo);

	d_DMABufferBytes	= d_DMABufferWords * 4;

	p_DMAMemBlock = p_MemManager->allocBlock( d_DMABufferBytes);

	p_DMAStream = new ostrstream( (char *) p_DMAMemBlock->buffer( ), d_DMABufferBytes, ios::out | ios::binary);

	writeBytes = Convert.writeDMAModelData( Protocol, *p_DMAStream, true, WriteInfo);

	assert( writeBytes == d_DMABufferBytes);

	delete p_DMAStream;

	return p_DMAMemBlock;
}

MemBlock* DMAObjectFactory::buildBonedDMABlock( MeshInterface &Protocol, VSTtoDMABlock &Convert, unsigned int startState, unsigned int endState, unsigned int modelFlags )
{
unsigned int			writeBytes;

unsigned int			d_DMABufferWords, d_DMABufferBytes;

ostream *				p_DMAStream;
MemBlock *				p_DMAMemBlock;

	assert( p_MemManager);

	d_DMABufferWords = Convert.calcDMAModelDataSize( Protocol, startState, endState, modelFlags );
	d_DMABufferBytes = d_DMABufferWords * 4;
	
	p_DMAMemBlock = p_MemManager->allocBlock( d_DMABufferBytes);

	p_DMAStream = new ostrstream( (char *) p_DMAMemBlock->buffer( ), d_DMABufferBytes, ios::out | ios::binary);

	writeBytes = Convert.writeDMAModelData( Protocol, *p_DMAStream, true, startState, endState, modelFlags );

	assert( writeBytes == d_DMABufferBytes);

	delete p_DMAStream;

	return p_DMAMemBlock;
}


void DMAObjectFactory::addObjectInfo( DMAModelHdr &Hdr, unsigned int VUMemAllocated)
{
	Hdr.mFlags |= (( VUMemAllocated & 0xff) << 24);
}