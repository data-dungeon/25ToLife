// all of these sizes should be in qwords
#define BONED_MATRIX_REGS						4
#define BONED_MATRIX_CACHE						32
#define BONED_GLOBAL_HEADER_SIZE 			0
//#define BONED_MATRIX_CACHE_START				(BONED_GLOBAL_HEADER_SIZE)
#define BONED_BUFFER_HEADER_SIZE				2
#define BONED_INSTRUCTION_STREAM_SIZE		32
#define BONED_MATRIX_CACHE_SIZE 				(BONED_MATRIX_CACHE * 4)

//how many buffers need to pass before a matrix can be reused, plus one.
// a lifetime of 2 indicates that the next buffer can't use our matrices
#define MATRIX_LIFETIME 2

#define INSTR_CACHETOREG	0		//cache slot, reg slot
#define INSTR_TRANSFORM		1		//vtx count, first vertidx
#define INSTR_KICK			2		//no args

#define INSTR_INSTR        0
#define INSTR_DATA1        1
#define INSTR_DATA2        2
#define INSTR_DATA3        3

#define MTX_IDX            0
#define MTX_CACHESLOT      1

//#define NUM_LIGHT_VECTORS 7
#define NUM_LIGHT_VECTORS	9

using namespace VUMicro;

#include "Display/PS2/Renderer/Common/VUDataHeaders.h"
#include "DMAObjectFactory.h"
class AnmVert
{
public:
	AnmVert *pNext;
	int m_idx;
	int m_mtxtransform;
	int m_regtransform;
	bool m_breaktransform;
	bool m_adc;

	AnmVert( );

	static void CleanChain( AnmVert *pChain );
	static AnmVert *Tail( AnmVert *pChain );
} ;

AnmVert::AnmVert( )
{
	pNext = NULL;  m_idx = -1; m_mtxtransform = -1; m_regtransform = -1;
	m_breaktransform = false; m_adc = false;
}

void AnmVert::CleanChain( AnmVert *pChain )
{
	AnmVert *pNV;

	while( pChain )
	{
		pNV = pChain->pNext;
		delete pChain;
		pChain = pNV;
	}
}

AnmVert *AnmVert::Tail( AnmVert *pChain )
{
	AnmVert *tail = NULL;
	if( pChain )
	{
		tail = pChain;
		while( tail->pNext )
			tail = tail->pNext;
	}
	return tail;
}

class AnmBuffer
{
private:
	static int TotalVertices;
	static int TotalWordCount;
	static int MatrixChainSize;
	static int Matrices[BONED_MATRIX_CACHE];
	static int MatrixInUse[BONED_MATRIX_CACHE];
	static int Regs[BONED_MATRIX_REGS];
	static int MaxRegs;
	static int MaxCached;
	static int MinVerts;
	static int MatrixQWCount;
	static bool Lightable;
	static unsigned int ConversionFlags;
	
	AnmBuffer *pNext;

	AnmVert *pVerts;

	int m_startvert, m_verts;

	int m_wordcount;
	int m_instruction, m_maxinstruction;
	int m_input, m_maxinput;
	int m_output, m_maxoutput;
	int m_matrix, m_maxmatrix;

	// the following members are for assistance in figuring out what to write, since
	// while we're scanning though figuring out the size, we get most of the information
	// anyway
	int m_matrixdma[BONED_MATRIX_CACHE][2]; //matrixidx, cacheslot
	int m_matrixdmapos;
	int m_istream[BONED_INSTRUCTION_STREAM_SIZE][4]; //instr, data1, data2, data3
	int m_istreampos;

	int m_geodmasize;

	// these are used while writing, to record positions in the stream that we don't
	// yet know what to fill them with
	int m_matrixfixup; // one for each matrix buffer

	unsigned int m_stateidx;
	unsigned int m_statesize;
	int          m_pass;

	int m_pad;

	static AnmBuffer BackupCopy;

public:
	static AnmBuffer *pBuffers;

	AnmBuffer() {};
	AnmBuffer( int vertidx, bool bLightable );
	~AnmBuffer();

	static void writeNonLitGeomStream( AnmBuffer *pAB, MeshInterface &Protocol, ostream &stream);
	static void writeLitGeomStream( AnmBuffer *pAB, MeshInterface &Protocol, ostream &stream);
	
	bool StripEnd( MeshInterface &Protocol );
	void AddState( int stateidx, VUStateProtocol &state, int pass );
	void SetState( int stateidx, int pass );
	int AddVerts( MeshInterface &Protocol, int vtxidx, int nverts = -1, int recursion = 0 );

	int ChooseSpot( MeshInterface &Protocol, int vtxidx, int *slots, int nslots, int *usage );
	int ChooseCacheSpot( MeshInterface &Protocol, int vtxidx );
	int ChooseRegSpot( MeshInterface &Protocol, int vtxidx );

	bool IsADC( int vtxidx );
	int GetRegTransform( int vtxidx );
	AnmVert *FindVertByIndex( int vtxidx );
	int GetLoadCacheInstruction( int regidx );
	int GetTransformInstruction( int regidx, int vtxidx );
	int GetFinishInstruction( void );
	int GetNextInstruction( void );

	static void Cleanup( void );
	static void SetConversionFlags( unsigned int uiFlags);
	static int TotalSize( void );
	static int Write( MeshInterface &Protocol, ostream &stream, int startState, int runStates );
	static void Backup( AnmBuffer &ab );
	static AnmBuffer &Restore( void );

	void DebugInfo( void );
};

AnmBuffer *AnmBuffer::pBuffers = NULL;
int AnmBuffer::TotalVertices;
int AnmBuffer::TotalWordCount;
int AnmBuffer::MatrixChainSize;
int AnmBuffer::Matrices[BONED_MATRIX_CACHE];
int AnmBuffer::MatrixInUse[BONED_MATRIX_CACHE];
int AnmBuffer::Regs[BONED_MATRIX_REGS];
int AnmBuffer::MaxRegs;
int AnmBuffer::MaxCached;
int AnmBuffer::MinVerts;
int AnmBuffer::MatrixQWCount;
unsigned int AnmBuffer::ConversionFlags;
bool AnmBuffer::Lightable;
AnmBuffer AnmBuffer::BackupCopy;
static int B_TotalWordCount;
static int B_MatrixChainSize;
static int B_Matrices[BONED_MATRIX_CACHE];
static int B_MatrixInUse[BONED_MATRIX_CACHE];
static int B_Regs[BONED_MATRIX_REGS];
static int B_MaxRegs;
static int B_MaxCached;
static int B_MinVerts;
static int B_MatrixQWCount;
static bool B_Lightable;

void AnmBuffer::Backup( AnmBuffer &ab )
{
	int i;
	BackupCopy = ab;
	B_TotalWordCount = TotalWordCount;
	B_MatrixChainSize = MatrixChainSize;
	for( i = 0; i < BONED_MATRIX_CACHE; i++ )
	{
		B_Matrices[i] = Matrices[i];
		B_MatrixInUse[i] = MatrixInUse[i];
	}
	for( i = 0; i < BONED_MATRIX_REGS; i++ )
		B_Regs[i] = Regs[i];
	B_MaxRegs = MaxRegs;
	B_MaxCached = MaxCached;
	B_MinVerts = MinVerts;
	B_MatrixQWCount = MatrixQWCount;
	B_Lightable = Lightable;
}

