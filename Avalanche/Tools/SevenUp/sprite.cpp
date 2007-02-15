#include "pch.h"
#include "sprite.h"

#include <Layers/Assert/Assert.h>
#include "texture.h"

class SpriteManager sm;

//=============================================================================
// SpriteHandle::SpriteHandle
//=============================================================================

SpriteHandle::SpriteHandle( void )
{
	m_pTextureSet = 0;
	m_TextureIndex = 0;
	m_SortPriority = 0;
	m_RenderFlags = 0;
	m_Flags = 0;
	m_Bind = 0;
}

//=============================================================================
// SpriteHandle::SetVertices
//=============================================================================

void SpriteHandle::SetVertices( Vector3 *pVertices )
{
	if( pVertices )
		memcpy( m_Vertices, pVertices, sizeof(m_Vertices) );

	return;
}

//=============================================================================
// SpriteHandle::SetTexcoords
//=============================================================================

void SpriteHandle::SetTexcoords( Vector2 *pTexcoords )
{
	if( pTexcoords )
		memcpy( m_Texcoords, pTexcoords, sizeof(m_Texcoords) );

	return;
}

//=============================================================================
// SpriteHandle::SetColors
//=============================================================================


void SpriteHandle::SetColors( ts_bRGBA *pColors )
{
	if( pColors )
		memcpy( m_Colors, pColors, sizeof(m_Colors) );

	return;
}

//=============================================================================
// SpriteManager::SpriteManager
//=============================================================================

SpriteManager::SpriteManager( void )
{
	m_pTextureSet = 0;
}

//=============================================================================
// SpriteManager::~SpriteManager
//=============================================================================

SpriteManager::~SpriteManager( void )
{
	while( m_ActiveSprites.Child() )
		DestroySprites( (SpriteHandle*) m_ActiveSprites.Child() );

	while( m_UnusedSprites.Child() )
		DestroySprites( (SpriteHandle*) m_UnusedSprites.Child() );

	return;
}

//=============================================================================
// SpriteManager::SetTextureSet
//=============================================================================

bool SpriteManager::SetTextureSet( t_Handle Handle )
{
	return SetTextureSet( (ts_TextureSet *) dblGetChunkData( DB_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, Handle ) );
}

//=============================================================================
// SpriteManager::SetTextureSet
//=============================================================================

bool SpriteManager::SetTextureSet( ts_TextureSet *pTextureSet )
{
	if( !pTextureSet )
		return false;

	m_pTextureSet = pTextureSet;

	return true;
}

//=============================================================================
// SpriteManager::CreateSprite
//=============================================================================

SpriteHandle * SpriteManager::CreateSprite( UINT TextureIndex, UINT SortPriority, UINT Flags, Vector3 *pVertices, Vector2 *pTexcoords, ts_bRGBA *pColors )
{
	SpriteHandle *pSprite = 0;

	if( m_pTextureSet )
	{
		if( !m_UnusedSprites.Child() )
			m_UnusedSprites.AddChild( new SpriteHandle );

		pSprite = (SpriteHandle *) m_UnusedSprites.Child();

		if( pSprite )
		{
			pSprite->m_pTextureSet = m_pTextureSet;
			pSprite->m_TextureIndex = TextureIndex;
			pSprite->m_SortPriority = SortPriority;
			pSprite->m_Flags = Flags;

			pSprite->m_RenderFlags = 0;

			if( TextureIndex == TS_NO_TEXTURE )
				pSprite->m_Bind = TS_NO_TEXTURE;
			else
			{
				UINT Bind = 0;
				TextureSetRecord *pTextureSetRecord = TextureMgr__GetTextureSetRecord( m_pTextureSet );

				if( pTextureSetRecord )
					Bind = (UINT) pTextureSetRecord->m_BindList[TextureIndex];

				pSprite->m_Bind = Bind;
			}

			pSprite->SetVertices( pVertices );
			pSprite->SetTexcoords( pTexcoords );
			pSprite->SetColors( pColors );
		}
	}

	return pSprite;
}

//=============================================================================
// SpriteManager::RegisterSprite
//=============================================================================

