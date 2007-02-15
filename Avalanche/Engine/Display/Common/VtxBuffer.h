#ifndef __VTX_BUFFER_H__
#define __VTX_BUFFER_H__

#include "Display/Common/DisplayState.h"
#include "Layers/Memory.h"
#include "Math/Vector.h"

class VtxBuffer
{
public:
	friend class			StripBuffer;
	friend class			StripBufferPlatformData;

	VtxBuffer( );
	~VtxBuffer( );

	enum {
		// vertex diffuse color
		VERT_HAS_DIFFUSE			= 0x00000001,		
		// vertex normal
		VERT_HAS_NORMAL			= 0x00000002,		
		// vertex specular color
		VERT_HAS_SPECULAR			= 0x00000004,		
		// vertex buffer will never be read from.
		VERT_WRITEONLY				= 0x00000008,		
		// these bits are reserved for hardware optimizations
		HARDWARE_BITS				= 0xFFFF0000,		
		UNUSED_BITS					= 0x0000FFF8,
    };
    
    enum {
      // number of dimensions in the texture coordinate (s,t,...).
		VERT_TEX_DIM1				= 0x00000001,
      VERT_TEX_DIM2				= 0x00000002,
      VERT_TEX_DIM3				= 0x00000004,
      VERT_TEX_DIM4				= 0x00000008,
        
      // use different bits of the vertex as the texture coordinate.
      // all of these imply a dimension of 3.
      VERT_TEX_USE_POSITION	= 0x00000010,
      VERT_TEX_USE_NORMAL		= 0x00000020,
      VERT_TEX_USE_COLOR		= 0x00000040,
    };
    
	// initialize this thing... 
#define VTX_BUFFER_TEXTURE_LAYERS	8
	bool Init( u32 vtxCount, u32 vtxFlags, u32 texFlags[VTX_BUFFER_TEXTURE_LAYERS] );

	u32						GetVtxCount( void );

	void						Lock( void );
	void						UnLock( void );
    
	static u32				cvtRGBA( u32 color );
	static u32				cvtRGBA( const ts_bRGBA &color )
		{ return cvtRGBA(*((u32 *)&color)); }
	u32 &						Diffuse( u32 vtx );
	u32 &						Specular( u32 vtx );

	Vector3Packed &		Normal( u32 vtx );
	Vector3Packed &		Vertex( u32 vtx );

	float &					TexCoordf( u32 layer, u32 vtx );
	Vector2 &				TexCoordv2( u32 layer, u32 vtx );
	Vector3Packed &		TexCoordv3( u32 layer, u32 vtx );
	Vector4 &				TexCoordv4( u32 layer, u32 vtx );
	
protected:
	// internal data.  please dont futz...
	class						VtxBufferPlatformData * m_PlatformData;
};

#endif // __VTX_BUFFER_H__

#ifdef VTXBUFFER_NEED_INLINES

#ifndef VTXBUFFER_HAVE_INLINES
#define VTXBUFFER_HAVE_INLINES

#if !defined(DIRECTX) && !defined(PS2) && !defined(GCN) && !defined(WIN32_TOOLS_DEBUG) && !defined(WIN32_TOOLS_RELEASE) && !defined(_XBOX)
#error You must have DIRECTX, PS2, GCN, WIN32_TOOLS_DEBUG or WIN32_TOOLS_RELEASE defined in your project file
#endif

#ifdef PS2
#include "Display/PS2/VtxBuffer.hpp"
#endif

#if defined (DIRECTX)
#include "Display/DirectX/VtxBuffer.hpp"
#endif

#if defined (_XBOX)
//#include "display/xbox/vtxbuffer.hpp"
#endif

#ifdef GCN
#include "Display/GCN/VtxBuffer.hpp"
#endif

#endif // VTXBUFFER_HAVE_INLINES

#endif //VTXBUFFER_NEED_INLINES