AnmBuffer &AnmBuffer::Restore( void )
{
	int i;
	TotalWordCount = B_TotalWordCount;
	MatrixChainSize = B_MatrixChainSize;
	for( i = 0; i < BONED_MATRIX_CACHE; i++ )
	{
		Matrices[i] = B_Matrices[i];
		MatrixInUse[i] = B_MatrixInUse[i];
	}
	for( i = 0; i < BONED_MATRIX_REGS; i++ )
		Regs[i] = B_Regs[i];
	MaxRegs = B_MaxRegs;
	MaxCached = B_MaxCached;
	MinVerts = B_MinVerts;
	MatrixQWCount = B_MatrixQWCount;
	Lightable = B_Lightable;
	return BackupCopy;
}

void AnmBuffer::DebugInfo( void )
{
static int Cache[BONED_MATRIX_CACHE] = { -1 };
static int Regs[4] = { -1 };

	int i;

	PluginLog::Add( LogFlags::VERBOSE_LEVEL_HIGH, "NEW BUFFER" );
	PluginLog::Add( LogFlags::VERBOSE_LEVEL_HIGH, "%d verts beginning at idx %d", m_verts, m_startvert );
	PluginLog::Add( LogFlags::VERBOSE_LEVEL_HIGH, "%d matrices into cache slot", m_matrixdmapos );
	for( i = 0; i < m_matrixdmapos; i++ )
	{
		PluginLog::Add( LogFlags::VERBOSE_LEVEL_HIGH, "   matrix %d into cache %d", m_matrixdma[i][0], m_matrixdma[i][1] );
		Cache[m_matrixdma[i][1]] = m_matrixdma[i][0];
	}

	PluginLog::Add( LogFlags::VERBOSE_LEVEL_HIGH, "%d instructions", m_istreampos );
	for( i = 0; i < m_istreampos; i++ )
	{
		switch( m_istream[i][0] )
		{
			case INSTR_CACHETOREG:
				PluginLog::Add( LogFlags::VERBOSE_LEVEL_HIGH, "   CACHETOREG: cache slot %d matrix %d to reg %d", m_istream[i][1], Cache[m_istream[i][1]], m_istream[i][2] );
				Regs[m_istream[i][2]] = Cache[m_istream[i][1]];
				break;
			case INSTR_TRANSFORM:
				PluginLog::Add( LogFlags::VERBOSE_LEVEL_HIGH, "        REGS: %02d %02d", Regs[0], Regs[1] );
				PluginLog::Add( LogFlags::VERBOSE_LEVEL_HIGH, "   TRANSFORM: %d verts beginning with %d", m_istream[i][1], m_istream[i][2] );
				break;
			case INSTR_KICK:
				PluginLog::Add( LogFlags::VERBOSE_LEVEL_HIGH, "   KICK" );
				break;
		}
	}
	AnmVert *pV = pVerts;
	int v=0;
	while( pV )
	{
		if( pV->m_breaktransform )
			PluginLog::Add( LogFlags::VERBOSE_LEVEL_HIGH, "      Transform broken by vert %d", v );
		v++;
		pV = pV->pNext;
	}
}

bool AnmBuffer::IsADC( int vtxidx )
{
	// so we can always get the previous vert's adc bit.  even the first vert's "previous"
	// vert
	if( vtxidx < 0 ) return false;

	AnmVert *pV = FindVertByIndex( vtxidx );
	assert( pV != NULL && "searching for a vertex index that doesn't exist" );
	return pV->m_adc;
}

int AnmBuffer::GetRegTransform( int vtxidx )
{
	AnmVert *pV = FindVertByIndex( vtxidx );
	assert( pV != NULL && "searching for a vertex index that doesn't exist" );
	return pV->m_regtransform;
}

AnmVert *AnmBuffer::FindVertByIndex( int vtxidx )
{
	AnmVert *pV = pVerts;
	int i;
	for( i = 0; i < vtxidx && pV; i++ )
	{
		pV = pV->pNext;
	}
	return pV;
}

int AnmBuffer::GetLoadCacheInstruction( int regidx )
{
	int baseinstr;
	if( Lightable )
		baseinstr = ANIML_LOAD_CACHE_TO_MTX_1;
	else
		baseinstr = ANIM_LOAD_CACHE_TO_MTX_1;
	return baseinstr + regidx;
}

enum { LOOKUP_START = 0, LOOKUP_CONT_NW, LOOKUP_CONT, LOOKUP_CONT_NR };
unsigned int instLitLookupTable[2][LOOKUP_CONT_NR + 1] =
{
	{ ANIML_TRANSFORM_MTX_1_START, ANIML_TRANSFORM_MTX_1_CONT_NW,
			ANIML_TRANSFORM_MTX_1_CONT, ANIML_TRANSFORM_MTX_1_CONT_NR },
	{ ANIML_TRANSFORM_MTX_2_START, ANIML_TRANSFORM_MTX_2_CONT_NW,
	ANIML_TRANSFORM_MTX_2_CONT, ANIML_TRANSFORM_MTX_2_CONT_NR }
};

int AnmBuffer::GetTransformInstruction( int regidx, int vtxidx )
{
	int instr;

	if( Lightable )
	{
		assert( regidx < 2 && "There are only 2 matrices in registers at a time in lit models" );
		if( vtxidx == 0 ) //First vert, start
			instr = instLitLookupTable[regidx][LOOKUP_START];
		else if( vtxidx == 1 ) // second vert, NoWrite
			instr = instLitLookupTable[regidx][LOOKUP_CONT_NW];
		else if( vtxidx == (m_verts - 1) ) //last vert, NoRead
			instr = instLitLookupTable[regidx][LOOKUP_CONT_NR];
		else if( vtxidx == (m_verts) ) // past the end, Finish
			instr = ANIML_TRANSFORM_FINISH;
		else // none of the above, regular Continue
			instr = instLitLookupTable[regidx][LOOKUP_CONT];

		// adc version? there's not room to pack the adc bit in with the vertices, so
		// the animated lit code has two separate code paths.  one for adc verts, one for
		// non-adc verts
		if( IsADC( vtxidx - 1 ) )
			instr++;
	}
	else
	{
		assert( regidx < 4 && "There are only 4 matrices in registers for normal animate modesl" );
		if( vtxidx == 0 ) //First vert, start
			instr = ANIM_TRANSFORM_MTX_1_START + regidx;
		else if( vtxidx == 1 ) // second vert, NoWrite
			instr = ANIM_TRANSFORM_MTX_1_CONT_NW + regidx;
		else if( vtxidx == (m_verts - 1) ) //last vert, NoRead
			instr = ANIM_TRANSFORM_MTX_1_CONT_NR + regidx;
		else if( vtxidx == (m_verts) ) // past the end, Finish
			instr = ANIM_TRANSFORM_FINISH;
		else // none of the above, regular Continue
			instr = ANIM_TRANSFORM_MTX_1_CONT + regidx;
	}
	return instr;
}