SpriteHandle * SpriteManager::RegisterSprite( UINT TextureIndex, UINT SortPriority, UINT Flags, Vector3 *pVertices, Vector2 *pTexcoords, ts_bRGBA *pColors )
{
	SpriteHandle *pSprite = CreateSprite( TextureIndex, SortPriority, Flags, pVertices, pTexcoords, pColors );

	if( pSprite )
		InsertSprite( pSprite );

	return pSprite;
}

//=============================================================================
// SpriteManager::InsertSprite
//=============================================================================

void SpriteManager::InsertSprite( SpriteHandle *pSprite )
{
	SpriteHandle *pParent = &m_ActiveSprites;
	SpriteHandle *pChild;

	RemoveSprite( pSprite );

	while( pParent->Child() )
	{
		pChild = (SpriteHandle *) pParent->Child();

		if( pChild->m_SortPriority == pSprite->m_SortPriority )
		{
			while( pChild->Next() )
			{
				if( pChild->m_Bind == pSprite->m_Bind )
					break;

				pChild = pChild->Next();
			}

			pChild->InsertAfter( pSprite );
			return;
		}

		if( pChild->m_SortPriority > pSprite->m_SortPriority )
		{
			pSprite->AdoptChildren( pParent );
			pParent->AddChild( pSprite );
			return;
		}

		pParent = pChild;
	}

	pParent->SetChild( pSprite );
}

//=============================================================================
// SpriteManager::RemoveSprite
//=============================================================================

void SpriteManager::RemoveSprite( SpriteHandle *pSprite )
{
	SpriteHandle* pParent;
	SpriteHandle* pChild;

	if( pSprite )
	{
		pParent = pSprite->Parent();

		if( pParent && pParent != &m_UnusedSprites )
		{
			pSprite->Remove();
			pChild = pParent->Child();

			if( pChild )
				pChild->AdoptChildren( pSprite );
			else
				pParent->AdoptChildren( pSprite );

			m_UnusedSprites.SetChild( pSprite );
		}
	}

	return;
}

//=============================================================================
// SpriteManager::DestroySprites
//=============================================================================

void SpriteManager::DestroySprites( SpriteHandle* pSpriteHandle )
{
	while( pSpriteHandle->Child() )
		DestroySprites( (SpriteHandle*) pSpriteHandle->Child() );

	delete pSpriteHandle;
}

//=============================================================================
// SpriteMgr__ChangeFlag
//=============================================================================

bool SpriteMgr__ChangeFlag( t_Handle Handle, u32 u32Flag )
{
	SpriteHandle *pSprite = (SpriteHandle *) Handle;

	if( !pSprite )
		return false;

	pSprite->m_Flags = (UINT) u32Flag;

	return true;
}

//=============================================================================
// SpriteMgr__SetBlendMode
//=============================================================================

void SpriteMgr__SetBlendMode( t_Handle Handle, u8 u8BlendMode )
{
	SpriteHandle *pSprite = (SpriteHandle *) Handle;

	if( pSprite )
		pSprite->m_BlendMode = (UINT) u8BlendMode;

	return;
}

//=============================================================================
// SpriteMgr__RegisterSprite - Allocates a new sprite and returns the handle.
//=============================================================================

t_Handle SpriteMgr__RegisterSprite( u32 u32TextureIndex, u16 u16Group, u16 u16SortPriority, u32 u32Flags, Vector3 *pVertices, Vector2 *pUVs, ts_bRGBA *pColors, u32 piece )
{
	return (t_Handle) sm.RegisterSprite( (UINT) u32TextureIndex, (UINT) u16SortPriority, (UINT) u32Flags, pVertices, pUVs, pColors );
}

//=============================================================================
// SpriteMgr__KillSprite - Removes the sprite from the system but does not
//									deallocate it.  The sprite is added back to the free
//									list.
//=============================================================================

bool SpriteMgr__KillSprite( t_Handle Handle )
{
	SpriteHandle *pSprite = (SpriteHandle *) Handle;

	if( !pSprite )
		return false;

	sm.RemoveSprite( pSprite );

	return true;
}

//=============================================================================
// SpriteMgr__DisableSprite - Prevents the sprite from being drawn.
//=============================================================================

