#include <hud\hudlib.h>
#include "direct3d.h"
#include "texture.h"

//=============================================================================
// SpriteHandle
//=============================================================================

class SpriteHandle : public CTreeNode
{
	DECLARE_CTREENODE( SpriteHandle );

	public:
		SpriteHandle( void );

		void SetVertices( Vector3 *pVertices );
		void SetTexcoords( Vector2 *pTexcoords );
		void SetColors( ts_bRGBA *pColors );	

	public:
		ts_TextureSet *m_pTextureSet;
		UINT m_TextureIndex;
		UINT m_SortPriority;
		UINT m_Flags;
		UINT m_BlendMode;
		UINT m_RenderFlags;
		UINT m_Bind;
		HWND m_hWnd;

		//////////////////////////////////////////////////////////////////////////
		
		Vector3 m_Vertices[4];
		Vector2 m_Texcoords[4];
		ts_bRGBA m_Colors[4];		
};

//=============================================================================
// SpriteManager
//=============================================================================

class SpriteManager
{
	public:
		SpriteManager( void );
		~SpriteManager( void );

		bool SetTextureSet( t_Handle Handle );
		bool SetTextureSet( ts_TextureSet *pTextureSet );
		SpriteHandle * CreateSprite( UINT TextureIndex, UINT SortPriority, UINT Flags, Vector3 *pVertices, Vector2 *pTexcoords, ts_bRGBA *pColors );
		SpriteHandle * RegisterSprite( UINT TextureIndex, UINT SortPriority, UINT Flags, Vector3 *pVertices, Vector2 *pTexcoords, ts_bRGBA *pColors );

		void RenderAllSprites( CDirect3D &d3d );

		void InsertSprite( SpriteHandle *pSprite );
		void RemoveSprite( SpriteHandle *pSprite );

	public:
		SpriteHandle m_UnusedSprites;
		SpriteHandle m_ActiveSprites;
		ts_TextureSet *m_pTextureSet;

	protected:
		void DestroySprites( SpriteHandle* pSpriteHandle );
		
	protected:
		LPDIRECT3DTEXTURE9 m_pBaseTexture;
};

SpriteHandle * GetSpriteAsHandle( Sprite *pSprite );

extern SpriteManager sm;