int AnmBuffer::GetNextInstruction( void )
{
	if( Lightable )
		return ANIML_LOAD_NEXT_INST;
	return ANIM_LOAD_NEXT_INST;
}

int AnmBuffer::GetFinishInstruction( void )
{
	if( Lightable )
		return ANIML_FINISH;
	return ANIM_FINISH;
}

void AnmBuffer::SetConversionFlags( unsigned int uiFlags)
{
	ConversionFlags = uiFlags;
}

int AnmBuffer::Write( MeshInterface &Protocol, ostream &stream, int startState, int runStates )
{
	unsigned int		padWord = 0;
//	unsigned int		GIFTagFixupOffset;
	unsigned int		texFixupWritePos = 0;
	unsigned int		texFixupLastOffset = 0;

	unsigned int		instFixupWritePos = 0;
	unsigned int		instFixupLastOffset = 0;
	unsigned int		instFixupData = 0;

	unsigned int		totalMatrixDMAs = 0;
	unsigned int		totalStates = 0;
	unsigned int		DTNextEntry, DTTablePos;

	AnmBuffer 			*pAB, *pFirstBuffer;

	int 					endState = startState + runStates;
	
	TotalVertices = 0;

	// find first buffer that falls into the state requirements
	pAB = pBuffers;
	pFirstBuffer = 0;
	
	while( pAB )
	{
		if( (pAB->m_statesize && pAB->m_stateidx == startState )  &&  pFirstBuffer == 0)
		{
			pFirstBuffer = pAB;
		}
		
		if( pFirstBuffer)
		{
			pAB->DebugInfo( );
		
			totalMatrixDMAs += pAB->m_matrixdmapos;
			
			if( pAB->m_statesize)
				totalStates++;
		}
				
		pAB = pAB->pNext;
	}

	
	// NOTE: keep in mind that at this point we're writing the stream in a format that
	// will be exportable.  Fixup will happen elsewhere, so we need to make sure we store
	// enough information to do all the necessary fixup.  We also need to make sure we
	// can duplicate the matrix part of the dma chain and fix it up for multiple instances.
	// In fact, each instance needs 2 copies of the matrix chain to start with, since
	// matrices will be double buffered.

	// TJC - old comment
	// global header info: (4 words)
	//		class id
	//		pad
	//		pad
	//		pad
	unsigned int hdrPos = stream.tellp( );
	texFixupWritePos = (unsigned int) stream.tellp( ) + offsetof( AnimDMAModelHdr, mOffsetToFirstTexture);
	instFixupWritePos = (unsigned int) stream.tellp( ) + offsetof( AnimDMAModelHdr, mOffsetToInstructionFixup);

	AnimDMAModelHdr 			Header;

	Header.mFixupClassID = FIXUP_ANIMATED;
	Header.mDataClassID = DATA_ANIMATED;

	Header.mFlags = 0;
	
	Header.mPRIM = 0; 										// dont need a prim..
	if( ConversionFlags & DMAObjectFactory::MODEL_HAS_DYNAMIC_TEXTURES)
		Header.mFlags |= OBJ_FLAG_DYNAMIC_TEXTURE; 		// dont need any stinking flags
	
	if( ConversionFlags & DMAObjectFactory::MODEL_HAS_ENVIRONMENT_MAP)
		Header.mFlags |= OBJ_FLAG_ENVIRONMENT_MAPPED;

	Header.mVertCount = 0;

	Header.mOffsetToFirstTexture = 0;  					// filled in later
	Header.mOffsetToInstructionFixup = 0;				// filled in later

	Header.mDMAChainSize = MatrixChainSize;

	Header.mMatrixCount = Protocol.numMatrices( );
	Header.mMatrixDMACount = totalMatrixDMAs;

	Header.mMatrixDMASize = MatrixQWCount;	

	stream.write( (char *) &Header, sizeof(AnimDMAModelHdr) );
	
	if( ConversionFlags & DMAObjectFactory::MODEL_HAS_DYNAMIC_TEXTURES )
	{
		Header.mOffsetToFirstTexture = (unsigned int) stream.tellp( ) - hdrPos;
		stream.write( (char *) &totalStates, 4);
		
		DTNextEntry = DTTablePos = stream.tellp( );
		
		stream.seekp( totalStates * sizeof( DTInfo), ios::cur);
	}
	
		
	// we're going to track which matrices are changed so the code can
	// multiply in the perspective matrix.

	unsigned int chainDMAPos = (unsigned int) stream.tellp( ) - hdrPos;
	
	// write matrix dmas
	pAB = pFirstBuffer;
	while( pAB && ( pAB->m_statesize == 0 || pAB->m_stateidx < (unsigned int) endState ) )
	{
		if( (ConversionFlags & DMAObjectFactory::MODEL_HAS_DYNAMIC_TEXTURES)  &&  pAB->m_statesize)
		{
			if( pAB != pFirstBuffer)
			{
				padWord = -2;
				stream.write( (char *) &padWord, 4 );
				stream.write( (char *) &padWord, 4 );

				// vifNOPs
				stream << createNOP( );
				stream << createNOP( );
			}
				
			unsigned int currPos = stream.tellp( );
				
			stream.seekp( DTNextEntry);
				
			VUStateProtocol &State = Protocol.activateState( pAB->m_stateidx, pAB->m_pass );
		
			unsigned int uiTexID = State.getDynamicTextureID( );
			unsigned int uiBlockOffset = currPos - chainDMAPos;
				
			stream.write( (char *) & uiTexID, 4);
			stream.write( (char *) & uiBlockOffset, 4);
			
			DTNextEntry = stream.tellp( );
				
			stream.seekp( currPos);
		}
			
		// we're not actually building dmatags here.  that will happen on fixup.
		// Instead, we're storing the matrix index and the offset to our geometry block in
		// the first 2 words.  The third word will be either a VIFNOP or SETCYCL,
		// and the fourth word will be an unpack

		// 2 copies for double buffered matrices
		// use the following format:
		//		word 0: offset to geometry block
		//		word 1: matrix index
		//		word 2: setcycl(4,4)
		//		word 3: unpack matrix into cache
		for( int mtx = 0; mtx < pAB->m_matrixdmapos; mtx++ )
		{
			padWord = 0;
			// we check the bits for safety
			stream.write( (char *) &padWord, 4 );
			stream.write( (char *)&pAB->m_matrixdma[mtx][0], 4 ); // mtx index
			stream << VIFCreator::createSTCYCL( 4, 4 );
			stream << VIFCreator::createUNPACK( VSTtoANMDblBuffer::VUMemBase + (pAB->m_matrixdma[mtx][1] * MatrixQWCount),
					V4_32, MatrixQWCount, 0 );
		}

		// tag for the CALL.  indicated by a matrix index of -1
		pAB->m_matrixfixup = stream.tellp( );

		padWord = -1;
		// no matrices into cache, but we still need a dmatag to call the geometry with
		stream.write( (char *) &padWord, 4 );
		stream.write( (char *) &padWord, 4 );

		// vifNOPs
		stream << createNOP( );
		stream << createNOP( );

		pAB = pAB->pNext;
	}
	
	// final dma tag to end it all.  technically, we could set the last geometry
	// dmatag to END instead of RET, but this is much simpler
	// actual construction of the DMATag happens at import time, though.  We just need
	// to indicate that this is the last one.
	padWord = -3;
	stream.write( (char *) &padWord, 4 );
	stream.write( (char *) &padWord, 4 );

	// vifNOPs
	stream << createNOP( );
	stream << createNOP( );

	unsigned int currPos = (unsigned int) stream.tellp( ) - hdrPos;
	assert( currPos % 4 == 0);

	while( currPos % 16 )
	{
		padWord = 0;
		
		stream.write( (char *) &padWord, 4);
		currPos = (unsigned int) stream.tellp( ) - hdrPos;
	}
	
	// write geometry dma
	DMATag						geomDMATag( 0, PCE_NOTHING, ID_RET, 0, 0);

	pAB = pFirstBuffer;
	while( pAB && ( pAB->m_statesize == 0 || pAB->m_stateidx < (unsigned int) endState ) )
	{
		// give the geometry offset to both matrix dma chains
		unsigned int currVUOffset = 0;
		unsigned int currPos = stream.tellp( );

		unsigned int geometryOffset = currPos - pAB->m_matrixfixup;

		stream.seekp( pAB->m_matrixfixup );
		stream.write( (char *) &geometryOffset, 4 );

		stream.seekp( currPos );

		// write DMATag for this block
		geomDMATag.setQWC( pAB->m_geodmasize - 1 );

		// calculate dirtymatrix info for this buffer
		unsigned int dirtymatrixbits = 0;
		unsigned int dirtymatrixcount = 0;
		for( int mtx = 0; mtx < pAB->m_matrixdmapos; mtx++ )
			if( !(dirtymatrixbits & (1 << pAB->m_matrixdma[mtx][1])))
			{
				dirtymatrixbits |= (1 << pAB->m_matrixdma[mtx][1]);
				dirtymatrixcount++;
			}

		stream << geomDMATag;
		stream << VIFCreator::createUNPACK( currVUOffset, V4_32, BONED_BUFFER_HEADER_SIZE, 1 ); 
		currVUOffset += BONED_BUFFER_HEADER_SIZE;
		
		MeshPassInfo &passInfo = Protocol.peekPassInfo(pAB->m_stateidx , pAB->m_pass );
		unsigned int stateData = pAB->m_statesize;

		if( passInfo.mPassFlags & MeshPassInfo::PASS_FLAG_ENVIRONMENT_MAPPED )
			stateData |= (OBJ_FLAG_ENVIRONMENT_MAPPED << 8);

		// header QW 1
		// 0 : matrix cache start
		// 1 : dirtybits count
		// 2 : dirtybits  0 - 15 (PS2 VU1 ints are 16bits :( )
		// 3 : dirtybits 16 - 31
		unsigned int header1info;
		
		header1info = VSTtoANMDblBuffer::VUMemBase;
		stream.write( (char *) &header1info, 4);
		stream.write( (char *) &dirtymatrixcount, 4);
		header1info = dirtymatrixbits & 0xffff;
		stream.write( (char *) &header1info, 4);
		header1info = (dirtymatrixbits >> 16) & 0xffff;
		stream.write( (char *) &header1info, 4);

		// header QW 2
		// 0 : numVerts
		// 1 : Input Buffer Offset
		// 2 : Output Buffer Offset
		// 3 : State Flags | State Packet Size
		stream.write( (char *)&pAB->m_verts, 4 );
		stream.write( (char *)&VSTtoANMDblBuffer::BufferInputOffset, 4 );
		stream.write( (char *)&VSTtoANMDblBuffer::BufferGiftagOffset, 4 );
		stream.write( (char *)&stateData, 4 );

		// write the state + fixup information
		if( pAB->m_statesize )
		{
			VUStateProtocol &State = Protocol.activateState( pAB->m_stateidx, pAB->m_pass );
				
			if( !(ConversionFlags & DMAObjectFactory::MODEL_HAS_DYNAMIC_TEXTURES) )
			{
				stream << VIFCreator::createUNPACK( currVUOffset, V4_32, pAB->m_statesize, 1 );
				
				// write packet
				State.write( stream );

				if( Header.mOffsetToFirstTexture == 0)
					Header.mOffsetToFirstTexture = State.getLastPacketStartPos( ) - hdrPos;
			}
			currVUOffset += pAB->m_statesize;
		}

		stream << VIFCreator::createUNPACK( currVUOffset, V4_32, pAB->m_istreampos, 1 );

		// instruction stream fixup
		unsigned int currInstPos = stream.tellp( );
		unsigned int instFixupDelta = currInstPos - instFixupLastOffset;
		
		instFixupData = (instFixupData & 0xffff0000) | instFixupDelta;

		if( Header.mOffsetToInstructionFixup == 0)
		{
			Header.mOffsetToInstructionFixup = instFixupData;
		}
		else
		{
			stream.seekp( instFixupWritePos);
			stream.write( (char *) &instFixupData, 4);
			stream.seekp( currInstPos );
		}

		pAB->m_istream[0][3] = instFixupData = (pAB->m_verts << 24) | (pAB->m_istreampos << 16);

		instFixupWritePos = currInstPos + 12;
		instFixupLastOffset += instFixupDelta;

		// instruction stream
		// instructions are a bit bizarre.
		for( unsigned int i = 0; i < (unsigned int)pAB->m_istreampos; i++ )
		{
			// LOOKUP INSTRUCTION HERE
			switch( pAB->m_istream[i][0] )
			{
				int 						regIdx;

				case 0: // cachetoreg
					// regidx is stored in data2
					regIdx = pAB->m_istream[i][2];
					pAB->m_istream[i][0] = pAB->GetLoadCacheInstruction( regIdx );
					// the vucode actually expects the matrix cacheslot to be in the
					// third word of the instruction, not the second, which is where we've
					// got it now
					pAB->m_istream[i][2] = VSTtoANMDblBuffer::VUMemBase + (pAB->m_istream[i][1] * MatrixQWCount); // cacheslot vu address
					break;
				case 1: // transform
					regIdx = pAB->GetRegTransform( pAB->m_istream[i][2] );
					pAB->m_istream[i][0] = pAB->GetTransformInstruction( regIdx, pAB->m_istream[i][2] );
					break;
				case 2: // finish and kick
					pAB->m_istream[i][0] = pAB->GetFinishInstruction( );
					break;
				default:
					pAB->m_istream[i][0] = 0xbeef | ((i+1) << 16);
					pAB->m_istream[i][1] = 0xdead | ((i+1) << 16);
					pAB->m_istream[i][2] = 0xcddc | ((i+1) << 16);
					break;
			}

			stream.write( (char *) pAB->m_istream[i], 16 );
			assert( (int) stream.tellp( ) != -1 );
		}

		// input geometry, 2 qwords per vertex

		// need an unpack for the geometry
		stream << VIFCreator::createUNPACK( VSTtoANMDblBuffer::BufferInputOffset, V4_32, pAB->m_verts * 2, 1 );
		
		if( Lightable )
			writeLitGeomStream( pAB, Protocol, stream);
		else
			writeNonLitGeomStream( pAB, Protocol, stream);
		
		padWord = 0; // MSCAL or MSCNT
      stream.write( (char *) &padWord, 4 );

		for( unsigned int j = 0; j < (unsigned int) pAB->m_pad; j++ )
			stream << VIFCreator::createNOP( );

		pAB = pAB->pNext;
	}

	Header.mVertCount = TotalVertices;

	unsigned int endPos = stream.tellp( );

	stream.seekp( hdrPos);
	stream.write( (char *) &Header, sizeof(AnimDMAModelHdr) );

	stream.seekp( endPos);

	return (unsigned int) endPos - hdrPos;
}

