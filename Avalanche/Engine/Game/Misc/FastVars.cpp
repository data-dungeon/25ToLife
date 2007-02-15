/*************************************/
/*                                   */
/*   FastVars.c                      */
/*   new engine  06/20/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   speed-critical variables        */
/*                                   */
/*************************************/

/* this file is used to declare variables that are speed-critical. if a platform has a concept of "fast" memory
(e.g. scratchpad ram on the ps2), variables that need to be there can be declared here */

/******************* includes ****************************/

#include "Game/GamePCH.h"





/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* externs *****************************/

/******************* global variables ********************/

#if defined (WIN32) || defined (GCN)

SCRATCHPAD(Graphics4x4,			g4Identity);
SCRATCHPAD(Graphics4x4,			g4Temp);
SCRATCHPAD(Graphics4x4,			g4WorldToCamera);
SCRATCHPAD(Graphics4x4,			g4CameraToWorld);
SCRATCHPAD(Graphics4x4,			g4LocalToCamera);
SCRATCHPAD(Graphics4x4,			g4BoneTransform);
DirCos3x3	g3YRotate90;	// HEY! DirCos3x3 is NOT aligned correctly, so it can't go in the scratchpad where alignment is important.
SCRATCHPAD(Vector3,				WorkVector[WORK_VECTOR_COUNT]);

SCRATCHPAD(Vector3,				vZero);
SCRATCHPAD(Vector3,				vUp);
SCRATCHPAD(ts_BoundingBox,		GeneralBoundingBox);
SCRATCHPAD(ts_BoundingSphere,	GeneralBoundingSphere);

#elif defined (PS2)

SCRATCHPAD(Graphics4x4,			g4Identity);
SCRATCHPAD(Graphics4x4,			g4Temp);
SCRATCHPAD(Graphics4x4,			g4WorldToCamera);
SCRATCHPAD(Graphics4x4,			g4CameraToWorld);
SCRATCHPAD(Graphics4x4,			g4LocalToCamera);
SCRATCHPAD(Graphics4x4,			g4BoneTransform);
DirCos3x3	g3YRotate90;	// HEY! DirCos3x3 is NOT aligned correctly, so it can't go in the scratchpad where alignment is important.
SCRATCHPAD(Vector3,				vZero)									__attribute__((aligned (VU_ALIGN)));
SCRATCHPAD(Vector3,				vUp)										__attribute__((aligned (VU_ALIGN)));
SCRATCHPAD(ts_BoundingBox,		GeneralBoundingBox)					__attribute__((aligned (VU_ALIGN)));
SCRATCHPAD(ts_BoundingSphere,	GeneralBoundingSphere)				__attribute__((aligned (VU_ALIGN)));

/* at least one variable on the scratchpad has to be initialized to a value in order for the metrowerks
profiler to work (see note at bottom of LinkSegment_PS2.lcf). this one chosen at random */

SCRATCHPAD(Vector3,				WorkVector[WORK_VECTOR_COUNT]);

/* pre-built DMA packets */

