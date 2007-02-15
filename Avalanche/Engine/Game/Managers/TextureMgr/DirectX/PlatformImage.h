#ifndef __PLATFORM_IMAGE_H__
#define __PLATFORM_IMAGE_H__

#include "Display/DirectX/DXPublished.h"

class PlatformImage : public DXDeviceObjectClass
{
protected:

enum {
	TEMPORARY_IMAGE	= 0x0001,
	DUMMY_IMAGE       = 0x0002,
	SURFACE_COPY		= 0x0004,
	RENDER_TARGET		= 0x0008,
	LOCK_OUTSTANDING	= 0x1000,
	BUFFER_FETCHED		= 0x2000,
	PALETTE_FETCHED	= 0x0800,
	CPU_SIDE_INVALID	= 0x4000,
	OWNS_CPU_BUFFER   = 0x8000,
#ifdef DIRECTX_PC
	DEPTH_BUFFER		= 0x0400,
#endif
} ;

typedef struct
{
	void *					p_Image;
	void *					p_Palette;
} MIPLevelData;

typedef void (PlatformImage::*PIImageConvertor)( void *, const void *, const unsigned int, const unsigned int );
typedef void (PlatformImage::*PIPaletteConvertor)( void *, const void *, const unsigned int );

	static bool				m_ForceDynamic;

	unsigned int			m_Flags;

	unsigned int			m_MaxMIPLevels;
	unsigned int			m_MIPLevels;

	float						m_UScale;
	float						m_VScale;

	bool						b_Dynamic;

	bool						b_OwnPalettes;
	bool						b_OwnImages;

	Image *					m_Owner;

	MIPLevelData *			m_MIPLevelBuffers;

	unsigned int			m_PaletteEntries;
	PALETTEENTRY *			m_MasterPalette;

	D3DFORMAT				m_InternalFormat;
	unsigned int			m_InternalBytesPP;

	D3DFORMAT				m_D3DFormat;

	PIImageConvertor		p_ImageConvertor;
	PIPaletteConvertor	p_PaletteConvertor;

	IDirect3DTexture8 *	p_D3DTexture;

#ifdef DIRECTX_PC
	IDirect3DSurface8 *	p_D3DSurface;
#endif

	static D3DVIEWPORT8			RTTBackupViewport;
	static IDirect3DSurface8 * pRTTBackupRTSurface;
	static IDirect3DSurface8 * pRTTBackupDSSurface;
	static IDirect3DSurface8 * pRTTDstSurface;
	static u32		GetBPP( u16 format );

	unsigned int	maxMIPLevels( unsigned int width, unsigned int height );

	void				checkOwnedImages( void );

	void				internalizeImage( unsigned int ownerFormat, const void * pImage, const void * pPalette, void * & pNewImage, void * & pNewPalette );
#ifdef MASSIVE_AD
	void				internalizeFormat( unsigned int ownerFormat, D3DFORMAT & newFormat, unsigned int & newBytesPP, unsigned int & newPaletteEntries, PIImageConvertor &imageConvertor, PIPaletteConvertor &paletteConvertor, bool isMassive = false );
#else
	void				internalizeFormat( unsigned int ownerFormat, D3DFORMAT & newFormat, unsigned int & newBytesPP, unsigned int & newPaletteEntries, PIImageConvertor &imageConvertor, PIPaletteConvertor &paletteConvertor);
#endif
	void				internalizeFormat( unsigned int ownerFormat, D3DFORMAT & newFormat, unsigned int & newBytesPP, unsigned int & newPaletteEntries );

	void				createD3DTexture( unsigned int & m_D3DMIPLevels );
	void				loadD3DTexture( unsigned int m_D3DMIPLevels );
	void				reloadD3DTexture( void * pImage, void *pPalette );

	void				copyMIPLevelToRect( unsigned int level, D3DLOCKED_RECT & Rect );
	void				copyRectToCPUBuffer( D3DLOCKED_RECT & Rect, void * pBuffer, unsigned int bytesPP );

	void				correctPaletteAlpha( void * pPalette, unsigned int entries );
	void				convertPalettedImage4to8( void * pNewImage, const void * pImage, const unsigned int width, const unsigned int height );
	void				convertPalettedImage8to8( void * pNewImage, const void * pImage, const unsigned int width, const unsigned int height );