void AnmBuffer::writeNonLitGeomStream( AnmBuffer *pAB, MeshInterface &Protocol, ostream &stream)
{
	float 			vertdata[4];
	AnmVert *		pV = pAB->pVerts;

	assert( sizeof(float) == 4 );

	MeshPassInfo &passInfo = Protocol.peekPassInfo(pAB->m_stateidx , pAB->m_pass );
	int uvStreamIndex = passInfo.idxTexCoordStream;
	int colorStreamIndex = passInfo.idxRGBAStream;
	
	while( pV )
	{
		TotalVertices++;

		// figure out the instruction for the next vert
		int instr = 0;

		if( pV->m_breaktransform )
		{
			if( pV->pNext )
				instr = pAB->GetNextInstruction( );
			else
				instr = pAB->GetTransformInstruction( 0, pAB->m_verts );
		}
		else
		{
			// if this assert triggers, something is screwy, because the last vert
			// in a buffer SHOULD have breaktransform set
			assert( pV->pNext );
			instr = pAB->GetTransformInstruction( pV->pNext->m_regtransform, pV->pNext->m_idx - pAB->pVerts->m_idx);
		}

		unsigned int *pi;

		// X, Y, Z, Next Instr,
		unsigned int vtxIdx = Protocol.idx( pV->m_idx) & MeshInterface::IDX_MASK;

		const Position3<float> vtx = Protocol.vtx( vtxIdx );

		vertdata[0] = vtx.X();
		vertdata[1] = vtx.Y();
		vertdata[2] = vtx.Z();
		pi = (unsigned int *) &vertdata[3];
		*pi = instr;
		
		unsigned int i = stream.tellp( );
		
		stream.write( (char *) vertdata, 16 );
		assert( (int)stream.tellp() != -1 );
		pV = pV->pNext;
	}
	
	pV = pAB->pVerts;
	
	while( pV)
	{
		unsigned int *pi;

		const Position2<float> st = Protocol.st( uvStreamIndex, pV->m_idx );
		const BaseColor<unsigned short> clr = Protocol.clr( colorStreamIndex, pV->m_idx );
		
		// S|R, T|G, B, ADC|A
		vertdata[0] = st.X();
		pi = (unsigned int *) &vertdata[0];
		*pi &= ~(0xff); *pi |= clr.bR();
		
		vertdata[1] = st.Y();
		pi = (unsigned int *) &vertdata[1];
		*pi &= ~(0xff); *pi |= clr.bG();
		
		vertdata[2] = 1.0f;
		pi = (unsigned int *) &vertdata[2];
		*pi &= ~(0xff); *pi |= clr.bB();
		
		if( pV->m_adc )
			vertdata[3] = 2048.0f;
		else
			vertdata[3] = 0.0f;
			
		pi = (unsigned int *) &vertdata[3];
		*pi &= ~(0xff); *pi |= clr.bA();

		unsigned int i = stream.tellp( );
      stream.write( (char *) vertdata, 16 );

		assert( (int) stream.tellp() != -1 );
		pV = pV->pNext;
	}
}