SCRATCHPAD(u32,	u32ViewportPacket[WORDS_PER_QUAD_WORD * VIEWPORT_PACKET_QWC])													__attribute__ ((aligned(DMA_ALIGN)));
SCRATCHPAD(u32,	u32LightPacket[WORDS_PER_QUAD_WORD * LIGHT_PACKET_QWC])															__attribute__ ((aligned(DMA_ALIGN)));
SCRATCHPAD(u32,	u32DirectionalLightMatrixPacket[WORDS_PER_QUAD_WORD * DIRECTIONAL_LIGHT_MATRIX_PACKET_QWC])			__attribute__ ((aligned(DMA_ALIGN)));
SCRATCHPAD(u32,	u32AmbientLightMatrixPacket[WORDS_PER_QUAD_WORD * AMBIENT_LIGHT_MATRIX_PACKET_QWC])						__attribute__ ((aligned(DMA_ALIGN)));
SCRATCHPAD(u32,	u32FullAmbientLightMatrixPacket[WORDS_PER_QUAD_WORD * FULL_AMBIENT_LIGHT_MATRIX_PACKET_QWC])			__attribute__ ((aligned(DMA_ALIGN)));
SCRATCHPAD(u32,	u32NoLightMatrixPacket[WORDS_PER_QUAD_WORD * NO_LIGHT_MATRIX_PACKET_QWC])									__attribute__ ((aligned(DMA_ALIGN)));
SCRATCHPAD(u32,	u32NoLightPlusEnvMapMatrixPacket[WORDS_PER_QUAD_WORD * NO_LIGHT_PLUS_ENV_MAP_MATRIX_PACKET_QWC])	__attribute__ ((aligned(DMA_ALIGN)));
SCRATCHPAD(u32,	u32VertexWeightingPacket[WORDS_PER_QUAD_WORD * VERTEX_WEIGHTING_PACKET_QWC])								__attribute__ ((aligned(DMA_ALIGN)));
SCRATCHPAD(u32,	u32OverrideColorPacket[WORDS_PER_QUAD_WORD * OVERRIDE_COLOR_PACKET_QWC])									__attribute__ ((aligned(DMA_ALIGN)));
SCRATCHPAD(u32,	u32VIFInterruptPacket[WORDS_PER_QUAD_WORD * VIF_INTERRUPT_PACKET_QWC])										__attribute__ ((aligned(DMA_ALIGN)));
SCRATCHPAD(u32,	u32GSState1Packet[WORDS_PER_QUAD_WORD * GS_STATE_PACKET_QWC])													__attribute__ ((aligned(DMA_ALIGN)));
SCRATCHPAD(u32,	u32GSState2Packet[WORDS_PER_QUAD_WORD * GS_STATE_PACKET_QWC])													__attribute__ ((aligned(DMA_ALIGN)));
SCRATCHPAD(u8,		u8TexturePath2Packet[BYTES_PER_QUAD_WORD * TEXTURE_PATH2_PACKET_QWC])										__attribute__ ((aligned(DMA_ALIGN)));
SCRATCHPAD(u8,		u8ParticleInitPacket[BYTES_PER_QUAD_WORD * PARTICLE_INIT_PACKET_QWC])										__attribute__ ((aligned(DMA_ALIGN)));
SCRATCHPAD(u8,		u8ParticleDataPacket[BYTES_PER_QUAD_WORD * PARTICLE_DATA_PACKET_QWC])										__attribute__ ((aligned(DMA_ALIGN)));
SCRATCHPAD(u8,		u8SpriteInitPacket[BYTES_PER_QUAD_WORD * SPRITE_INIT_PACKET_QWC])												__attribute__ ((aligned(DMA_ALIGN)));
SCRATCHPAD(u8,		u8SpriteDataPacket[BYTES_PER_QUAD_WORD * SPRITE_DATA_PACKET_QWC])												__attribute__ ((aligned(DMA_ALIGN)));
SCRATCHPAD(u8,		u8FlushePacket[BYTES_PER_QUAD_WORD * FLUSHE_PACKET_QWC])															__attribute__ ((aligned(DMA_ALIGN)));

#endif //WIN32/PS2

#ifdef GCN
u32 gxLightIDs;
#endif

/******************* local variables *********************/

/******************* local prototypes ********************/

#ifdef PS2
int FastVars__BuildTexturePath2Packet(u_char* addr);
#endif

/******************* functions ***************************/


void FastVars__Init(void)
{
#ifdef PS2
	int nQWC;

	/* for dynamic texture upload */

	nQWC = FastVars__BuildTexturePath2Packet((u_char*) u8TexturePath2Packet);
	ASSERT(nQWC == TEXTURE_PATH2_PACKET_QWC);

	/* simple flushe packet */

	u_char* addr = (u_char*) u8FlushePacket;
	MasterList::Append8(addr, MasterList::createDMATAG());
	MasterList::Append(addr, createFLUSHE());	/* wait for previous microprogram */
	MasterList::Append(addr, createNOP());
#endif
}

#ifdef PS2
int FastVars__BuildTexturePath2Packet(u_char* addr)
{
	int qwc = 0;	// return final qwc

	/* direct-to-gs packet */

	MasterList::Append(addr, MasterList::createDMATAG(1));

	// TJC - flush to wait for drawing to complete before uploading texture
	// once the new renderer correctly uses the path3 uploads, this flush
	// can be replaced with the one that is generated by the TextureMgr
	// in a much better place...
	MasterList::Append(addr, createFLUSH());
	MasterList::Append(addr, createDIRECT(1));
	qwc++;

	/* send GIF tag for texture data transfer: */

	MasterList::Append(
		addr,
		createGIFTAG(
			0,		// nloop
			0,		// nregs
			0,		// regs
			true,	// eop
			GIFTag::FLG_IMAGE)); // image mode

	qwc++;

	/* and the texture data */

	MasterList::Append(addr, MasterList::createDMATAG(0 /*count*/, TAGID_REF, 0 /*source*/));	// count and source get filled in
	MasterList::Append(addr, createNOP());
	MasterList::Append(addr, createDIRECT(0));
	qwc++;

	/* done. return quad word count of packet */

	return(qwc);
}
#endif