	void				convertImageRGBAtoARGB( void * pNewImage, const void * pImage, const unsigned int width, const unsigned int height );
	void				convertImagex555to565( void * pNewImage, const void * pOldImage, const unsigned int width, const unsigned int height );
	
#ifdef MASSIVE_AD
	void				convertMassiveImageRGBtoARGB( void * pNewImage, const void * pOldImage, const unsigned int width, const unsigned int height );
#endif
	void				convertImageRGBtoARGB( void * pNewImage, const void * pOldImage, const unsigned int width, const unsigned int height );
	void				convertImage565toARGB( void * pNewImage, const void * pOldImage, const unsigned int width, const unsigned int height );
	void				convertImage1555to8888( void * pNewImage, const void * pOldImage, const unsigned int width, const unsigned int height );

	void				convertPalette( void *pNewPalette, const void * pOldPalette, const unsigned int entries );

public:
enum ImageUse
{
	IMAGE_USE_STANDARD = 0x0000,
	IMAGE_USE_ANIMATED = D3DUSAGE_DYNAMIC,
	IMAGE_USE_RENDERED = D3DUSAGE_RENDERTARGET
} ;

	static void		Init( void );
	static void		Cleanup( void );

	void				releaseDeviceObjects( void );
	void				restoreDeviceObjects( void );

	static void		Test( void ) { }

	static void		PausingTextureManager( void );
	static void		ResumingTextureManager( void );

	/// create the platform specific info given the inputs to the contructor
#ifdef MASSIVE_AD
	PlatformImage( Image * _Owner, void * _Data, void * _Palette = NULL, bool FromXPR = false, bool isMassive = false);
#else
	PlatformImage( Image * _Owner, void * _Data, void * _Palette = NULL, bool FromXPR = false);
#endif
	~PlatformImage( );

	void				EndMIPLevels( bool _Dynamic );
	void				SetCPUSideImageFormat( unsigned int format );

	/// record when the user wants the first mipmap change to occur.
	void				SetMipmapDistance( float fDistance );

	static void		SetDynamicTextureFlag( bool Flag )		{ m_ForceDynamic = Flag; }
	static bool		GetDynamicTextureFlag( void )				{ return m_ForceDynamic; }


	/// append a new mip level to this image
	void				AppendMipLevel( u16 format, u16 width, u16 height, void * data, u16 palFmt, u16 palWidth, u16 palHeight, void * palette );
	u8					GetNumMipLevels( void )	const							{ return m_MIPLevels; }

	u32				GetSize( void ) const									{ return m_Owner->GetWidth( ) * m_Owner->GetHeight( ) * m_InternalBytesPP; }
	void				GetUVScaling( float & fU, float & fV ) const		{ fU = m_UScale; fV = m_VScale; }

	bool				IsLocked( void ) const									{ return (m_Flags & LOCK_OUTSTANDING) != 0; }

	/// TJC - profiling
	u32 GetContextChangeCount(void);
	void ClearContextChangeCount(void);

	/// platform specific Lock/Unlock routine.
	void				Lock( void );
	void				Unlock( void );

	void				LockForRender( void );
	void				UnlockForRender( void );

	/// prep this image for drawing operations
	void				Select( void );
	void				Unselect( void );

	/// render this image into the xywh rectangle.
	/// this image is already selected.  Might be 
	/// writing to frame buffer or to image lockedforrender.
	void				BitBlt(int x, int y, int w, int h);

	/// render this images G channel into A channel of
	/// the xywh rectangle.
	/// this image is already selected. Might be
	/// writing to the frame buffer or to image lockedforrender.
	void				BltGtoA(int x, int y, int w, int h);

	/// release..  release
	/// the texture memory allocated by this image.
	void				Release( void );

	/// release the memory used in main memory.  If this image is not
	/// in texture memory, its data is lost to the ether.
	void				FreeMainRam( void );

	/// platform specific function to get a pointer to the image buffer
	void *			GetImageBuffer( void );

	/// platform specific function to release the image buffer
	void				ReleaseImageBuffer( bool modified = false );

	/// platform specific function to get a pointer to the palette buffer
	void *			GetPaletteBuffer( void);

	/// platform specific function to release the palette buffer
	void				ReleasePaletteBuffer( bool modified = false );

	#ifdef DIRECTX_PC
	IDirect3DTexture8 * getD3DTexture() {return p_D3DTexture;}
	#endif

};

#endif // __PLATFORM_IMAGE_H__