void AnmBuffer::writeLitGeomStream( AnmBuffer *pAB, MeshInterface &Protocol, ostream &stream)
{
	AnmVert *pV = pAB->pVerts;

	MeshPassInfo &passInfo = Protocol.peekPassInfo(pAB->m_stateidx , pAB->m_pass );
	int normalStreamIndex = passInfo.idxNormalStream;
	int uvStreamIndex = passInfo.idxTexCoordStream;

	while( pV )
	{
		TotalVertices++;

		// figure out the instruction for the next vert
		int instr = 0;

		if( pV->m_breaktransform )
		{
			if( pV->pNext )
				// jlb - wasn't sure if changing GetNextInstruction( ) would affect anything else
				instr = 	ANIML_TRANSFORM_PROCESS_INST;
			else
				instr = pAB->GetTransformInstruction( 0, pAB->m_verts );
		}
		else
		{
			// if this assert triggers, something is screwy, because the last vert
			// in a buffer SHOULD have breaktransform set
			assert( pV->pNext );
			instr = pAB->GetTransformInstruction( pV->pNext->m_regtransform, pV->pNext->m_idx - pAB->pVerts->m_idx);
		}

		assert( sizeof(float) == 4 );
		float vertdata[8]; // 8 words
		unsigned int *pi;

		// Vx, Vy, Vz, Nx | Inst
		unsigned int vtxIdx = Protocol.idx( pV->m_idx) & MeshInterface::IDX_MASK;
		
		const Position3<float> vtx = Protocol.vtx( vtxIdx );
		const Position3<float> nrm = Protocol.nrm( normalStreamIndex, pV->m_idx );
		const Position2<float> st = Protocol.st( uvStreamIndex, pV->m_idx );

		vertdata[0] = vtx.X();
		vertdata[1] = vtx.Y();
		vertdata[2] = vtx.Z();
		vertdata[3] = nrm.X();
		pi = (unsigned int *) &vertdata[3];
		*pi &= ~(0xff);
		*pi |= instr;
		
		// Ts, Tt, Ny, Nz
		vertdata[4] = st.X();
		vertdata[5] = st.Y();
		
		vertdata[6] = nrm.Y();
		vertdata[7] = nrm.Z();

		unsigned int i = stream.tellp( );
      stream.write( (char *)vertdata, 32 );

		assert( (int) stream.tellp() != -1 );
		pV = pV->pNext;
	}
}

