/*************************************/
/*                                   */
/*   SpriteMgr.h                     */
/*   new engine  03/29/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   2-d image manager               */
/*                                   */
/*************************************/

#ifndef __SPRITEMGR_H
#define __SPRITEMGR_H

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include "Layers/LayersStruct.h"			// low-level include
#include "Math/Vector.h"

class TextureDatabase;
class Texture;

/******************* forward declarations ****************/

/******************* defines *****************************/

/* indices for setting vertices, colors, uvs. some implementations may want a z-shaped order so it can be a tri strip */

#define SPRITE_INDEX0	0
#define SPRITE_INDEX1	1
#define SPRITE_INDEX2	2
#define SPRITE_INDEX3	3

/* sprite flags */

#define VERTEX_BY_REFERENCE		0x00000001	// use vertex pointer instead of self-contained vertices
#define UV_BY_REFERENCE				0x00000002	// use uv pointer instead of self-contained uv's
#define COLOR_BY_REFERENCE			0x00000004	// use color pointer instead of self-contained colors
#define WHITE_SPRITE					0x00000008	// all colors white
#define FULL_TEXTURE_SPRITE		0x00000010	// sprite uses full texture (i.e. uvs are all 0 or 1)
#define SPRITE_ALL_ONE_COLOR		0x00000020	// all vertices the same color
#define SPRITE_DISABLED				0x00000040	// sprite is temporarily disabled
#define SPRITE_RENDERED				0x00000080	// sprite has been added to render lists
#define SHADE_SPRITE					0x00000100	// use shade value to shade sprite
#define DIRECT_BIND					0x00000200	// u32TextureIndex is actual bind value, don't go through texture set's pu16BindList
#define SHADE_ALPHA_SPRITE			0x00000400	// use shade value to alpha sprite
#define SPRITE_BLENDMODE_ENABLED	0x00000800	// use additive render mode for this sprite

// sprites have blend modes now!

enum
{
	SPRITEBLEND_SOLID,
	SPRITEBLEND_NORMAL,
	SPRITEBLEND_ADD,
	SPRITEBLEND_SUBTRACT,
	SPRITEBLEND_MODULATE,
	SPRITEBLEND_GLOSS,
	SPRITEBLEND_DIFFUSE,
	SPRITEBLEND_SATURATE,
	SPRITEBLEND_DOUBLE,
	SPRITEBLEND_NEGATIVE,
};

/* default group and sort values */

#define SPRITE_DEFAULT_GROUP	0
#define SPRITE_DEFAULT_SORT	0x8000	// put in the middle of the 16-bit range so we can have stuff of lower priority

#if defined(WIN32) || defined(GCN)
#define SHADE_SHIFT		8
#define MAX_COLOR			255
#else
#define SHADE_SHIFT		7
#define MAX_COLOR			255
#endif

/* value to pass to update routine to not change a field */

#define SPRITE_NO_UPDATE		0xffff

/* for vu1 sprite renderer */

#define SPRITE_INIT_DATA_DMA_QWC						6	// amount of data to be dma'd to sprite setup data are-- must match defines in Sprites.vsm
#define SPRITE_VIF1_BASE_QWC							6	// total sprite single buffer data-- must match defines in Sprites.vsm. typically same as SPRITE_INIT_DATA_DMA_QWC, unless we want to reserve some scratch space in single buffer
#define SPRITE_DOUBLE_BUFFER_SIZE					(1024 - SPRITE_VIF1_BASE_QWC) / 2	// total vu data ram is 1024 words
#define SPRITE_DATA_HEADER_QWC						2												// 1 gs register, control word, giftag
#define MAX_SPRITES_PER_RENDER_PACKET				(SPRITE_DOUBLE_BUFFER_SIZE - SPRITE_DATA_HEADER_QWC) / 16	// 16 vu words per sprite (4 vertices, each nop/st/color/xyz, but first nop is tex0)
#define SPRITE_INIT_PACKET_QWC						(SPRITE_INIT_DATA_DMA_QWC + 8)		// how much to reserve on scratchpad in FastVars.cpp
#define SPRITE_DATA_PACKET_QWC						(8)											// how much to reserve on scratchpad in FastVars.cpp
#define SPRITE_INIT_MATRIX_QWC						4	// where matrix data goes in init packet

/* for ps2, the tex0 list buffer is half a quad word per entry. since memory to be dma'd to vram has to be quad word */
/* aligned, it's easier if max sprites per vu packet is even-- then we are guaranteed that tex0 list dma address will */
/* be aligned */

#define MAX_EVEN_SPRITES_PER_RENDER_PACKET		(MAX_SPRITES_PER_RENDER_PACKET & ~1)

/******************* macros ******************************/

/******************* structures **************************/

/******************* global variables ********************/

/******************* global prototypes *******************/

void SpriteMgr__Init(int nMaxSprites);
void SpriteMgr__Reset(void);
void SpriteMgr__Shutdown(void);
void SpriteMgr__SetNearClip(float nearClip);
void SpriteMgr__SetFarClip(float farClip);
float SpriteMgr__GetNearClip( void );
float SpriteMgr__GetFarClip( void );

#ifndef CONSUMER_BUILD
bool SpriteMgr__IsEmpty(void);
int SpriteMgr__FreeCount(void);
int SpriteMgr__ActiveCount(void);
#endif //CONSUMER_BUILD