bool SpriteMgr__DisableSprite( t_Handle Handle )
{
	SpriteHandle *pSprite = (SpriteHandle *) Handle;

	if( !pSprite )
		return false;

	pSprite->m_Flags |= SPRITE_DISABLED;

	return true;
}

//=============================================================================
// SpriteMgr__EnableSprite - Enables the sprite and allows it be drawn.
//=============================================================================

bool SpriteMgr__EnableSprite( t_Handle Handle )
{
	SpriteHandle *pSprite = (SpriteHandle *) Handle;

	if( !pSprite )
		return false;

	pSprite->m_Flags &= ~SPRITE_DISABLED;

	return true;

}

//=============================================================================
// SpriteMgr__SetTextureSet - Specifies the texture set to use when registering
//										new sprites.
//=============================================================================

bool SpriteMgr__SetTextureSet( t_Handle Handle )
{
	return sm.SetTextureSet( Handle );
}

//=============================================================================
// SpriteMgr__SetTextureDbl - Specifies the texture set to use when registering
//										new sprites.
//=============================================================================

bool SpriteMgr__SetTextureDbl( t_Handle Handle )
{
	return SpriteMgr__SetTextureSet( Handle );
}

//=============================================================================
// SpriteMgr__UpdateSpriteEx
//=============================================================================

bool SpriteMgr__UpdateSpriteEx( t_Handle Handle, u16 u16Group, u16 u16SortPriority, u32 u32Flags, Vector3 *pVertices, Vector2 *pUVs, ts_bRGBA *pColors, u8 u8BlendMode )
{
	SpriteHandle *pSprite = (SpriteHandle *) Handle;

	if( !pSprite )
		return false;

	SpriteMgr__ChangeSortPriority( Handle, u16SortPriority );
	SpriteMgr__ChangeFlag( Handle, u32Flags );
	SpriteMgr__SetBlendMode( Handle, u8BlendMode );

	pSprite->SetVertices( pVertices );
	pSprite->SetTexcoords( pUVs );
	pSprite->SetColors( pColors );

	return false;
}

//=============================================================================
// SpriteMgr__UpdateSprite
//=============================================================================

bool SpriteMgr__UpdateSprite( t_Handle Handle, u16 u16Group, u16 u16SortPriority, u32 u32Flags, Vector3 *pVertices, Vector2 *pUVs, ts_bRGBA *pColors )
{
	return SpriteMgr__UpdateSpriteEx( Handle, u16Group, u16SortPriority, u32Flags, pVertices, pUVs, pColors, SPRITEBLEND_NORMAL );
}

//=============================================================================
// SpriteMgr__SetTextureByIndex
//=============================================================================

void SpriteMgr__SetTextureByIndex( t_Handle Handle, u32 u32TextureIndex )
{
	SpriteHandle* pSprite = (SpriteHandle*) Handle;

	if( !pSprite )
		return;

	pSprite->m_TextureIndex = u32TextureIndex;
}

//=============================================================================
// SpriteMgr__ChangeSortPriority - Changes the sprite's sort priority.
//=============================================================================

bool SpriteMgr__ChangeSortPriority( t_Handle Handle, u16 SortValue )
{
	SpriteHandle *pSprite = (SpriteHandle *) Handle;

	if( !pSprite )
		return false;

	if( pSprite->m_SortPriority != SortValue )
	{
		pSprite->m_SortPriority = (UINT) SortValue;

		sm.RemoveSprite( pSprite );
		sm.InsertSprite( pSprite );
	}
	
	return true;
}

//=============================================================================
// SpriteMgr__GetSortPriority - Returns the sprite's sort priority.
//=============================================================================

u16 SpriteMgr__GetSortPriority( t_Handle Handle )
{
	SpriteHandle *pSprite = (SpriteHandle *) Handle;

	if( !pSprite )
		return 0;

	return (u16) pSprite->m_SortPriority;
}

//=============================================================================
// SpriteMgr__GetDataPointers - Get you some pointers to the sprites internal
//										  data.
//=============================================================================