int AnmBuffer::TotalSize( void )
{
	AnmBuffer *pB = pBuffers;
	int mtxcount;
	int returntags;

	MatrixChainSize = 0;
	TotalWordCount = 0;

	// global info header, not included in matrix chain
	TotalWordCount += sizeof(AnimDMAModelHdr) / 4;

	if( ConversionFlags & DMAObjectFactory::MODEL_HAS_DYNAMIC_TEXTURES )
		returntags = 0;
	else
		returntags = 1;
		
	// first count matrix dma chains
	mtxcount = 0;
	while( pB )
	{
		// one REF tag for each matrix going to the cache, plus a CALL tag
		mtxcount += pB->m_matrixdmapos + 1;
		if( (ConversionFlags & DMAObjectFactory::MODEL_HAS_DYNAMIC_TEXTURES)  &&  pB->m_statesize)
			returntags += 1;
		
		pB = pB->pNext;
	}
	
	if( ConversionFlags & DMAObjectFactory::MODEL_HAS_DYNAMIC_TEXTURES)
		TotalWordCount += 1 + returntags * 2;
		
	// one dmatag per matrix
	MatrixChainSize += 4 * mtxcount;
	// a final empty dma tag to end it all
	MatrixChainSize += 4 * returntags; 

	TotalWordCount += MatrixChainSize;

	if( TotalWordCount % 4)
		TotalWordCount += 4 - (TotalWordCount % 4);
		
	// now geometry chains
	pB = pBuffers;
	while( pB )
	{
		int WordCount = 0;

		WordCount += 4; // dmatag, unpack for header, unpack for istream)
		WordCount += 4 * BONED_BUFFER_HEADER_SIZE; // header
		
		if( !(ConversionFlags & DMAObjectFactory::MODEL_HAS_DYNAMIC_TEXTURES)  &&  pB->m_statesize)
			WordCount += 1 + (pB->m_statesize * 4); // state data and state giftag if present
			
		WordCount += 4 * pB->m_istreampos;

		WordCount++; // unpack for (input data)
		WordCount += 8 * pB->m_verts; // input data, 2 qwords per vert

		WordCount++; // mscal/cnt

		//pad to a 16 byte boundary if necessary
		if( WordCount % 4 != 0 )
			pB->m_pad = 4 - (WordCount % 4);
		else
			pB->m_pad = 0;
		WordCount += pB->m_pad;

		pB->m_geodmasize = WordCount / 4;
		TotalWordCount += WordCount;

		pB = pB->pNext;
	}

	pB = pBuffers;
	while( pB )
	{
		// go through and mark all the end transform points
		AnmVert *pV = pB->pVerts;
		for( int i = 0; i < pB->m_istreampos; i++ )
		{
			if( pB->m_istream[i][0] == INSTR_TRANSFORM )
			{
				for( int j = 0; j < pB->m_istream[i][1] - 1; j++ )
				{
					assert( pV );
					pV = pV->pNext;
				}
				assert( pV );
				pV->m_breaktransform = true;
				pV = pV->pNext;
			}
		}

		pB = pB->pNext;
	}

	return TotalWordCount;
}

void AnmBuffer::Cleanup()
{
	AnmBuffer *pB = pBuffers, *pNB;
	while( pB )
	{
		pNB = pB->pNext;
		delete pB;
		pB = pNB;
	}
	pBuffers = NULL;
}

bool AnmBuffer::StripEnd( MeshInterface &Protocol )
{
	AnmVert *pT = AnmVert::Tail( pVerts );
	if( Protocol.idx( pT->m_idx ) & MeshInterface::IDX_END_OF_STRIP )
		return true;
	return false;
}

AnmBuffer::AnmBuffer( int vertidx, bool bLightable )
{
	int i;

	Lightable = bLightable;
	pNext = NULL; m_startvert = m_verts = m_wordcount = m_instruction = m_maxinstruction = 0;
	m_input = m_maxinput = m_output = m_maxoutput = m_matrix = m_maxmatrix = 0;
	m_matrixdmapos = m_istreampos = m_stateidx = m_statesize = m_pass = 0;
	pVerts = 0;

	for( i = 0; i < BONED_MATRIX_CACHE; i++ )
		m_matrixdma[i][0] = m_matrixdma[i][1] = -1;
	for( i = 0; i < BONED_INSTRUCTION_STREAM_SIZE; i++ )
		m_istream[i][0] = m_istream[i][1] = m_istream[i][2] = m_istream[i][3] = -1;

	if( pBuffers )
	{
		AnmBuffer *pB = pBuffers;
		while( pB->pNext )
			pB = pB->pNext;
		pB->pNext = this;
	}
	else
	{
		// first buffer, init the object
		TotalWordCount = 0;
		for( i = 0; i < BONED_MATRIX_CACHE; i++ )
		{
			Matrices[i] = -1;
			MatrixInUse[i] = 0;
		}
		for( i = 0; i < BONED_MATRIX_REGS; i++ )
			Regs[i] = -1;


		pBuffers = this;
		pNext = NULL;

		if( bLightable)
		{
			MatrixQWCount = NUM_LIGHT_VECTORS;
			MaxCached = (BONED_MATRIX_CACHE * 4) / NUM_LIGHT_VECTORS;
			MinVerts = 2;
			MaxRegs = 2;
			// send light color data
			MatrixInUse[0] = MATRIX_LIFETIME;
		}
		else
		{
			MatrixQWCount = 4;
			MaxCached = BONED_MATRIX_CACHE;
			MinVerts = 3;
			MaxRegs = 4;
		}

		TotalWordCount = 8; //beginning dmatag, base, offset, flush.  only one of these per object
	}

	// we don't know for sure that regs will stay valid between buffers
	for( i = 0; i < BONED_MATRIX_REGS; i++ )
		Regs[i] = -1;
	m_instruction = 1; // because the first qword is always occupied by a giftag
	m_instruction++; // because we always need a kick instruction at the end
	m_maxinstruction = BONED_INSTRUCTION_STREAM_SIZE;
	m_maxinput = VSTtoANMDblBuffer::BufferInputSize;
	m_output = 1; // for a giftag
	m_maxoutput = VSTtoANMDblBuffer::BufferSize - VSTtoANMDblBuffer::BufferGiftagOffset + 1;

	// figure out how many matrices we can use.
	// regardless of what else happens, we need to make sure that the next buffer
	// has some slots to use.
	for( i = 0; i < MaxCached; i++ )
	{
		if( MatrixInUse[i] )
		{
			MatrixInUse[i]--;
			if( MatrixInUse[i] == 0 )
				m_maxmatrix++;
		}
		else
		{
			m_maxmatrix++;
		}
	}

	if( m_maxmatrix > (MaxCached / 2) )
		m_maxmatrix = (MaxCached / 2);

	m_startvert = vertidx;
}

AnmBuffer::~AnmBuffer()
{
	AnmVert *pV = pVerts, *pNV;
	while( pV )
	{
		pNV = pV->pNext;
		delete pV;
		pV = pNV;
	}
	pVerts = NULL;
}

void AnmBuffer::AddState( int _stateidx, VUStateProtocol &state, int pass )
{
	if( m_statesize ) //shouldn't be set twice, or we're screwed
	{
		int *dontdothis = NULL;
		int yousuck;

		yousuck = *dontdothis;
	}

	m_statesize = state.sizeVUQWords();
	m_stateidx = _stateidx;
	m_pass = pass;

	// the state data takes up space both from the instruction stream and the output area.
	m_output += state.sizeVUQWords();
	m_instruction += state.sizeVUQWords();
}

void AnmBuffer::SetState( int _stateidx, int pass )
{
	m_stateidx = _stateidx;
	m_pass = pass;
}

