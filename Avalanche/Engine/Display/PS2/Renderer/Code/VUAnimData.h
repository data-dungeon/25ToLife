#ifndef __VUANIMDATA_H_INCLUDED__
#define __VUANIMDATA_H_INCLUDED__

#include "VUManagedCode.h"
#include "VUDataClass.h"
#include "GIFTag.h"
#include "DMAObject.h"
#include "AnimDDMAObject.h"
#include "CompilerControl.h"

class VUAnimData : public VUDataClass
{
	GIFTag					d_GIFTag, d_ShadowerTag;
		
	bool						bPersistantInited;
	unsigned int *			pPersistantBasic;
	unsigned int *			pPersistantLit;
	
	unsigned int *			buildPersistantTable( unsigned int* table, const VUManagedCode &Code, unsigned int *pOffsetTable, unsigned int szeOffsetTable);
	
	enum ALInstType
	{
		AL_INST_FIRST = 0,		// first instruction in VertexData stream
			AL_INST_LAST,				// last instuction in VertexData stream
			AL_INST_OTHER				// any other instruction
	} ;
	
	void						fixupMatrixDMAChain( unsigned int *pMatrixChain, unsigned int *pOriginalChain, void *pMatrixBuffer, unsigned int mtxEntrySize);
	void						fixupGeometryChain( DMAModelHdr * dataBuffer, unsigned int instFixupOffset, bool bLit);
	
	unsigned int 			fixupAnimLitAddress( unsigned int *pVUInstructionTable, unsigned int inst, ALInstType instType);

	void 						transformMatrixBufferLights( int mtxCount, void *pMatrixBuffer, sceVu0FMATRIX LightMtx, sceVu0FMATRIX PointLightMtx );

	void 						validateTables( void );

public:
	VUAnimData( );
	virtual void 			init( VUCodeSetupHelper & helper);

	// add an object to the master dma chain
	virtual void 			prepDMA( DDMAObject &Model, const PS2Matrix4x4& Mtx1, const PS2Matrix4x4& Mtx2, const VURenderInfo& rndInfo);
	virtual void 			getRequirementsFlags( DDMAObject &Model, VURenderInfo &rndInfo);

} ;


#endif