bool SpriteMgr__GetDataPointers( t_Handle Handle, u32 *pu32Flags, Vector3 **ppVertices, Vector2 **ppUVs, ts_bRGBA **ppColors )
{
	SpriteHandle *pSprite = (SpriteHandle *) Handle;

	if( !pSprite )
		return false;

	if( pu32Flags )
		*pu32Flags = pSprite->m_Flags;

	if( ppVertices )
		ppVertices[0] = pSprite->m_Vertices;

	if( ppUVs )
		ppUVs[0] = pSprite->m_Texcoords;

	if( ppColors )
		ppColors[0] = pSprite->m_Colors;

	return true;
}

//=============================================================================
// SpriteMgr__GetTextureBind
//=============================================================================

u16 SpriteMgr__GetTextureBind( t_Handle Handle )
{
	SpriteHandle *pSprite = (SpriteHandle *) Handle;

	if( !pSprite )
		return 0;

	return (u16) pSprite->m_Bind;
}


//=============================================================================
// SpriteMgr__GetTexture
//=============================================================================

Texture* SpriteMgr__GetTexture( t_Handle Handle )
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////

typedef struct
{
	float x;
	float y;
	float z;
	D3DCOLOR color;
	float u;
	float v;
}
sprite_vertex;

#define FVF_SPRITE_VERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

