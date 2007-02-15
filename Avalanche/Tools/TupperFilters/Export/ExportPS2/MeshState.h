////////////////////////////////////////////////////////////////////////////
//
// MeshState
//
// State Packet class for communicating with the PS2 Packetizer library
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MeshState.h $
 * 
 * *****************  Version 11  *****************
 * User: Joe Barnes   Date: 4/24/03    Time: 1:26p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * JLB: Changes for Dynamic Textures
 * 
 * *****************  Version 10  *****************
 * User: Joe Barnes   Date: 2/10/03    Time: 1:15p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * jlb: changes for new PS2 renderer
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 9/17/02    Time: 10:00a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * Multi-texturing first pass complete
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 5/07/02    Time: 11:40a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * Working (have yet to test the data outputted)
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 5/06/02    Time: 4:30p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * DMA Som Code put in
 */

#ifndef MESH_STATE_H
#define MESH_STATE_H

#include "Platform\BaseType.h"
#include "Packetizer/VUStatePacket.h"

// Changed this so that the assert won't happen on flat shaded textured 
#define MAX_PACKET_REGS 7

struct ts_TextureContext;

class MeshState : public VUStatePacket
{
protected:
	unsigned int		calcPacketRegs( void ) const;
	unsigned int		fillPacketData( void );
	u32					m_u32TexIndex;
	u32					m_u32LayerIndex;
	
	static u32			prevStateData[MAX_PACKET_REGS][4];
	static u32			prevNRegs;
	static streampos	spPrevPos;
	static bool			bFirstPass;

	ts_TextureContext* pContext;
public:
	MeshState();
	~MeshState();
	void				setTextureContext(ts_TextureContext* ctxt, u32 u32TexContextIndex);
	ts_TextureContext* getTextureContext(void) { return pContext; }
	u32				getTextureLayer(void) { return m_u32LayerIndex; }
	void				setTextureIndex(u32 texindex);
	void				setTextureLayer(u32 m_u32TextureLayer);
	static void		clearFirstPass(void) { bFirstPass = true; };

	unsigned int	getDynamicTextureID( void ) const;
//	void				writeVIF( ostream & str, unsigned int &baseOffset);

};


#endif
