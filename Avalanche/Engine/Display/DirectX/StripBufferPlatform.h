#ifndef __STRIP_BUFFER_PLATFORM_H
#define __STRIP_BUFFER_PLATFORM_H

#include "Display/DisplayPCH.h"
#include "Display/DirectX/DirectX.h"

#include "Display/DirectX/DXPublished.h"
#include "Display/DirectX/DynamicIB.h"

class DXStripData
{
public:

	u16 *							p_Indices;
	unsigned int            m_NumIndices;

	unsigned int				m_MinIdx;
	unsigned int				m_MaxIdx;

	DXStripData( 
		const u16 *					_Indices,
		unsigned int				_NumIndices
		);

	~DXStripData( );
	
};

class StripBufferPass
{
public:
	Texture *					m_Texture;
	u8								m_Method;
	u8								m_WrapU;
	u8								m_WrapV;
	u8								m_Filter;
	u8								m_Blend;
};

////////////////////////////////////////////////////////////////////////////

class StripBufferPlatformData
{
	static unsigned int					m_SharedIBIndices;
	static unsigned int					m_StripBufferStart;
	static unsigned int					m_StripBufferExpand;

	static DynamicIB *					p_IB;

	D3DPRIMITIVETYPE						m_PrimitiveType;
	unsigned int							m_RenderTextures;

	void							renderTexturePasses( unsigned int baseIndex, unsigned int numIndices, unsigned int minIndex, unsigned int numVertices, const Matrix4x4 * texMatrices );

public:
	static void								createDeviceObjects( void );
	static void								deleteDeviceObjects( void );

	DynamicArray< DXStripData * >		a_Strips;

	StripBufferPass						m_Passes[8];

	StripBufferPlatformData( D3DPRIMITIVETYPE _Type );
	~StripBufferPlatformData( );

	bool							VerifyTexturePasses( unsigned int numPasses );
	void							Render( const Matrix4x4 & modelMatrix, const Matrix4x4* texMatrices, VtxBuffer & VB );
};

#endif