void SpriteManager::RenderAllSprites( CDirect3D &d3d )
{
	static const int VertexOrder[4] = {0, 1, 3, 2};
	LPDIRECT3DTEXTURE9 TextureInUse = 0;
	LPDIRECT3DTEXTURE9 TextureToUse = 0;
	sprite_vertex *pPrimaryVertices = 0;

	LPDIRECT3DVERTEXBUFFER9 PrimaryVertices;
	int VertexBufferLength = sizeof(sprite_vertex) * 4;

	HRESULT hresult;

	int R, G, B, A;
	int i, n;

	if( !m_pBaseTexture )
	{
		int Width = 4;
		int Height = 4;
		int x, y;

		BITMAP2 *bm2 = CreateBitmap2( Width, Height, PIXELFMT_A8R8G8B8, 0, 0 );

		if( bm2 )
		{
			BeginBitmap( 0, bm2 );

			for( y = 0; y < Height; y++ )
				for( x = 0; x < Width; x++ )
					SetPixelU( 0, x, y, 0xffffffff );

			FinishBitmap(0);

			m_pBaseTexture = d3d.CreateTextureFromBitmap( bm2, Width, Height, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED );

			DeleteBitmap2( bm2 );
		}
	}

	D3DRESULT( d3d.CreateVertexBuffer( VertexBufferLength, 0, FVF_SPRITE_VERTEX, D3DPOOL_MANAGED, &PrimaryVertices, 0 ) );

	D3DRESULT( d3d.BeginScene() );
	D3DRESULT( d3d.SetRenderState( D3DRS_ZENABLE, FALSE ) );
	D3DRESULT( d3d.SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID ) );
	D3DRESULT( d3d.SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD ) );
	D3DRESULT( d3d.SetRenderState( D3DRS_ZWRITEENABLE, FALSE ) );
	D3DRESULT( d3d.SetRenderState( D3DRS_ALPHATESTENABLE, FALSE ) );
	D3DRESULT( d3d.SetRenderState( D3DRS_LASTPIXEL, TRUE ) );
	D3DRESULT( d3d.SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE ) );
	D3DRESULT( d3d.SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO ) );
	D3DRESULT( d3d.SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ) );
	D3DRESULT( d3d.SetRenderState( D3DRS_LIGHTING, FALSE ) );
	D3DRESULT( d3d.SetRenderState( D3DRS_AMBIENT, 0xffffffff ) );
	D3DRESULT( d3d.SetRenderState( D3DRS_COLORVERTEX, TRUE ) );

	//////////////////////////////////////////////////////////////////////////
		
	D3DRESULT( d3d.SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR ) );
	D3DRESULT( d3d.SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR ) );
	D3DRESULT( d3d.SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP ) );
	D3DRESULT( d3d.SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP ) );

	D3DRESULT( d3d.SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE ) );
	D3DRESULT( d3d.SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE ) );
	D3DRESULT( d3d.SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE ) );

	D3DRESULT( d3d.SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE ) );
	D3DRESULT( d3d.SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE ) );
	D3DRESULT( d3d.SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE ) );

	//////////////////////////////////////////////////////////////////////////
	
	D3DXMATRIX WorldMatrix;
	D3DXMATRIX ViewMatrix;
	D3DXMATRIX ViewMatrix2;
	D3DXMATRIX ViewMatrix3;
	D3DXMATRIX ProjMatrix;

	D3DXMatrixIdentity( &WorldMatrix );
	D3DXMatrixIdentity( &ViewMatrix );
	D3DXMatrixScaling( &ViewMatrix2, 2.0f, -2.0f, 1.0f );
	D3DXMatrixTranslation( &ViewMatrix3, -0.5f, -0.5f, 0.0f );
	D3DXMatrixOrthoLH( &ProjMatrix, 2.0f, 2.0f, 0.0f, 1.0f );

	D3DRESULT( d3d.SetTransform( D3DTS_WORLD, &WorldMatrix ) );
	D3DRESULT( d3d.SetTransform( D3DTS_PROJECTION, &ProjMatrix ) );
	D3DRESULT( d3d.SetTransform( D3DTS_VIEW, &ViewMatrix ) );
	D3DRESULT( d3d.MultiplyTransform( D3DTS_VIEW, &ViewMatrix2 ) );
	D3DRESULT( d3d.MultiplyTransform( D3DTS_VIEW, &ViewMatrix3 ) );

	SpriteHandle *pSprite = (SpriteHandle *) sm.m_ActiveSprites.Child();
	SpriteHandle *pParent = pSprite;

	int s1 = sizeof(Sprite);
	int s2 = sizeof(SPRITESTATESTRUCT);

	Sprite TheSprite;

	int s3 = sizeof(TheSprite);
	int s4 = sizeof(TheSprite.m_state);
	UINT Flags;
	UINT BlendMode;
	DWORD dwSrcBlend;
	DWORD dwDstBlend;
	DWORD dwBlendOp;
	BOOL bAlphaBlend;

	while( pSprite )
	{
		Flags = pSprite->m_Flags;
		BlendMode = pSprite->m_BlendMode;

		if( !(Flags & SPRITE_DISABLED) )
		{
			hresult = D3DRESULT( PrimaryVertices->Lock( 0, VertexBufferLength, (VOID **) &pPrimaryVertices, 0 ) );

			if( !FAILED(hresult) )
			{
				for( i = 0; i < 4; i++ )
				{
					n = VertexOrder[i];

					R = pSprite->m_Colors[n].R << 16;
					G = pSprite->m_Colors[n].G << 8;
					B = pSprite->m_Colors[n].B << 0;
					A = pSprite->m_Colors[n].A << 24;

					pPrimaryVertices[i].x = pSprite->m_Vertices[n].x();
					pPrimaryVertices[i].y = pSprite->m_Vertices[n].y();
					pPrimaryVertices[i].z = pSprite->m_Vertices[n].z();

					pPrimaryVertices[i].color = R|G|B|A;

					pPrimaryVertices[i].u = pSprite->m_Texcoords[n].x();
					pPrimaryVertices[i].v = pSprite->m_Texcoords[n].y();
				}
				
				D3DRESULT( PrimaryVertices->Unlock() );

				TextureToUse = m_pBaseTexture;

				if( pSprite->m_Bind != TS_NO_TEXTURE )
					TextureToUse = TextureMgr__GetTextureDirect3D( pSprite->m_Bind );

				if( TextureInUse != TextureToUse )
				{
					D3DRESULT( d3d.SetTexture( 0, TextureToUse ) );
					TextureInUse = TextureToUse;
				}

				if( (Flags & SPRITE_BLENDMODE_ENABLED) == SPRITE_BLENDMODE_ENABLED )
				{
					switch( BlendMode )
					{
					case SPRITEBLEND_SOLID:
						bAlphaBlend = FALSE;
						dwSrcBlend = D3DBLEND_SRCCOLOR;
						dwDstBlend = D3DBLEND_DESTCOLOR;
						dwBlendOp = D3DBLENDOP_ADD;
						break;

					case SPRITEBLEND_NORMAL:
						bAlphaBlend = TRUE;
						dwSrcBlend = D3DBLEND_SRCALPHA;
						dwDstBlend = D3DBLEND_INVSRCALPHA;
						dwBlendOp = D3DBLENDOP_ADD;
						break;

					case SPRITEBLEND_ADD:
						bAlphaBlend = TRUE;
						dwSrcBlend = D3DBLEND_SRCALPHA;
						dwDstBlend = D3DBLEND_ONE;
						dwBlendOp = D3DBLENDOP_ADD;
						break;

					case SPRITEBLEND_SUBTRACT:
					//	bAlphaBlend = TRUE;
					//	dwSrcBlend = D3DBLEND_DESTCOLOR;
					//	dwDstBlend = D3DBLEND_SRCALPHA;
					//	dwBlendOp = D3DBLENDOP_SUBTRACT;
						bAlphaBlend = FALSE; // NOT SUPPORTED
						dwSrcBlend = D3DBLEND_SRCCOLOR;
						dwDstBlend = D3DBLEND_DESTCOLOR;
						dwBlendOp = D3DBLENDOP_ADD;
						break;

					case SPRITEBLEND_MODULATE:
						bAlphaBlend = FALSE; // NOT SUPPORTED
						dwSrcBlend = D3DBLEND_SRCCOLOR;
						dwDstBlend = D3DBLEND_DESTCOLOR;
						dwBlendOp = D3DBLENDOP_ADD;
						break;

					case SPRITEBLEND_GLOSS:
						bAlphaBlend = TRUE;
						dwSrcBlend = D3DBLEND_ZERO;
						dwDstBlend = D3DBLEND_SRCCOLOR;
						dwBlendOp = D3DBLENDOP_ADD;
						break;

					case SPRITEBLEND_DIFFUSE:
						bAlphaBlend = TRUE;
						dwSrcBlend = D3DBLEND_DESTCOLOR;
						dwDstBlend = D3DBLEND_ZERO;
						dwBlendOp = D3DBLENDOP_ADD;
						break;

					case SPRITEBLEND_SATURATE:
						bAlphaBlend = TRUE;
						dwSrcBlend = D3DBLEND_ONE;
						dwDstBlend = D3DBLEND_SRCCOLOR;
						dwBlendOp = D3DBLENDOP_ADD;
						break;

					case SPRITEBLEND_DOUBLE:
						bAlphaBlend = TRUE;
						dwSrcBlend = D3DBLEND_DESTCOLOR;
						dwDstBlend = D3DBLEND_SRCCOLOR;
						dwBlendOp = D3DBLENDOP_ADD;
						break;

					case SPRITEBLEND_NEGATIVE:
						bAlphaBlend = TRUE;
						dwSrcBlend = D3DBLEND_INVDESTCOLOR;
						dwDstBlend = D3DBLEND_ZERO;
						dwBlendOp = D3DBLENDOP_ADD;
						break;
					}
				}
				else
				{
					bAlphaBlend = TRUE;
					dwSrcBlend = D3DBLEND_SRCALPHA;
					dwDstBlend = D3DBLEND_INVSRCALPHA;
				}

				D3DRESULT( d3d.SetRenderState( D3DRS_ALPHABLENDENABLE, bAlphaBlend ) );
				D3DRESULT( d3d.SetRenderState( D3DRS_SRCBLEND, dwSrcBlend ) );
				D3DRESULT( d3d.SetRenderState( D3DRS_DESTBLEND, dwDstBlend ) );
				D3DRESULT( d3d.SetRenderState( D3DRS_BLENDOP, dwBlendOp ) );
				D3DRESULT( d3d.SetFVF( FVF_SPRITE_VERTEX ) );
				D3DRESULT( d3d.SetStreamSource( 0, PrimaryVertices, 0, sizeof(sprite_vertex) ) );
				D3DRESULT( d3d.DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 ) );
			}
		}

		pSprite = (SpriteHandle *) pSprite->Next();

		if( !pSprite && pParent )
		{
			pSprite = (SpriteHandle *) pParent->Child();
			pParent = pSprite;
		}
	}

	PrimaryVertices->Release();

	D3DRESULT( d3d.EndScene() );

	return;
}

SpriteHandle *GetSpriteAsHandle( Sprite *pSprite )
{
	if( pSprite )
		return (SpriteHandle *) pSprite->GetHandle();

	return 0;
}