int AnmBuffer::AddVerts( MeshInterface &Protocol, int vtxidx, int nverts, int recursion )
{
	int maxVin, maxVout, maxV, i;
	bool inTransform;
	int vert, mtx, bm, reg;
	bool found;
	int stripend = 0;

	if( pVerts == NULL ) // always begin a buffer with 2 adc bits
		stripend = 2;

	// See if we can fit as many verts as requested into the current buffer.  If
	// we can't, try nverts-1

	//the next three lines are so I have somewhere to set a breakpoint to check
	// the recursion depth and track nasty recursion bugs if any more occur
	found = false;
	if( recursion > 3 )
		found = true;

	if( nverts == 0 )
		return 0;

	if( nverts == -1 )
		nverts = Protocol.numVertices() - vtxidx;

	// first, a simple test.  We know we cannot possibly add more than remaining input
	// qwords / 2, or remaining output qwords / 3
	maxVin = (m_maxinput - m_input) / 2;
	maxVout = (m_maxoutput - m_output) / 3;

	maxV = maxVin < maxVout ? maxVin : maxVout;

	if( nverts > maxV )
		return AddVerts( Protocol, vtxidx, maxV, recursion + 1 );

	// next, an even simpler test.  If we don't have any more instruction space, we can't
	// build any more in.
	if( m_instruction >= m_maxinstruction )
		return 0;


	assert( vtxidx + nverts <= (int)Protocol.numVertices() );

	// NOTE: there are early outs all over the place.  for example, if we're processing
	// a vert and that vert overfills the input buffer, we early out right there and
	// try with as many verts as we already processed minus one.  Be sure to copy back
	// our backup before recursing or returning

	Backup( *this );
	// assuming that we always start out in the instruction stream, not transforming
	inTransform = false;

	// when we start transforming, we have to run through minverts vertices
	// before we can safely pop out and do a register switch, so make sure our first
	// verts have their matrices loaded into regs

	// don't do any of this if we're not in the first minverts verts, though.
	// it causes bad things to happen
	if( m_verts < MinVerts )
	{

		int beginmatrices[3] = {-1,-1,-1};
		int bmcount = 0;
		for( vert = 0; vert < MinVerts; vert++ )
		{
			bool		found;
			
			int		idx = Protocol.idxMatrix( vtxidx + vert);

			found = false;
			for( bm = 0; bm < bmcount; bm++)
			{
				if( beginmatrices[bm] == idx)
					found = true;	
			}

			if( found == false)
				beginmatrices[bmcount++] = idx;
		}

		for( bm = 0; bm < bmcount; bm++ )
		{
			int cachespot = -1, regspot = -1;

			mtx = beginmatrices[bm];
			for( i = 0; i < MaxCached; i++ )
			{
				if( Matrices[i] == mtx )
				{
					cachespot = i;
					if( MatrixInUse[i] != MATRIX_LIFETIME ) // did somebody else put it there originally?
					{
						// we don't need to dma it into the cache, but we do still need to
						// claim it as our own so nobody else fucks with it
						MatrixInUse[i] = MATRIX_LIFETIME;
					}
					m_matrix++;
					break;
				}
			}
			if( cachespot == -1 )
			{
				// find a spot for it
				cachespot = ChooseCacheSpot( Protocol, vtxidx );

				// there is no space left in the cache, so get out of here without adding the vert..
				assert( cachespot != -1);

				// we do need to add it to our dma, so make sure we figure in the space.
				// we also need to claim it so it doesn't get clobbered
				Matrices[cachespot] = mtx;
				MatrixInUse[cachespot] = MATRIX_LIFETIME;
				m_matrix++;
				m_matrixdma[m_matrixdmapos][MTX_IDX] = mtx;
				m_matrixdma[m_matrixdmapos][MTX_CACHESLOT] = cachespot;
				m_matrixdmapos++;
			}
			// we assume that regs are clobbered between buffers, so just fill up the first
			// few reg slots
			Regs[bm] = mtx;
			m_instruction++;
			m_istream[m_istreampos][INSTR_INSTR] = INSTR_CACHETOREG;
			m_istream[m_istreampos][INSTR_DATA1] = cachespot;
			m_istream[m_istreampos][INSTR_DATA2] = bm;
			m_istreampos++;
		}
		// at this point, all we've done is make sure the matrices for the first three verts
		// are in regs.  we haven't figured space for the verts yet, so we still need to loop
		// from the beginning.  It will just happen that the verts find their matrices already
		// in regs, so they won't have to do anything

		// technically, we should check here to see if we've overflowed anything,
		// but if we have, we're pretty much fucked anyway.  These functions are meant to
		// stuff buffers as full as you can from start to finished, not build a buffer
		// up in pieces.
	}

	// keep these separate until we know how many really are going to fit, so we can
	// clean these up if we end up restoring the old AnmBuffer
	AnmVert *addedVerts = NULL;

	for( vert = 0; vert < nverts; vert++ )
	{
		AnmVert *nV = new AnmVert;

		if( AnmVert::Tail( addedVerts ) )
			AnmVert::Tail( addedVerts )->pNext = nV;
		else
			addedVerts = nV;


		if( stripend > 0 )
		{
			nV->m_adc = true;
			stripend--;
		}

		nV->m_idx = vtxidx + vert;
		mtx = Protocol.idxMatrix( nV->m_idx );
		nV->m_mtxtransform = mtx;
		int inregs = -1;

		// is our matrix in registers?
		for( reg = 0; reg < MaxRegs; reg++ )
			if( Regs[reg] == mtx )
				inregs = reg;

		if( inregs != -1 )
		{
			nV->m_regtransform = inregs;

			// if transforming, just continue, nothing to do here since we already checked
			// for input and output space at the beginning of the loop
			if( !inTransform )
			{
				// begin transforming
				m_instruction++;
				m_istream[m_istreampos][INSTR_INSTR] = INSTR_TRANSFORM;
				m_istream[m_istreampos][INSTR_DATA1] = 0;
				m_istream[m_istreampos][INSTR_DATA2] = vert;
				m_istreampos++;
				if( m_instruction > m_maxinstruction )
				{
					*this = Restore();
					AnmVert::CleanChain( addedVerts );
					addedVerts = NULL;
					return AddVerts( Protocol, vtxidx, vert - 1, recursion + 1 );
				}
				inTransform = true;
			}
		}
		else
		{
			// our matrix is not in regs.  Get it there
			inTransform = false;

			int cachespot = -1, regspot = -1;
			for( i = 0; i < MaxCached; i++ )
			{
				if( Matrices[i] == mtx )
				{
					cachespot = i;
					if( MatrixInUse[i] != MATRIX_LIFETIME ) // did somebody else put it there originally?
					{
						// we don't need to dma it into the cache, but we do still need to
						// claim it as our own so nobody else fucks with it
						MatrixInUse[i] = MATRIX_LIFETIME;
						m_matrix++;
						if( m_matrix >= m_maxmatrix )
						{
							*this = Restore();
							AnmVert::CleanChain( addedVerts );
							addedVerts = NULL;
							return AddVerts( Protocol, vtxidx, vert - 1, recursion + 1 );
						}
					}
					break;
				}
			}
			if( cachespot == -1 )
			{
				// find a spot for it
				cachespot = ChooseCacheSpot( Protocol, vtxidx + vert );
				if( cachespot == -1)
				{
					PluginLog::Add( LogFlags::VERBOSE_LEVEL_HIGH, "BAD MODEL!!!");

					AnmBuffer *pW = pBuffers;
					while( pW)
					{
						for( unsigned int m = 0 ; m < (unsigned int)MaxCached ; m++)
							PluginLog::Add( LogFlags::VERBOSE_LEVEL_HIGH, "M[%02d] %02d : %02d", m, pW->Matrices[m], pW->MatrixInUse[m]);

						pW->DebugInfo( );
						pW = pW->pNext;
					}
					PluginLog::Add( LogFlags::VERBOSE_LEVEL_HIGH, "BAD MODEL!!!");
					assert(cachespot != -1);
				}
				// we do need to add it to our dma, so make sure we figure in the space.
				// we also need to claim it so it doesn't get clobbered
				Matrices[cachespot] = mtx;
				MatrixInUse[cachespot] = MATRIX_LIFETIME;
				m_matrix++;
				m_matrixdma[m_matrixdmapos][MTX_IDX] = mtx;
				m_matrixdma[m_matrixdmapos][MTX_CACHESLOT] = cachespot;
				m_matrixdmapos++;
				if( m_matrix >= m_maxmatrix )
				{
					*this = Restore();
					AnmVert::CleanChain( addedVerts );
					addedVerts = NULL;
					return AddVerts( Protocol, vtxidx, vert - 1, recursion + 1 );
				}
			}
			regspot = ChooseRegSpot( Protocol, vtxidx + vert );
			nV->m_regtransform = regspot;
			Regs[regspot] = mtx;
			m_instruction++;
			m_istream[m_istreampos][INSTR_INSTR] = INSTR_CACHETOREG;
			m_istream[m_istreampos][INSTR_DATA1] = cachespot;
			m_istream[m_istreampos][INSTR_DATA2] = regspot;
			m_istreampos++;
			m_instruction++;
			m_istream[m_istreampos][INSTR_INSTR] = INSTR_TRANSFORM;
			m_istream[m_istreampos][INSTR_DATA1] = 0;
			m_istream[m_istreampos][INSTR_DATA2] = vert;
			m_istreampos++;
			if( m_instruction > m_maxinstruction )
			{
				*this = Restore();
				AnmVert::CleanChain( addedVerts );
				addedVerts = NULL;
				return AddVerts( Protocol, vtxidx, vert - 1, recursion + 1 );
			}
			inTransform = true;
		}

		if( !inTransform )
		{
			int *fuckedup = NULL;
			int readingnull;

			//um...we can't add a vertex unless we're transforming.  wtf is going on?
			readingnull = *fuckedup;
		}
		// add one more vert to the transform instruction
		m_istream[m_istreampos-1][INSTR_DATA1]++;
		// nothing else matters if we don't have room for the vert in the input and output
		// buffers
		m_input += 2;
		m_output += 3;
		m_verts++;
		if( m_input > m_maxinput || m_output > m_maxoutput )
		{
			*this = Restore();
			AnmVert::CleanChain( addedVerts );
			addedVerts = NULL;
			return AddVerts( Protocol, vtxidx, vert - 1, recursion + 1 );
		}
		if( Protocol.idx( nV->m_idx ) & MeshInterface::IDX_END_OF_STRIP )
			stripend = 2;
	}
	// don't forget the kick
	m_istream[m_istreampos][INSTR_INSTR] = INSTR_KICK;
	m_istreampos++;
	// we don't need to account for the kick in instruction and maxinstruction, because
	// we already added one for it

	// if taking this many verts is going to leave less than 5 for the next buffer, cut
	// back a few.
	int nextbuffer = Protocol.numVertices() - ( vtxidx + nverts );
	if( nextbuffer > 0 && nextbuffer < 5 )
	{
		*this = Restore();
		AnmVert::CleanChain( addedVerts );
		addedVerts = NULL;
		return AddVerts( Protocol, vtxidx, nverts - ( 5 - nextbuffer ), recursion + 1 );
	}

	if( m_input <= m_maxinput && m_output <= m_maxoutput &&
			m_instruction <= m_maxinstruction && m_matrix <= m_maxmatrix )
	{
		// final dropout point.  if the second to last vert is the end of a strip, break the
		// buffer there.
		// if the last vert is the end of a strip, we don't need any special handling.
		// if neither is the end of a strip, the last 2 verts will need to be duplicated
		// in the next buffer.
		AnmVert *pT = AnmVert::Tail( addedVerts );
		if( !(Protocol.idx( pT->m_idx - 1 ) & MeshInterface::IDX_END_OF_STRIP) )
		{
			// SUCCESS!  We added all the verts we were asked to add
			AnmVert *pV = AnmVert::Tail( pVerts );
			if( pV ) pV->pNext = addedVerts;
			else pVerts = addedVerts;
			return nverts;
		}
	}

	// we overflowed somewhere, see if we can add one less
	*this = Restore();
	AnmVert::CleanChain( addedVerts );
	addedVerts = NULL;
	return AddVerts( Protocol, vtxidx, nverts - 1, recursion + 1 );
}

