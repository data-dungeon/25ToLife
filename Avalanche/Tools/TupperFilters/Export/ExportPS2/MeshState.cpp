////////////////////////////////////////////////////////////////////////////
//
// MeshState
//
// State Packet class for communicating with the PS2 Packetizer library
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MeshState.cpp $
 * 
 * *****************  Version 14  *****************
 * User: Joe Barnes   Date: 4/24/03    Time: 1:26p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * JLB: Changes for Dynamic Textures
 * 
 * *****************  Version 13  *****************
 * User: Joe Barnes   Date: 2/10/03    Time: 1:15p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * jlb: changes for new PS2 renderer
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 9/17/02    Time: 10:00a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * Multi-texturing first pass complete
 * 
 * *****************  Version 11  *****************
 * User: Nmartin      Date: 7/05/02    Time: 12:01p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * move som.h to game\database
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 5/06/02    Time: 4:30p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * DMA Som Code put in
 */

#include "ExportPch.h"

#define SCE_GS_ALPHA_1		0x42
#define SCE_GS_TEX0_1		0x06
#define SCE_GS_TEX1_1		0x14
#define SCE_GS_CLAMP_1		0x08
#define SCE_GS_TEST_1		0x47
#define SCE_GS_RGBAQ		0x01
#define SCE_GS_MIPTBP1_1	0x34
#define SCE_GS_MIPTBP2_1	0x36

#include "MeshState.h"
#include "Display/PS2/Renderer/Common/VIFCreator.h"
#include "Game/Database/SOM.h"

//Statics for the MeshState class
streampos	MeshState::spPrevPos;
bool MeshState::bFirstPass=true;
u32 MeshState::prevNRegs=0;
u32	MeshState::prevStateData[MAX_PACKET_REGS][4];

// this is the version # of the fixup info.. it should be incremented
// if the data in the fixup packet gets modified.
// 0 = TEX0, TEX1, CLAMP, [RGBAQ], [ALPHA]
// 1 = TEX0, TEX1, CLAMP, MIPTBP1, MIPTBP2, [RGBAQ], [ALPHA]
#define FIXUP_VERSION 1

MeshState::MeshState() : VUStatePacket(MAX_PACKET_REGS, 12, MODE_SHADE_CONSTANT, 0xff808080, MODE_UNTEXTURED )
{
	m_u32TexIndex = -1;
	m_u32LayerIndex = 0; // default to layer 0
}

MeshState::~MeshState()
{
}

unsigned int MeshState::calcPacketRegs(void) const
{
	u32	regs = 0;

	if(d_StateInfo.texturing != MODE_UNTEXTURED)
		regs += 6;
	
//	if(d_StateInfo.texturing == MODE_DIFFUSE_TEXTURED)
//		regs++;

	if(d_StateInfo.shading == MODE_SHADE_FLAT)
		regs++;

	return(regs);
}

unsigned int MeshState::fillPacketData(void)
{
	u32			nRegs;
	u32			*pStateMem;

	nRegs = 0;
	pStateMem = (u32 *)p_StatePacketMem;

	if (d_StateInfo.texturing != MODE_UNTEXTURED)
	{
#ifdef WIN32
		pStateMem[0] = m_u32TexIndex;	//Add 1 because 0 = white texture
		pStateMem[1] = FIXUP_VERSION;
#else
		pStateMem[0] = stGetTex0Low( m_u32TexIndex);
		pStateMem[1] = stGetTex0High( m_u32TexIndex);
#endif
		pStateMem[2] = 0xdeadbeef;
		//This will need to be changed to the offset of the next stateinfo in memory
		//so we can fix up the textures on the PS2
		pStateMem[3] = 0;

		d_StatePacketTag.setReg( nRegs++, GIFTag::REG_TEX0_1);
		pStateMem += 4;

		//The values for TEX1 and CLAMP are determined on fixup
		pStateMem[0] = m_u32LayerIndex;
		pStateMem[1] = 0;
		pStateMem[2] = SCE_GS_TEX1_1;

		d_StatePacketTag.setReg(nRegs++, GIFTag::REG_A_D);
		pStateMem +=4;

		pStateMem[0] = 0;
		pStateMem[1] = 0;
		pStateMem[2] = SCE_GS_CLAMP_1;

		d_StatePacketTag.setReg(nRegs++, GIFTag::REG_A_D);
		pStateMem += 4;

		pStateMem[0] = 0;
		pStateMem[1] = 0;
		pStateMem[2] = SCE_GS_MIPTBP1_1;

		d_StatePacketTag.setReg(nRegs++, GIFTag::REG_A_D);
		pStateMem += 4;

		pStateMem[0] = 0;
		pStateMem[1] = 0;
		pStateMem[2] = SCE_GS_MIPTBP2_1;

		d_StatePacketTag.setReg(nRegs++, GIFTag::REG_A_D);
		pStateMem += 4;

		if(d_StateInfo.texturing == MODE_DIFFUSE_TEXTURED)
		{
			pStateMem[0] = 0x89; // abcd (Cd - 0) * As + 0  (a=1, b = 2, c = 0, d = 2)
		} else {
			pStateMem[0] = 0x44;
		}
		pStateMem[1] = 0x80; // fix
		pStateMem[2] = SCE_GS_ALPHA_1;

		d_StatePacketTag.setReg(nRegs++, GIFTag::REG_A_D);
		pStateMem += 4;
	}
	if (d_StateInfo.shading == MODE_SHADE_FLAT)
	{
		pStateMem[0] = d_ConstantColor & 0x0ff;
		pStateMem[1] = (d_ConstantColor >> 8) & 0x0ff;
		pStateMem[2] = (d_ConstantColor >> 16) & 0x0ff;
		pStateMem[3] = (d_ConstantColor >> 24) & 0x0ff;

		d_StatePacketTag.setReg(nRegs++, GIFTag::REG_RGBAQ);
		pStateMem += 4;
	}
	d_StatePacketTag.setNREGS(nRegs);
	return(nRegs);
}

void MeshState::setTextureLayer(u32 u32TextureLayer)
{
	m_u32LayerIndex = u32TextureLayer;
}

unsigned int MeshState::getDynamicTextureID( void ) const
{
	assert( m_u32LayerIndex < 256);

	unsigned int texID = 0;

	if( m_u32TexIndex == -1)
		texID = 0x00ffffff;
	else 
	{
		texID = m_u32TexIndex & 0xffff;
		
		if( m_u32TexIndex & 0xff000000)
			texID |= 0x400000;
	}

	assert( (texID & 0xff000000) == 0);

	return texID | (m_u32LayerIndex << 24);
}

void MeshState::setTextureContext(ts_TextureContext* ctxt, u32 u32TexContextIndex)

{
	pContext = ctxt;
	/*m_u32TexIndex = ctxt->TextureLayer[0].s16TextureBind;*/
	//Make sure that the context in question has textures
	if (ctxt->TextureCount>0)
		m_u32TexIndex = u32TexContextIndex;
	else
		m_u32TexIndex = -1;
}

void MeshState::setTextureIndex(u32 texindex)
{
	ts_TextureContext *tc = (ts_TextureContext *)texindex;
	m_u32TexIndex = tc->TextureLayer[0].s16TextureIndex;
}
