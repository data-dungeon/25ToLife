#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "direct3d.h"

//=============================================================================
// TEXTUREBIND
//=============================================================================

typedef struct
{
	LPDIRECT3DTEXTURE9 pTexture;
	int Width;
	int Height;
	D3DFORMAT Format;
}
TEXTUREBIND;

#define MAX_TEXTURE_BINDS (1024)

//=============================================================================
// TextureSetRecord
//=============================================================================

struct ts_TextureSet : public TextureDatabase
{
	ts_TextureSet( void ) : TextureDatabase( "Default" ) {}
};

class TextureSetRecord : public CTreeNode
{
	public:
		TextureSetRecord( void );

	public:
		ts_TextureSet m_TextureSet;
		t_Handle m_Handle;

		int m_BindList[16];
};

//=============================================================================
// TextureManager
//=============================================================================

class TextureManager
{
public:
	TextureManager( void );
	~TextureManager( void );

public:

	int RegisterTexture( LPDIRECT3DTEXTURE9 pTexture );
	void DestroyTexture( int Bind );
	void DestroyAllTextures( void );

	//////////////////////////////////////////////////////////////////////////

	static int GetBestTextureSize( int size );

	//////////////////////////////////////////////////////////////////////////

	TextureSetRecord * NewTextureSetRecord( void );
	TextureSetRecord * GetTextureSetRecord( ts_TextureSet *pTextureSet );
	TextureSetRecord * GetTextureSetRecord( t_Handle Handle );

	//////////////////////////////////////////////////////////////////////////

	int TextureBind( LPDIRECT3DTEXTURE9 pTexture );
	int TextureWidth( int Bind );
	int TextureHeight( int Bind );
	D3DFORMAT TextureFormat( int Bind );
	LPDIRECT3DTEXTURE9 Texture( int Bind );

	//////////////////////////////////////////////////////////////////////////

	int CreateTextureFromBitmap( BITMAP2 *pBitmap, int Width, int Height, D3DFORMAT Format, D3DPOOL Pool );

protected:
	TEXTUREBIND m_TextureBinds[MAX_TEXTURE_BINDS];
	TextureSetRecord m_TextureSetRecords;
};

LPDIRECT3DTEXTURE9 TextureMgr__GetTextureDirect3D( int Bind );
LPDIRECT3DTEXTURE9 TextureMgr__GetTextureDirect3D( t_Handle Handle, int Texture );
bool TextureMgr__ProcessTextureSet( t_Handle Handle, void *pChunkDataVoid, ts_DBChunkHdr *pChunkHeader, void **ppRetData, u32 *pu32DataSize);
bool TextureMgr__ProcessBitmap( t_Handle Handle, HBITMAP hbm );
ts_TextureSet * TextureMgr__GetTextureSet( t_Handle Handle );
TextureSetRecord * TextureMgr__GetTextureSetRecord( ts_TextureSet *pTextureSet );
bool TextureMgr__SaveTexture( char *pFileName, t_Handle Handle, int Texture );

#endif // _TEXTURE_H_