int AnmBuffer::ChooseSpot( MeshInterface &Protocol, int vtxidx, int *slots, int nslots, int *usage )
{
	int usecount[BONED_MATRIX_CACHE][2]; // won't be bigger than this, even for regs
	int i, j, mtx, maxscan;

	for( i = 0; i < nslots; i++ )
	{
		usecount[i][0] = 0;
		usecount[i][1] = 0;
	}

	// don't ask me why, because I have no clue, but every test I ran indicated that
	// scanning ahead three times your cache size for prediction purposes gave the
	// best results.  But if we have fewer verts than that, of course don't scan past
	// the end.  duh.
	maxscan = Protocol.numVertices( ) - vtxidx;
	if( maxscan > (nslots * 3) )
		maxscan = nslots * 3;
	for( i = 0; i < maxscan; i++ )
	{
		mtx = Protocol.idxMatrix( vtxidx + i );
		for( j = 0; j < nslots; j++ )
			if( slots[j] == mtx )
				usecount[j][0]++;
	}
	// now scan through again, but scan to the end of the state to see how many times
	// the matrix is used.  This will become a tiebreaker, especially in the case of a
	// matrix being used 0 times in the nslots*3 span, but possibly being used later.
	// We don't want to replace one like that if there's one that isn't used at all
	maxscan = Protocol.numVertices( ) - vtxidx;
	for( i = 0; i < maxscan; i++ )
	{
		mtx = Protocol.idxMatrix( vtxidx + i );
		for( j = 0; j < nslots; j++ )
			if( slots[j] == mtx )
				usecount[j][1]++;
	}

	int slot = -1;
	// now pick one
	for( i = nslots - 1; i >= 0; i-- )
	{
		if( usage && usage[i] != 0 ) // we can't clobber this one because it's in use
			continue;

		// if we haven't picked one yet and this slot is eligible, pick it now
		if( slot == -1 )
		{
			slot = i;
			continue;
		}

		// now check usecounts
		if( slots[i] == -1 ) // pick unused slots above all else
			slot = i;
		else if( usecount[i][0] < usecount[slot][0] )
			slot = i;
		else if( usecount[i][0] == usecount[slot][0] )
			if( usecount[i][1] < usecount[slot][1] )
				slot = i;
	}

	return slot;
}

int AnmBuffer::ChooseCacheSpot( MeshInterface &Protocol, int vtxidx )
{
	return ChooseSpot( Protocol, vtxidx, Matrices, MaxCached, MatrixInUse );
}

int AnmBuffer::ChooseRegSpot( MeshInterface &Protocol, int vtxidx )
{
	return ChooseSpot( Protocol, vtxidx, Regs, MaxRegs, NULL );
}