void SpriteMgr__Render(void);
bool SpriteMgr__SetTextureDbl(t_Handle Handle);
void SpriteMgr__RemoveTextureDbl(t_Handle Handle);
t_Handle SpriteMgr__CurrentTextureDbl( void );
void SpriteMgr__CheckTextureDatabaseRemove(TextureDatabase *pTextureDatabase);
t_Handle SpriteMgr__RegisterSprite(u32 u32TextureIndex, u16 u16Group, u16 u16SortPriority, u32 u32Flags, Vector3 *pVertices, Vector2 *pUVs, ts_bRGBA *pColors, u32 piece = (u32)-1);
bool SpriteMgr__UpdateSprite(t_Handle Handle, u16 u16Group, u16 u16SortPriority, u32 u32Flags, Vector3 *pVertices, Vector2 *pUVs, ts_bRGBA *pColors);
bool SpriteMgr__GetDataPointers(t_Handle Handle, u32 *pu32Flags, Vector3 **ppVertices, Vector2 **ppUVs, ts_bRGBA **ppColors);
Texture* SpriteMgr__GetTexture(t_Handle Handle);
u32 SpriteMgr__GetPiece(t_Handle Handle);
bool SpriteMgr__ChangeSortPriority(t_Handle Handle, u16 SortValue);
u16 SpriteMgr__GetSortPriority(t_Handle Handle);
bool SpriteMgr__ChangeFlag(t_Handle Handle, u32 u32Flag);
bool SpriteMgr__EnableSprite(t_Handle Handle);
bool SpriteMgr__DisableSprite(t_Handle Handle);
void SpriteMgr__EnableSpriteGroup(u16 u16Group);
void SpriteMgr__DisableSpriteGroup(u16 u16Group);
bool SpriteMgr__KillSprite(t_Handle Handle);
void SpriteMgr__KillSpriteGroup(u16 u16Group);
void SpriteMgr__KillSpriteCategory(u16 u16Group, u16 mask);
typedef void (*SPRITEAFFECTFN)( t_Handle handle, void *data);
void SpriteMgr__AffectSpriteGroup(u16 u16Group, SPRITEAFFECTFN routine, void *data);
void SpriteMgr__AffectSpriteCategory(u16 u16Group, u16 mask, SPRITEAFFECTFN routine, void *data);
typedef void (*SPRITEKILLFN)( t_Handle handle);
void SpriteMgr__AffectAllSprites( SPRITEKILLFN routine);
void SpriteMgr__KillAllSprites(void);
void SpriteMgr__KillAllDisabledSprites(void);
bool SpriteMgr__ChangeShadeValue(t_Handle Handle, u8 u8Shade);
u8 SpriteMgr__GetShadeValue(t_Handle Handle);
u16 SpriteMgr__GetTexIndex( int iPieceIndex);
s16 SpriteMgr__GetTexWidth( int iPieceIndex);
inline s16 SpriteMgr__GetTexHeight( int iPieceIndex);
s16 SpriteMgr__GetWidth( int iPieceIndex);
s16 SpriteMgr__GetHeight( int iPieceIndex);
s16 SpriteMgr__GetHotSpotX( int iPieceIndex);
s16 SpriteMgr__GetHotSpotY( int iPieceIndex);
inline s16 SpriteMgr__GetXOffset( int iPieceIndex);
inline s16 SpriteMgr__GetYOffset( int iPieceIndex);
void SpriteMgr__ComputeUV_XYWH( Vector2 *pUVs, float x, float y, float pw, float ph, float tw, float th);
void SpriteMgr__ComputeUVs( Vector2 *pUVs, int iPieceIndex);
void SpriteMgr__ComputeVerticesWH( Vector3 *pVert, float left, float top, float w, float h);
void SpriteMgr__ComputeVertices( Vector3 *pVert, int iPieceIndex, float left, float top, float scalex, float scaley);
t_Handle SpriteMgr__AddSprite( t_Handle hTextureSet, u32 piece, float x, float y, float scalex, float scaley, u16 sid, u16 priority, u32 color, u8 alpha);
t_Handle SpriteMgr__AddBox( float x, float y, float w, float h, u16 sid, u16 priority, u32 color, u8 alpha);
void SpriteMgr__SetBlendMode( t_Handle Handle, u8 u32BlendMode );
u8 SpriteMgr__GetBlendMode( t_Handle Handle );

#ifndef CDROM
void SpriteMgr__GetTextureSize(t_Handle Handle, u32 *pu32Width, u32 *pu32Height);
t_Handle SpriteMgr__ShowDirectBindSprite(Texture* texture);
int SpriteMgr__GetSpritePopulation( void );
void SpriteMgr__DumpSpriteData();
#endif //CDROM

#ifndef CONSUMER_BUILD
void SpriteMgr__SetPlotBar(int nIndex, float fScale);
void SpriteMgr__SetPlotBar(float fScale);	// put value into bar 0, all values move down one bar
int SpriteMgr__GetPlotBarCount();
#endif //CONSUMER_BUILD

void SpriteMgr__SetTextureByIndex( t_Handle Handle, u32 u32TextureIndex );
void SpriteMgr__SetZTest( bool enable );

#endif // __SPRITEMGR_H
