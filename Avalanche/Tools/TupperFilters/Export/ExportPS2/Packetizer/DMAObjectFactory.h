#ifndef __DMA_OBJECT_FACTORY_H
#define __DMA_OBJECT_FACTORY_H

#include <ostream>
using namespace			std;


#include "MemManager.h"
#include "MeshInterface.h"
#include "VSTtoDMABlock.h"
#include "Display/PS2/Renderer/Common/DMAObject.h"
#include "Display/PS2/Renderer/Common/VUDataHeaders.h"

class DMAObjectFactory
{
	MemManager *			p_MemManager;

	unsigned int			d_VUMemAvailable;

	unsigned int			d_StaticVUMemAllocated;
	unsigned int			d_BonedVUMemAllocated;

	unsigned int			d_CreateFlags;

	void						setCreateFlags( unsigned int _CreateFlags) { d_CreateFlags = _CreateFlags; }
	void						addObjectInfo( DMAModelHdr &Hdr, unsigned int VUMemAllocated);

	VSTtoDMABlock *		getConvertor( MeshInterface &Protocol );

	MemBlock *				buildStaticDMABlock( MeshInterface &Protocol, VSTtoDMABlock &Convert, StatesWriteInfo &WriteInfo);
	MemBlock *				buildBonedDMABlock( MeshInterface &Protocol, VSTtoDMABlock &Convert, unsigned int startState, unsigned int endState, unsigned int modelFlags);

	unsigned int			buildHasFlags( MeshInterface &Protocol );

public:
	enum
	{
		// passed in create flags
		CREATE_BONED_LIT	= 0x00000001,

		// these are generated and used for misc purposes
		MODEL_HAS_DYNAMIC_TEXTURES = 0x00010000,
		MODEL_HAS_ENVIRONMENT_MAP	= 0x00020000
	};

	DMAObjectFactory( );

	void						setMemManager( MemManager &Manager);
	void						setVUMemUsage( unsigned int _MemAvailable);

	MemBlock **				buildStaticDMABlockArray( MeshInterface &Protocol, unsigned int flagsCreate, unsigned int &numMemBlocks);
	MemBlock **				buildBonedDMABlockArray( MeshInterface &Protocol, unsigned int flagsCreate, unsigned int &numMemBlocks);

} ;

#endif