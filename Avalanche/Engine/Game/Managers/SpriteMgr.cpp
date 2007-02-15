/*************************************/
/*                                   */
/*   SpriteMgr.c                     */
/*   new engine  03/29/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   2-d image manager               */
/*                                   */
/*************************************/

/* the sprite manager is responsible for rendering all the 2-d images on the game screen (excluding particles). this
includes text, HUD's, etc. sprites are registered with the manager, and then displayed as part of the render cycle */

/******************* includes ****************************/

#include "Game/GamePCH.h"



#include "Layers/Timing.h"
#include "EngineHelper/filestream.h"

#ifdef GCN
#include "Display/GCN/Shader.h"
#include "Display/GCN/FrameData.h"
#endif

#ifdef PS2
#include "Game/Managers/TextureMgr/PlatformImage.h"
#endif
/* UNPACK codes for VIF1 DMA packet */

#define S_32  (0)
#define S_16  (1)
#define S_8   (2)
#define V2_32 (4)
#define V2_16 (5)
#define V2_8  (6)
#define V3_32 (8)
#define V3_16 (9)
#define V3_8  (10)
#define V4_32 (12)
#define V4_16 (13)
#define V4_8  (14)
#define V4_5  (15)

/******************* defines *****************************/

/* bytes needed for sprite display list for ps2-- mostly dma and gs commands-- no data */

#define SPRITE_DISPLAY_LIST_BUFFER_SIZE	12000	// must be divisible by 16 to keep things dma-aligned

/******************* macros ******************************/

#define SPRITE_MGR_GET_RECORD(Handle)	&SpriteMgr.pSpriteAlloc[HANDLE_INDEX(Handle)]

/* ps2 glDrawArrays insists on 4-component vertices */

#ifdef WIN32
#define SPRITE_VERTEX_TYPE Vector3Packed
#elif defined (PS2)
#define SPRITE_VERTEX_TYPE Vector3Packed
#elif defined (GCN)
#define SPRITE_VERTEX_TYPE Vector3Packed
#endif //WIN32/PS2

/* macros for particular entries in dma packets, to ensure they are same for pre-build/update phases */

#define SPRITE_DATA_GIFTAG(addr, SpriteCount)																		\
																																	\
	MasterList::Append8(																										\
		addr,																														\
		SCE_GIF_SET_TAG(																										\
			SpriteCount,					/* nloop */																		\
			1,									/* eop */																		\
			1,									/* pre-- primitive enable */												\
			SCE_GS_SET_PRIM(				/* prim */																		\
				SCE_GS_PRIM_TRISTRIP,	/* prim(3)-- primitive type */											\
				1, 							/* iip(1)-- shading method (0 = flat, 1 = gouraud) */				\
				1,								/* tme(1)-- texture mapping enable */									\
				0,								/* fge(1)-- fogging enable */												\
				1,								/* abe(1)-- alpha blending enable */									\
				0,								/* aa1(1)-- antialiasing enable */										\
				0,								/* fst(1)-- texture coord (0 = STQ, 1 = UV) */						\
				0,								/* ctxt(1)-- context */														\
				0),							/* fix(1)-- fragment value control(0 = normal, 1 = fixed) */	\
			SCE_GIF_PACKED,				/* flag-- mode */																\
			0)									/* nreg-- register count-- 0 means 16 */								\
			)

/* the qwc and unpack calculations take data size into account */

#define SPRITE_DATA_TEX0_DMATAG(addr, VertexCount, Data)		MasterList::Append(addr, MasterList::createDMATAG(QWC_V2_32(VertexCount), TAGID_REF, Data)) // qwc, pce (priority control), id (type), irq, addr (for ref/refe/next/call types, or with 0x80000000 for scratchpad)
#define SPRITE_DATA_TEX0_UNPACK(addr, VertexCount)				MasterList::Append4(addr, SCE_VIF1_SET_UNPACK((SPRITE_DATA_HEADER_QWC + 0) | 0x8000, UNPACK_COUNT_V2_32(VertexCount), V2_32, 0))	// 0x8000 = use xtop register for double buffering
#define SPRITE_DATA_UV_DMATAG(addr, VertexCount, Data)		MasterList::Append(addr, MasterList::createDMATAG(QWC_V2_32_4X(VertexCount), TAGID_REF, Data)) // qwc, pce (priority control), id (type), irq, addr (for ref/refe/next/call types, or with 0x80000000 for scratchpad)
#define SPRITE_DATA_UV_UNPACK(addr, VertexCount)				MasterList::Append4(addr, SCE_VIF1_SET_UNPACK((SPRITE_DATA_HEADER_QWC + 1) | 0x8000, UNPACK_COUNT_V2_32_4X(VertexCount), V2_32, 0))	// 0x8000 = use xtop register for double buffering
#define SPRITE_DATA_COLOR_DMATAG(addr, VertexCount, Data)	MasterList::Append(addr, MasterList::createDMATAG(QWC_V4_8_4X(VertexCount), TAGID_REF, Data)) // qwc, pce (priority control), id (type), irq, addr (for ref/refe/next/call types, or with 0x80000000 for scratchpad)
#define SPRITE_DATA_COLOR_UNPACK(addr, VertexCount)			MasterList::Append4(addr, SCE_VIF1_SET_UNPACK((SPRITE_DATA_HEADER_QWC + 2) | 0x8000 | 0x4000, UNPACK_COUNT_V4_8_4X(VertexCount), V4_8, 0))	// 0x8000 = use xtop register for double buffering, 0x4000 = unsigned unpacking
#define SPRITE_DATA_VERTEX_DMATAG(addr, VertexCount, Data)	MasterList::Append(addr, MasterList::createDMATAG(QWC_V3_32_4X(VertexCount), TAGID_REF, Data)) // qwc, pce (priority control), id (type), irq, addr (for ref/refe/next/call types, or with 0x80000000 for scratchpad)
#define SPRITE_DATA_VERTEX_UNPACK(addr, VertexCount)			MasterList::Append4(addr, SCE_VIF1_SET_UNPACK((SPRITE_DATA_HEADER_QWC + 3) | 0x8000, UNPACK_COUNT_V3_32_4X(VertexCount), V3_32, 0))	// 0x8000 = use xtop register for double buffering

/* number of general purpose plot bars across bottom of screen */

#define MAX_PLOT_BARS		40
#define PLOT_BAR_BOTTOM		0.98f	// i want a small gap below bottom of bar for accurate reading
#define PLOT_BAR_HEIGHT		0.1f	// size of plot bar at scale == 1.0
#define PLOT_BAR_GAP_MULT	0.5f	// relation of gap width to plot width

/******************* structures **************************/

/* entry in a singly-linked list of sprites */

struct ts_Sprite
{
	ts_Sprite			*pNext;				// linked list pointer-- does not have to be first
	ts_Sprite			*pPrev;				// linked list pointer

	u32					u32Flags;
	float					fLifetime;			// in seconds

	Texture*				pTexture;
	u16					u16Group;			// sprites can be grouped, and enabled/disabled/killed by group
	u16					u16SortPriority;	// higher priority sprites are rendered last, so they come out on top (since z-buffer is off)
	t_Handle				Handle;				// assigned handle for invalid handle checking

	u8						u8Shade;
	u8                u8BlendMode;
	u32					u32Piece;			// remember piece
	t_Handle				hTextureDbl;

	/* record can have self-contained vertices, or point to vertices to be maintained by game */

	Vector3				Vertices[4];
	Vector3				*pVertices;

	/* record can have self-contained uv's, or point to uv's to be maintained by game */

	Vector2				UVs[4];
	Vector2				*pUVs;

	/* record can have self-contained colors, or point to colors to be maintained by game */

	union {
		ts_bRGBA				Colors[4];
		ts_bRGBA				*pColors;
	};
};

/* sprite manager is basically just an active list and a free list of sprites */

class CSpriteMgr
{
public:
	CSpriteMgr() : textureArray(256) {}

	/* record passed initialization values */
	int						nMaxSprites;

	/* viewport */
	float						NearClip;
	float						FarClip;

	/* sprite records */
	ts_Sprite				*pSpriteAlloc;	// original alloc'd buffer
	ts_DLinkList			FreeList;
	ts_DLinkList			ActiveList;

	/* active texture set for registering sprites */
	TextureDatabase			*pTextureDatabase;
	t_Handle					hTextureDbl;

	/* for handle assignment (id portion of handle) */
	u16						u16ID;

	/* lists for rendering */
	SPRITE_VERTEX_TYPE	*pVertexList;
	Vector2					*pUVList;
	ts_bRGBA					*pColorList;

#ifdef PS2
	/* ps2 needs texture list, and a toggle */
	u64						*pTEX0List;
#endif //PS2
	u32						u32Toggle;

#ifndef CONSUMER_BUILD
	/* for rendering profile timing bars */
	t_Handle					*pBarHandleList;
	int						nLastBars;
#endif //CONSUMER_BUILD

#ifndef CONSUMER_BUILD
	/* for rendering general purpose bars across bottom */
	t_Handle					PlotBarHandle[MAX_PLOT_BARS];
	bool						bPlotBarsActivated;
#endif //CONSUMER_BUILD

	TextureArray			textureArray;
	bool						zTestEnabled;
};

/******************* externs *****************************/

#ifdef PS2
extern unsigned int	SpriteVU1CodeDMA	__attribute__( ( section( ".vudata" )));
extern unsigned int	SpritesEnd			__attribute__( ( section( ".vudata" )));
extern unsigned int	SpritesVUMemStart	__attribute__( ( section( ".vudata" )));
extern unsigned int	SpritesVUMemEnd	__attribute__( ( section( ".vudata" )));
#endif //PS2

/******************* global variables ********************/

/******************* local variables *********************/

/* the sprite manager */

static CSpriteMgr SpriteMgr;

/* full texture uvs */

static Vector2 FullTextureUVs[4] =
{
	Vector2(0.0f, 0.0f), 
	Vector2(1.0f, 0.0f), 
	Vector2(1.0f, 1.0f), 
	Vector2(0.0f, 1.0f)
};

/* all white colors */
#ifdef WIN32
static ts_bRGBA WhiteColors[4] = {{255, 255, 255, BYTE_FULL_ALPHA}, {255, 255, 255, BYTE_FULL_ALPHA}, {255, 255, 255, BYTE_FULL_ALPHA}, {255, 255, 255, BYTE_FULL_ALPHA}};
#elif defined(GCN)
static ts_bRGBA WhiteColors[4] = {{255, 255, 255, BYTE_FULL_ALPHA}, {255, 255, 255, BYTE_FULL_ALPHA}, {255, 255, 255, BYTE_FULL_ALPHA}, {255, 255, 255, BYTE_FULL_ALPHA}};
#else
static ts_bRGBA WhiteColors[4] = {{128, 128, 128, BYTE_FULL_ALPHA}, {128, 128, 128, BYTE_FULL_ALPHA}, {128, 128, 128, BYTE_FULL_ALPHA}, {128, 128, 128, BYTE_FULL_ALPHA}};
#endif

/******************* local prototypes ********************/

ts_Sprite *SpriteMgr__AllocRecord(void);
void SpriteMgr__SortActiveList(void);
#ifndef CONSUMER_BUILD
void SpriteMgr__UpdateProfileBars(void);
void SpriteMgr__ResetProfileBars(void);
#endif //CONSUMER_BUILD
#ifndef CONSUMER_BUILD
void SpriteMgr__ActivatePlotBars();
#endif //CONSUMER_BUILD
#ifdef PS2
void SpriteMgr__BuildInitDMAPacket(void);
void SpriteMgr__BuildDataDMAPacket(void);
#endif //PS2

/******************* functions ***************************/

/* allocate sprite records */

void SpriteMgr__Init(int nMaxSprites)
{
	ts_Sprite *pSprite;
#ifdef PS2
	int i;
	SPRITE_VERTEX_TYPE *pVertex;
#endif //PS2

	/* clear manager */


	SpriteMgr.pTextureDatabase = NULL;
	SpriteMgr.hTextureDbl = INVALID_HANDLE;


	/* linked lists */

	ASSERT(nMaxSprites < MAX_HANDLES);	// see definition of MAX_HANDLES
	SpriteMgr.nMaxSprites = nMaxSprites;
	MEM_SET_ALLOC_NAME("SpriteMgr list");
	SpriteMgr.pSpriteAlloc = (ts_Sprite *) memAlloc(SpriteMgr.nMaxSprites * sizeof(ts_Sprite));
	DLinkList__Init(&SpriteMgr.FreeList, offsetof(ts_Sprite, pNext), offsetof(ts_Sprite, pPrev));
	DLinkList__LinkBlock(&SpriteMgr.FreeList, SpriteMgr.pSpriteAlloc, SpriteMgr.nMaxSprites, sizeof(ts_Sprite));
	DLinkList__Init(&SpriteMgr.ActiveList, offsetof(ts_Sprite, pNext), offsetof(ts_Sprite, pPrev));

	/* clear all handles */

	pSprite = (ts_Sprite *) SpriteMgr.FreeList.pHead;
	while (pSprite)
	{
		pSprite->Handle = INVALID_HANDLE;
		pSprite = pSprite->pNext;
	}

	SpriteMgr__SetNearClip( 0.0f );
	SpriteMgr__SetFarClip( 1.0f );

	/* malloc room for rendering lists */

#if defined(PS2) || defined(GCN)
#define SPRITE_MULTIPLIER 2	// malloc twice as much so we can double-buffer for master display list
#else //PS2
#define SPRITE_MULTIPLIER 1
#endif //PS2
	MEM_SET_ALLOC_NAME("SpriteMgr vertex");
	SpriteMgr.pVertexList = (SPRITE_VERTEX_TYPE *) memAlloc(SpriteMgr.nMaxSprites * 4 * SPRITE_MULTIPLIER * sizeof(SPRITE_VERTEX_TYPE));	// each sprite contributes 4 vertices
	MEM_SET_ALLOC_NAME("SpriteMgr UV");
	SpriteMgr.pUVList = (Vector2 *) memAlloc(SpriteMgr.nMaxSprites * 4 * SPRITE_MULTIPLIER * sizeof(Vector2));		// each sprite contributes 4 vertices
	MEM_SET_ALLOC_NAME("SpriteMgr Color");
	SpriteMgr.pColorList = (ts_bRGBA *) memAlloc(SpriteMgr.nMaxSprites * 4 * SPRITE_MULTIPLIER * sizeof(ts_bRGBA));	// each sprite contributes 4 vertices
#ifdef PS2
	MEM_SET_ALLOC_NAME("SpriteMgr TEX0");
	SpriteMgr.pTEX0List = (u64 *) memAlloc(SpriteMgr.nMaxSprites * SPRITE_MULTIPLIER * sizeof(u64));	// each sprite contributes 1 tex0 value
#endif //PS2

#ifdef PS2
	/* build dma packets for vu1 renderer */

	SpriteMgr__BuildInitDMAPacket();
	SpriteMgr__BuildDataDMAPacket();
#endif //PS2

#ifndef CONSUMER_BUILD
	/* no plot bars */

	for (int i = 0; i < MAX_PLOT_BARS; i++)
		SpriteMgr.PlotBarHandle[i] = INVALID_HANDLE;
	SpriteMgr.bPlotBarsActivated = false;
#endif //CONSUMER_BUILD
}

/* reset */

void SpriteMgr__Reset(void)
{
	/* kill all sprites */
	SpriteMgr__KillAllSprites();
	ASSERT(SpriteMgr__IsEmpty());

	// clear texture array (don't call Clear() though because it will
	// release our memory, in turn causing fragmentation)
	while (SpriteMgr.textureArray.TextureCount() > 0)
		SpriteMgr.textureArray.RemoveTexture(SpriteMgr.textureArray.GetTexture(0));

	/* no known texture sets */
	SpriteMgr.pTextureDatabase = NULL;
	SpriteMgr.hTextureDbl = INVALID_HANDLE;

	SpriteMgr.zTestEnabled = false;

#ifndef CONSUMER_BUILD
	/* clean up profile timing bars */

	SpriteMgr__ResetProfileBars();
#endif //CONSUMER_BUILD

#ifndef CONSUMER_BUILD
	/* no plot bars */

	for (int i = 0; i < MAX_PLOT_BARS; i++)
		SpriteMgr.PlotBarHandle[i] = INVALID_HANDLE;
	SpriteMgr.bPlotBarsActivated = false;
#endif //CONSUMER_BUILD

}

/* shutdown */

void SpriteMgr__Shutdown(void)
{
	/* maybe not active */

	if (SpriteMgr.pSpriteAlloc == NULL)
		return;

	/* free memory */

	memFree(SpriteMgr.pSpriteAlloc);
	memFree(SpriteMgr.pVertexList);
	memFree(SpriteMgr.pUVList);
	memFree(SpriteMgr.pColorList);
#ifdef PS2
	memFree(SpriteMgr.pTEX0List);
#endif //PS2

	/* clean up profile timing bars */

#ifndef CONSUMER_BUILD
	SpriteMgr__ResetProfileBars();
#endif //CONSUMER_BUILD
}

void SpriteMgr__SetNearClip(float nearClip)
{
	SpriteMgr.NearClip = nearClip;
}

void SpriteMgr__SetFarClip(float farClip)
{
	SpriteMgr.FarClip = farClip;
}

float SpriteMgr__GetNearClip( void )
{
	return SpriteMgr.NearClip;
}

float SpriteMgr__GetFarClip( void )
{
	return SpriteMgr.FarClip;
}

#ifndef CONSUMER_BUILD
bool SpriteMgr__IsEmpty(void)
{
	return(SpriteMgr__FreeCount() == (signed) SpriteMgr.nMaxSprites);
}
int SpriteMgr__FreeCount(void)
{
	return(DLinkList__Check(&SpriteMgr.FreeList));
}
int SpriteMgr__ActiveCount(void)
{
	return(DLinkList__Check(&SpriteMgr.ActiveList));
}
#endif //CONSUMER_BUILD

/* render active sprites */

void SpriteMgr__Render(void)
{
	static const int BlendModeLookup[] =
	{
		RS_BLENDFUNC_NONE,			// SPRITEBLEND_SOLID
		RS_BLENDFUNC_DEFAULT,		// SPRITEBLEND_NORMAL
		RS_BLENDFUNC_ADDITIVE,		// SPRITEBLEND_ADD
		RS_BLENDFUNC_SUBTRACTIVE,	// SPRITEBLEND_SUBTRACT
		RS_BLENDFUNC_NONE,			// SPRITEBLEND_MODULATE,
		RS_BLENDFUNC_GLOSS,			// SPRITEBLEND_GLOSS,
		RS_BLENDFUNC_DIFFUSE,		// SPRITEBLEND_DIFFUSE,
		RS_BLENDFUNC_SATURATE,		// SPRITEBLEND_SATURATE,
		RS_BLENDFUNC_DOUBLE,			// SPRITEBLEND_DOUBLE,
		RS_BLENDFUNC_NEGATIVE,		// SPRITEBLEND_NEGATIVE,
	};


//#define _DEBUG_DAMN_SORT_ORDER
#ifdef _DEBUG_DAMN_SORT_ORDER
	// On this frame, we'll record which sprites were rendered and the order they were ACTUALLY rendered instead of
	// what we thought it would be. At the end of this frame, we'll loop through the rendered sprites in order and
	// check make sure they were sorted in ascending order, i.e 0, 1, 2, 3, 4, ... 500, 600, etc..
	// If we come across a sprite that is out of order, we'll mark it--its pointer is put in
	//
	//     ts_Sprite* this_sprite_is_not_sorted;
	//
	// and the previous sprite before this is stored in
	//
	//		ts_Sprite* next_sprite_is_not_sorted.
	//
	// I've strategically placed some ASSERTS that will brake when the THE NEXT sprite is rendered out of order,
	// helps you to step through the code and see exactly what happens at the moment is happens.
	// This code is a hog and only should be turned on when you need it, usually after making changes to the
	// sprite code that might affect the render order and you want to check it.
	static ts_Sprite* SpriteOrder[5000];
	static u16 SpritePriorities[5000];
	memset( SpriteOrder, 0, sizeof(SpriteOrder) );
	memset( SpritePriorities, 0, sizeof(SpritePriorities) );
	static ts_Sprite* next_sprite_is_not_sorted = 0;
	static ts_Sprite* this_sprite_is_not_sorted = 0;
	int n = 0;
#endif // _DEBUG_DAMN_SORT_ORDER

   int i;
	u16 r, g, b;
	u32 u32SpriteCount;
	u32 u32LastSortPriority;
	bool bSortNeeded;
#ifndef PS2	// ps2 just renders sprites in order, doesn't bucket by texture context
	Texture* pTexture;
	u16 u16SortPriority;		// which priority current group of sprites being rendered is using
#endif //PS2
	u32 u32ListIndex;			// index into data lists (e.g vertex list) for current sprite
	u32 u32StartIndex;		// index into data lists (e.g vertex list) for current set of sprites (all same texture)
	ts_Sprite *pSprite;
	ts_Sprite *pSpriteStart;
	Vector3 *pVertices;
	Vector2 *pUVs;
	ts_bRGBA *pColors;
	u8 u8BlendMode;

	/* sprite manager must be running */

	ASSERT(SpriteMgr.pSpriteAlloc != NULL);	// if this hits, SpriteMgr__Init hasn't been called (or sprite manager has been shutdown)

	/* add profile timing bars */

#ifndef CONSUMER_BUILD
	SpriteMgr__UpdateProfileBars();
#endif //CONSUMER_BUILD

// on ps2, we aren't sure of the gs state at this point? some gs commands get baked into display lists, have to account for them
// TJC - it has to come before MatrixStack__FullScreenViewport();
//  since that updates the scissor register & doesnt flush...
// TJC - is this really nessisary?
// TJC - the GSState__Clear() has now bitten me once.
// TJC - second time its caused havoc for me.
// TJC - and a third... this is getting annoying..
#ifdef PS2
	GSState__Clear();
	RenderState__ForceAll();	
#elif defined(DIRECTX)
	RenderState__ForceAll( );
	RenderState__Default( );
#endif //PS2

	/* quick exit */

	if (SpriteMgr.ActiveList.pHead == NULL)
		return;

	/* clear flag that tells us whether sprite still needs to be processed. also determine whether active list */
	/* needs to be sorted */

	u32SpriteCount = 0;
	pSprite = (ts_Sprite *) SpriteMgr.ActiveList.pHead;
	bSortNeeded = FALSE;
	u32LastSortPriority = 0;
	while (pSprite)
	{
		if (pSprite->u32Flags & SPRITE_DISABLED)
			pSprite->u32Flags |= SPRITE_RENDERED;
		else
		{
			pSprite->u32Flags &= ~SPRITE_RENDERED;
			u32SpriteCount++;

			/* out of sort order? */

			if (pSprite->u16SortPriority < u32LastSortPriority)
				bSortNeeded = TRUE;
			u32LastSortPriority = pSprite->u16SortPriority;
		}
		pSprite = pSprite->pNext;
	}
	if (u32SpriteCount == 0)
		return;

	/* do we need to sort the active list? */

	if (bSortNeeded)
		SpriteMgr__SortActiveList();	// higher priorities rendered last so they show up on top (z-buffer is off)

	/* make sure we're not scissored out of any part of the screen */

	MatrixStack__FullScreenViewport();

	/* clear but don't bother saving texture matrix */

	TextureState__RequestUnit(0);	// sprites always use texture unit 0
	MatrixStack__Mode(MS_TEXTURE);
	MatrixStack__Identity();

	/* save and clear projection matrix for 2d stuff */

	MatrixStack__Mode( MS_PROJECTION );
	MatrixStack__Push();
#ifdef GCN
	MatrixStack__Orthographic( -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f );
#endif
	MatrixStack__Identity();

	/* save and setup model matrix for 2d stuff */

	MatrixStack__Mode( MS_MODELVIEW );
	MatrixStack__Push();
	MatrixStack__Identity();

#ifdef PS2
	/* screen y runs from low at top to high at bottom-- backwards from pc opengl */
	MatrixStack__Scale( 2.0f,  2.0f, 1.0f );
#else
	MatrixStack__Scale( 2.0f, -2.0f, 1.0f );
#endif

#ifdef GCN
	MatrixStack__Translate( -0.5f, -0.5f, -1.0f );
#else
	MatrixStack__Translate( -0.5f, -0.5f, 0.0f );
#endif

#ifdef GCN
	g_ShaderMgr.Reset( );
#endif
	
	/* default render state, and a couple of sprite-specific things */

	RenderState__Default();

	RenderState__Request( RS_PRIMITIVE, RS_PRIMITIVE_DRAWARRAY );	// we are rendering through draw arrays
	/* note: RS_ZFUNC request used to be RenderState__Request(RS_ZTEST, FALSE) to just turn off the z-test, but */
	/* apparently on the ps2 that also turns off writes to the z-buffer on the gs, so do it this way instead */
	
	// Culling is fubar'd (probably because of the -1 scale in the viewport), so I have to put this here.
	RenderState__Request( RS_CULL, RS_CULL_NONE );

	/* fog is in some weird mode on directx, so turn off fog */

	RenderState__Request(RS_FOG, RS_FALSE);

	SpriteMgr.textureArray.Lock();

	RenderState__Request( RS_ZWRITE, RS_FALSE );			// don't write to z-buffer since we're ignoring it

#ifndef PS2
	RenderState__Request( RS_ZFUNC, SpriteMgr.zTestEnabled ? RS_ZFUNC_LEQUAL : RS_ZFUNC_ALWAYS );
#else
	// NOTE ::: the GSState__Set(SCE_GS_TEST_1) below overrides this!!!
	// this is just here so our shadow registers match.
	RenderState__Request( RS_ZFUNC, SpriteMgr.zTestEnabled ? RS_ZFUNC_GEQUAL : RS_ZFUNC_ALWAYS );
#endif

#ifdef PS2
	
	// Jim Henn is reporting seeing the 0-alpha parts of sprites - try setting dither matrix to positive to eliminate this
	// switch to all positive dither matrix
	GSState__Set(SCE_GS_DIMX, GSState.POSDIMX);

	/* activate desired gs state */
	GSState__Set(SCE_GS_TEST_1, SCE_GS_SET_TEST_1(
							 1, 	// ate, alpha test enable, 1 = on
							 0x001, 	// atst, alpha test method, 111 = notequal
							 0x80, // aref, alpha reference value for alpha test, 0x80 = opaque
							 0x0, 	// afail, what to do when alpha test fails, 0 = KEEP (transparent wireframe) 10=ZB_ONLY (opaque wireframe)
							 0, 	// date, destination alpha test, 0 = off, 1 = on
							 1, 	// datm, destination alpha test mode, 1 bit
							 1, 	// zte, depth test, 1 = on (TRC Requirement)
							 SpriteMgr.zTestEnabled ? 0x2 : 0x1		// ztst, depth test method, 0 = never, 1 = always, 2 = gequal, 3 = greater
							 ));

	GSState__Send();


//	MatrixStack__Scale( 1.0f, -1.0f, 1.0f );

	/* send down init packet-- downloads vu1 rendering code, and transform matrix to use. to get transform matrix, */
	/* combine modelview matrix, clip-to-screen into world-to-screen matrix. sprite coordinates are in, what should */
	/* we call it, normalized screen space? that is, x runs from 0.0 to 1.0, same with y and z. (x = 0.0, y = 0.0) */
	/* is top left of screen. z = 0.0 is near clip plane, z = 1.0 is far clip plane. so this is really close to clip */
	/* space (x and y range = -1.0 to 1.0, z runs 0.0 to 1.0), and that's what the modelview matrix does-- converts */
	/* x and y from (0.0 to 1.0) to (-1.0 to 1.0), leaving z alone. then the clip-to-screen handles converting to */
	/* screen space (pixel coordinates)-- it's just the viewport matrix */

//	CHECK_LAST_FROM_SPR_DMA(u8SpriteInitPacket);
	*(Matrix4x4 *) &u8SpriteInitPacket[SPRITE_INIT_MATRIX_QWC * BYTES_PER_QUAD_WORD] = *MatrixStack__Get(MS_MODELVIEW) * (Matrix4x4) *MatrixStack__GetViewport();
	COPY_TO_MASTER_LIST(u8SpriteInitPacket, SPRITE_INIT_PACKET_QWC);

	// Tell masterlist that it needs to fix base & offset and it will need to flush when things start back up...
	MasterList::SetFlushed(false);
	MasterList::SetBase(SPRITE_VIF1_BASE_QWC);
	MasterList::SetOffset(SPRITE_DOUBLE_BUFFER_SIZE);
	PS2Renderer::JustTrashedVUInstructionMem((u_int)&SpritesVUMemStart, (u_int)&SpritesVUMemEnd - (u_int)&SpritesVUMemStart);

//yuch-- don't know why this is necessary (or even IF it's necessary at this point)

#else //PS2

	/* go through the sprites, grouping them by texture. when we run out of sprites of a given texture (or reach */
	/* some other limit, like vertex count on ps2), then render that set of data via glDrawArrays */

	DrawArray__VectorPointer( sizeof ( SPRITE_VERTEX_TYPE ) / sizeof ( float ), DA_FLOAT, 0, SpriteMgr.pVertexList );
	DrawArray__TexturePointer( 2, DA_FLOAT, 0, SpriteMgr.pUVList );
	DrawArray__ColorPointer( 4, DA_UBYTE, 0, SpriteMgr.pColorList );
	DrawArray__Enable( DA_VECTOR );
	DrawArray__Enable( DA_TEXTURE );
	DrawArray__Enable( DA_COLOR );
#endif //PS2

	/* start building data */

#if defined(WIN32)
	u32ListIndex = 0;	// index into vertex/UV/color lists
#elif defined (PS2) || defined(GCN)
	u32ListIndex = SpriteMgr.u32Toggle * (SpriteMgr.nMaxSprites * 4);	// double buffer in lists-- toggle is 0 or 1
#endif //WIN32/PS2
	pSpriteStart = (ts_Sprite *) SpriteMgr.ActiveList.pHead;	// start at head of sprite list to look for unprocessed texture
	while (pSpriteStart)
	{
		/* find next un-processed texture */

		pSprite = pSpriteStart;
		while ((pSprite) && (pSprite->u32Flags & SPRITE_RENDERED))
			pSprite = pSprite->pNext;
		if (pSprite == NULL)
			pSpriteStart = NULL;	// no unrendered sprites, we're done
		else
		{
			/* update place to start looking next time */

			pSpriteStart = pSprite->pNext;

			/* record list starting place */

			u32StartIndex = u32ListIndex;

#ifndef PS2	// ps2 just renders sprites in order, doesn't bucket by texture context
			/* record texture */

			pTexture        = pSprite->pTexture;
			u16SortPriority = pSprite->u16SortPriority;
			u8BlendMode     = pSprite->u8BlendMode;
#endif //PS2

			/* go through sprites looking for other ones that match this texture. if found, add to list */

			while (pSprite)
			{
				if ((pSprite->u32Flags & SPRITE_RENDERED) == 0)
				{
#ifndef PS2	// ps2 just renders sprites in order, doesn't bucket by texture context
					if( pSprite->u16SortPriority != u16SortPriority )
					{
						/* sprite doesn't match u16Bind of first sprite in group. if it has a different priority, then */
						/* we have to stop current group to preserve sort order */
						pSprite = NULL;	// end this list, start another one
					}
					else
					if ((pSprite->pTexture == pTexture) && pSprite->u8BlendMode == u8BlendMode)
#endif //PS2
					{
					#ifdef _DEBUG_DAMN_SORT_ORDER
						// when this hits, the next sprite to render is not sorted correctly!
						ASSERT(pSprite != next_sprite_is_not_sorted);
						// when this hits, the current sprite to be rendered is not sorted correctly!
						ASSERT(pSprite != this_sprite_is_not_sorted);
						// when this hits, the sort order is blatenly wrong
						#ifndef PS2
						ASSERT(pSprite->u16SortPriority >= u16SortPriority);
						#endif
						SpritePriorities[n] = pSprite->u16SortPriority;
						SpriteOrder[n++] = pSprite;
					#endif // _DEBUG_DAMN_SORT_ORDER

						/* mark sprite as processed */

						pSprite->u32Flags |= SPRITE_RENDERED;

						/* add sprite data to lists */

						if (pSprite->u32Flags & VERTEX_BY_REFERENCE)
							pVertices = pSprite->pVertices;
						else
							pVertices = pSprite->Vertices;	// sprite's internal data
#if defined(PS2)
						/* z-order */
						memcpy(&SpriteMgr.pVertexList[u32ListIndex + 0], &pVertices[0], sizeof(SPRITE_VERTEX_TYPE));
						memcpy(&SpriteMgr.pVertexList[u32ListIndex + 1], &pVertices[1], sizeof(SPRITE_VERTEX_TYPE));
						memcpy(&SpriteMgr.pVertexList[u32ListIndex + 2], &pVertices[3], sizeof(SPRITE_VERTEX_TYPE));
						memcpy(&SpriteMgr.pVertexList[u32ListIndex + 3], &pVertices[2], sizeof(SPRITE_VERTEX_TYPE));
#else
						memcpy(&SpriteMgr.pVertexList[u32ListIndex], pVertices, 4 * sizeof(SPRITE_VERTEX_TYPE));
#endif

						/* uv's */

						if (pSprite->pTexture != NULL)
						{
							if (pSprite->u32Flags & UV_BY_REFERENCE)
								pUVs = pSprite->pUVs;
							else
								pUVs = pSprite->UVs;				// sprite's internal data
#if defined(PS2)
							/* z-order */

							memcpy(&SpriteMgr.pUVList[u32ListIndex + 0], &pUVs[0], 2 * sizeof(Vector2));
							memcpy(&SpriteMgr.pUVList[u32ListIndex + 2], &pUVs[3], sizeof(Vector2));
							memcpy(&SpriteMgr.pUVList[u32ListIndex + 3], &pUVs[2], sizeof(Vector2));

							/* we're using the bottom bit of the first u-value as a default/additive render flag. i believe */
							/* that the bit is way past the ps2's uv resolution, so it's totally safe */

							u32 *pu32UValue = (u32 *) &SpriteMgr.pUVList[u32ListIndex + 0];
							*pu32UValue &= ~1;
							if (pSprite->u8BlendMode == SPRITEBLEND_ADD )
								*pu32UValue |= 1;
#else
							memcpy(&SpriteMgr.pUVList[u32ListIndex], pUVs, 4 * sizeof(Vector2));
#endif
						}

						/* colors */

						if (pSprite->u32Flags & COLOR_BY_REFERENCE)
							pColors = pSprite->pColors;
						else
							pColors = pSprite->Colors;		// sprite's internal data
						if (pSprite->u32Flags & SPRITE_ALL_ONE_COLOR)
						{
							/* only the first color guaranteed correct */

							memcpy(&SpriteMgr.pColorList[u32ListIndex + 0], pColors, sizeof(ts_bRGBA));
							memcpy(&SpriteMgr.pColorList[u32ListIndex + 1], pColors, sizeof(ts_bRGBA));
							memcpy(&SpriteMgr.pColorList[u32ListIndex + 2], pColors, sizeof(ts_bRGBA));
							memcpy(&SpriteMgr.pColorList[u32ListIndex + 3], pColors, sizeof(ts_bRGBA));
						}
						else
						{
#if defined(PS2)
							/* z-order */

							memcpy(&SpriteMgr.pColorList[u32ListIndex + 0], &pColors[0], 2 * sizeof(ts_bRGBA));
							memcpy(&SpriteMgr.pColorList[u32ListIndex + 2], &pColors[3], sizeof(ts_bRGBA));
							memcpy(&SpriteMgr.pColorList[u32ListIndex + 3], &pColors[2], sizeof(ts_bRGBA));
#else
							memcpy(&SpriteMgr.pColorList[u32ListIndex], pColors, 4 * sizeof(ts_bRGBA));
#endif
						}

#ifdef PS2
						/* tex0 value for texture bind-- "/ 4" is because this is one per sprite */

						pSprite->pTexture->GetImage()->GetPlatformImage()->GetRegisterValues(&SpriteMgr.pTEX0List[u32ListIndex / 4], NULL, NULL, NULL, NULL);
#endif //PS2

						if ( pSprite->u32Flags & SHADE_SPRITE)
						{
							for( i=0; i<4; i++)
							{
								r = ( pSprite->Colors[i].R * pSprite->u8Shade) >> SHADE_SHIFT;		// divide by 0x80
								if ( r > MAX_COLOR) r = MAX_COLOR;
								g = ( pSprite->Colors[i].G * pSprite->u8Shade) >> SHADE_SHIFT;		// divide by 0x80
								if (r > MAX_COLOR) g = MAX_COLOR;
								b = ( pSprite->Colors[i].B * pSprite->u8Shade) >> SHADE_SHIFT;		// divide by 0x80
								if (r > MAX_COLOR) b = MAX_COLOR;

								SpriteMgr.pColorList[u32ListIndex + i].R = (u8) r;
								SpriteMgr.pColorList[u32ListIndex + i].G = (u8) g;
								SpriteMgr.pColorList[u32ListIndex + i].B = (u8) b;
							}
						}
						else
						if ( pSprite->u32Flags & SHADE_ALPHA_SPRITE)
						{
							for( i=0; i<4; i++)
							{
								u16 a = ( pSprite->Colors[i].A * pSprite->u8Shade) >> SHADE_SHIFT;		// divide by 0x80
								if ( a > BYTE_FULL_ALPHA) a = BYTE_FULL_ALPHA;

								SpriteMgr.pColorList[u32ListIndex + i].A = (u8) a;
							}
						}

						/* update list index, list vertex count */

						u32ListIndex += 4;
					}
				}

				/* to next sprite */

				if (pSprite)	// if still going...
					pSprite = pSprite->pNext;

#ifdef PS2
				/* there is a limit to how many vertices in an array for ps2 (based on size of VU RAM) */

				if (u32ListIndex - u32StartIndex >= MAX_EVEN_SPRITES_PER_RENDER_PACKET * 4)	// 4 vertices per sprite
					pSprite = NULL;	// end this list, start another one
#endif //PS2
			}

			/* render this set of data */

#ifndef PS2
			if (pTexture == NULL)
				TextureState__RequestAll( 0, TS_TYPE_NONE, 0, 0, 0, 0, 0 );
			else
				TextureState__RequestAll( 0, TS_TYPE_2D, pTexture, TS_METHOD_MODULATE, TS_WRAP_CLAMP, TS_WRAP_CLAMP, TS_FILTER_BILINEAR );

			RenderState__Request( RS_BLENDFUNC, BlendModeLookup[u8BlendMode] );
			RenderState__Begin();
#ifdef GCN
			DCStoreRangeNoSync( (void *)( SpriteMgr.pVertexList + u32StartIndex), sizeof(SPRITE_VERTEX_TYPE) * ( u32ListIndex - u32StartIndex));
			DCStoreRangeNoSync( (void *)( SpriteMgr.pUVList + u32StartIndex), sizeof(Vector2) * ( u32ListIndex - u32StartIndex));
			DCStoreRangeNoSync( (void *)( SpriteMgr.pColorList + u32StartIndex), sizeof(ts_bRGBA) * ( u32ListIndex - u32StartIndex));
			PPCSync( );
#endif
			DrawArray__Render( DA_QUADS, u32StartIndex, u32ListIndex - u32StartIndex );
			RenderState__End();

			TextureState__Request( 0, TS_BIND, NULL );
	
#else //PS2
			/* build up dma tags to send the collected data-- fill in variable data in pre-built dma packet */

			// wait for prior dma of this block to finish before modifying contents
//			CHECK_LAST_FROM_SPR_DMA(u8SpriteDataPacket);

			u_char* addr = (u_char*) u8SpriteDataPacket + BYTES_PER_QUAD_WORD;	// skip dma tag

			/* control word */

			u32 u32VertexCount = u32ListIndex - u32StartIndex;
			MasterList::Append4(addr, u32VertexCount);	// x-component
			addr += 12;								// skip unused y/z/w-components

			/* giftag to send down all sprites as a tri-strip-- adc bit is set by vu code to break strip up into quads. */
			/* nloop count in giftag is sprite count, not vertex count */

			SPRITE_DATA_GIFTAG(addr, (u32VertexCount / 4));	// use macro to ensure same as update in SpriteMgr__BuildDataDMAPacket
			addr += 8;								// skip register list

			/* then tex0 data-- one per sprite. this list is one entry per sprite, while all others are 4 entries per */
			/* sprite (e.g. vertices). this should always be aligned because we always fill the lists for dma purposes */
			/* with an even number of sprites. since each tex0 entry is a half quad word, two entries are quad word */
			/* aligned */

			ASSERT(IS_DMA_ALIGNED(&SpriteMgr.pTEX0List[u32StartIndex / 4]));
			SPRITE_DATA_TEX0_DMATAG(addr, u32VertexCount / 4, &SpriteMgr.pTEX0List[u32StartIndex / 4]);	// use macro to ensure same as init in SpriteMgr__BuildDataDMAPacket
			addr += 4;	// skip STCYCL
			SPRITE_DATA_TEX0_UNPACK(addr, u32VertexCount / 4);	// use macro to ensure same as init in SpriteMgr__BuildDataDMAPacket

			/* then uv data */

			ASSERT(IS_DMA_ALIGNED(&SpriteMgr.pUVList[u32StartIndex]));
			SPRITE_DATA_UV_DMATAG(addr, u32VertexCount, &SpriteMgr.pUVList[u32StartIndex]);	// use macro to ensure same as init in SpriteMgr__BuildDataDMAPacket
			addr += 4;	// skip STCYCL
			SPRITE_DATA_UV_UNPACK(addr, u32VertexCount);	// use macro to ensure same as init in SpriteMgr__BuildDataDMAPacket

			/* then color data */

			ASSERT(IS_DMA_ALIGNED(&SpriteMgr.pColorList[u32StartIndex]));
			SPRITE_DATA_COLOR_DMATAG(addr, u32VertexCount, &SpriteMgr.pColorList[u32StartIndex]);	// use macro to ensure same as init in SpriteMgr__BuildDataDMAPacket
			addr += 4;	// skip STCYCL
			SPRITE_DATA_COLOR_UNPACK(addr, u32VertexCount);	// use macro to ensure same as init in SpriteMgr__BuildDataDMAPacket
	
			/* then vertex data */

			ASSERT(IS_DMA_ALIGNED(&SpriteMgr.pVertexList[u32StartIndex]));
			SPRITE_DATA_VERTEX_DMATAG(addr, u32VertexCount, &SpriteMgr.pVertexList[u32StartIndex]);	// use macro to ensure same as init in SpriteMgr__BuildDataDMAPacket
			addr += 4;	// skip STCYCL
			SPRITE_DATA_VERTEX_UNPACK(addr, u32VertexCount);	// use macro to ensure same as init in SpriteMgr__BuildDataDMAPacket

			/* and send it */

			COPY_TO_MASTER_LIST(u8SpriteDataPacket, SPRITE_DATA_PACKET_QWC);
//			printf("%d %d\n", MasterList::GetFrameNumber(), u32StartIndex);
#endif //PS2
		}
	}

	/* toggle double buffers */
	SpriteMgr.u32Toggle ^= 1;

	SpriteMgr.textureArray.Unlock();


#ifdef PS2
	// restore dither matrix
	GSState__Set(SCE_GS_DIMX, GSState.DIMX);
#endif

	/* we don't know color state after all this is through */

	RenderState__Force( RS_BASECOLOR );

	/* restore matrices */

	MatrixStack__Mode( MS_PROJECTION );
   MatrixStack__Pop();

	MatrixStack__Mode( MS_MODELVIEW );
	MatrixStack__Pop();

#ifdef _DEBUG_DAMN_SORT_ORDER
	u16SortPriority = 0;
	next_sprite_is_not_sorted = 0;
	this_sprite_is_not_sorted = 0;
	ts_Sprite* pPrevSprite[2];
	pPrevSprite[0] = 0;
	pPrevSprite[1] = 0;
	for( i = 0; i < n; i++ )
	{
		pSprite = SpriteOrder[i];
		if( pSprite->u16SortPriority < u16SortPriority )
		{
			next_sprite_is_not_sorted = pPrevSprite[1];
			this_sprite_is_not_sorted = pPrevSprite[0];
			break;
		}
		u16SortPriority = pSprite->u16SortPriority;
		pPrevSprite[1] = pPrevSprite[0];
		pPrevSprite[0] = pSprite;
	}
#endif // _DEBUG_DAMN_SORT_ORDER

	return;
}

/* set the texture set for registering sprites */

bool SpriteMgr__SetTextureDbl(t_Handle Handle)	// Handle is .dbl file handle in which to find texture set
{
	// Return early if the texture handle doesn't change. -Dan
	if (SpriteMgr.hTextureDbl == Handle)
		return true;

	/* look for texture set-- may be normal or merged */

	SpriteMgr.pTextureDatabase  = (TextureDatabase *) dblGetChunkData(DB_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, Handle);
	if (SpriteMgr.pTextureDatabase == NULL)
		SpriteMgr.pTextureDatabase = (TextureDatabase *) dblGetChunkData(DB_MERGED_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, Handle);
	ASSERT_PTR(SpriteMgr.pTextureDatabase);

	// HACK! The sprite manager creates a texture set that consists of
	// all textures in the database.. this would be bad if we had a
	// larger database format than the dbl.. but since the code above
	// loads a dbl just for this purpose, its safe for now.
//	SpriteMgr.textureArray.AddEntireDatabase(SpriteMgr.pTextureDatabase);
	SpriteMgr.textureArray.AddWhiteTexture();

	/* set it */

	SpriteMgr.hTextureDbl = Handle;
	
	return(SpriteMgr.pTextureDatabase != NULL);
}

/* remove a texture set by handle */

void SpriteMgr__RemoveTextureDbl(t_Handle Handle)	// Handle is .dbl file handle in which to find texture set
{
	/* look for texture set-- may be normal or merged */

	TextureDatabase* pTextureDatabase  = (TextureDatabase *) dblGetChunkData(DB_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, Handle);
	if (pTextureDatabase == NULL)
		pTextureDatabase = (TextureDatabase *) dblGetChunkData(DB_MERGED_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, Handle);
	ASSERT_PTR(pTextureDatabase);
	SpriteMgr__CheckTextureDatabaseRemove(pTextureDatabase);
}

t_Handle SpriteMgr__CurrentTextureDbl( void )
{
	return SpriteMgr.hTextureDbl;
}

void SpriteMgr__CheckTextureDatabaseRemove(TextureDatabase *pTextureDatabase)
{
	/* check if sprite manager's texture set is going away. called from TextureMgr__RemoveTextureSet */

	if (pTextureDatabase == SpriteMgr.pTextureDatabase)
	{
		SpriteMgr.pTextureDatabase = NULL;
		SpriteMgr.hTextureDbl = INVALID_HANDLE;
	}
	// HACK! The sprite manager creates a texture set that consists of
	// all textures in the database.. this would be bad if we had a
	// larger database format than the dbl.. but since the code above
	// loads a dbl just for this purpose, its safe for now.
//	SpriteMgr.textureArray.RemoveEntireDatabase(pTextureDatabase);
	SpriteMgr.textureArray.RemoveWhiteTexture();
}

/* register a sprite. LOCAL_MEMORY sprites (that is, none of the BY_REFERENCE flags bits set) may be more efficient because
they can be compiled into a display list

flags:

VERTEX_BY_REFERENCE		sprite gets vertices from a user-controlled buffer instead of copying data to local record
COLOR_BY_REFERENCE		sprite gets colors from a user-controlled buffer instead of copying data to local record
UV_BY_REFERENCE			sprite gets uvs from a user-controlled buffer instead of copying data to local record
WHITE_SPRITE				uses a set of all white colors. pColors can be NULL
FULL_TEXTURE_SPRITE		uses a full-texture set of uv coords. pUVs can be NULL
SPRITE_ALL_ONE_COLOR		can specify that colors are all the same. careful if you use COLOR_BY_REFERENCE not to change to different colors
SPRITE_DISABLED			sprite will be processed but not displayed until call to SpriteMgr__EnableSprite

u32TextureIndex is an index into the textures in the current texture set, which is set by SpriteMgr__SetTextureSet

*/

t_Handle SpriteMgr__RegisterSprite(u32 u32TextureIndex, u16 u16Group, u16 u16SortPriority, u32 u32Flags, Vector3 *pVertices, Vector2 *pUVs, ts_bRGBA *pColors, u32 piece)
{
	/* sprite manager must have been initialized, and a texture set must have been chosen */

	ASSERT(SpriteMgr.pSpriteAlloc);	// if this hits, SpriteMgr__Init hasn't been called
	if( ( u32TextureIndex != TS_NO_TEXTURE) && !( u32Flags & DIRECT_BIND))
	{
		ASSERT_PTR(SpriteMgr.pTextureDatabase);	// if this hits, SpriteMgr__SetTextureSet hasn't been called (or has failed)
	}

	/* grab an instance record from free list */

	ts_Sprite *pSprite = SpriteMgr__AllocRecord();
	if (pSprite == NULL)
		return(INVALID_HANDLE);

	/* add to head of active list */

	DLinkList__AddLinkToHead(&SpriteMgr.ActiveList, pSprite);

	/* and off the texset id */

	if (u32TextureIndex != TS_NO_TEXTURE && !(u32Flags & DIRECT_BIND) )
		u32TextureIndex &= 0xffffff;

	/* check some flag settings */

	if ( (u32Flags & FULL_TEXTURE_SPRITE) || (u32TextureIndex == TS_NO_TEXTURE))
	{
		u32Flags &= ~UV_BY_REFERENCE;		// fully local sprites can be done via display lists, so help out
		pUVs = FullTextureUVs;
	}
	if (u32Flags & WHITE_SPRITE)
	{
		u32Flags &= ~COLOR_BY_REFERENCE;	// fully local sprites can be done via display lists, so help out
		u32Flags |= SPRITE_ALL_ONE_COLOR;
		pColors = WhiteColors;
	}

	/* default blend mode */
	pSprite->u8BlendMode = SPRITEBLEND_NORMAL;

	/* fill in record */
	pSprite->u32Piece = piece;

	pSprite->u16Group = u16Group;
	pSprite->u16SortPriority = u16SortPriority;
	if (u32Flags & VERTEX_BY_REFERENCE)
		pSprite->pVertices = pVertices;
	else
		memcpy(pSprite->Vertices, pVertices, 4 * sizeof(Vector3));

	if (u32TextureIndex == TS_NO_TEXTURE)
		pSprite->pTexture = NULL;
	else if (u32Flags & DIRECT_BIND)
		pSprite->pTexture = (Texture*) u32TextureIndex;	// pass texture directly
	else if (u32TextureIndex < SpriteMgr.pTextureDatabase->TextureCount())
		pSprite->pTexture = SpriteMgr.pTextureDatabase->GetTexture((u16)u32TextureIndex);
	else
		pSprite->pTexture = NULL;

//yuch-- what to do about non-textured sprites in joe's renderer? point off to white sprite?
#ifdef PS2
	if (pSprite->pTexture == NULL)
		pSprite->pTexture = TextureMgr::GetWhiteTexture();
#endif
	
	if (pSprite->pTexture != NULL)
		SpriteMgr.textureArray.AddTexture(pSprite->pTexture);
	

	if (u32Flags & UV_BY_REFERENCE)
		pSprite->pUVs = pUVs;
	else
		memcpy(pSprite->UVs, pUVs, 4 * sizeof(Vector2));

	if (u32Flags & COLOR_BY_REFERENCE)
		pSprite->pColors = pColors;
	else
	{
		memcpy(pSprite->Colors, pColors, 4 * sizeof(ts_bRGBA));

		/* if user didn't specify all-one-color, check it anyway since it improves rendering */

		if (((u32Flags & SPRITE_ALL_ONE_COLOR) == 0) &&
			 (memcmp(&pColors[SPRITE_INDEX0], &pColors[SPRITE_INDEX1], sizeof(ts_bRGBA)) == 0) &&
			 (memcmp(&pColors[SPRITE_INDEX0], &pColors[SPRITE_INDEX2], sizeof(ts_bRGBA)) == 0) &&
			 (memcmp(&pColors[SPRITE_INDEX0], &pColors[SPRITE_INDEX3], sizeof(ts_bRGBA)) == 0))
		{
			u32Flags |= SPRITE_ALL_ONE_COLOR;
		}
	}

	/* do flags last since they are updated by processing */

	pSprite->u32Flags = u32Flags;

	// make sure shade value is normal
	pSprite->u8Shade = BYTE_FULL_ALPHA;

	/* success */

	return(pSprite->Handle);
}

/* update the contents of a sprite record. returns FALSE if sprite has been deallocated (e.g. handle doesn't match).

pass u16Group == SPRITE_NO_UPDATE to leave current u16Group value alone, same for u16SortPriority

pass NULL for pVertices, pUVs, pColors to leave that data alone

flags:

WHITE_SPRITE				overrides whatever color settings sprite previously had. pColors can be NULL
FULL_TEXTURE_SPRITE		overrides whatever uv settings sprite previously had. pUVs can be NULL
SPRITE_ALL_ONE_COLOR		if pColors != NULL, can specify that colors are all the same

gotchas:

WHITE_SPRITE or FULL_TEXTURE_SPRITE will override COLOR_BY_REFERENCE and UV_BY_REFERENCE, respectively, but
otherwise there is no way to change a sprite from BY_REFERENCE to LOCAL_COPY. can be implemented if we need it.

there is no way currently to change a sprite from LOCAL_COPY to BY_REFERENCE

*/

bool SpriteMgr__UpdateSprite(t_Handle Handle, u16 u16Group, u16 u16SortPriority, u32 u32Flags, Vector3 *pVertices, Vector2 *pUVs, ts_bRGBA *pColors )
{
	ts_Sprite *pSprite;

	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	pSprite = SPRITE_MGR_GET_RECORD(Handle);
	if (pSprite->Handle != Handle)
		return(FALSE);

	/* check some flag settings */

	if (u32Flags & FULL_TEXTURE_SPRITE)
	{
		pSprite->u32Flags &= ~UV_BY_REFERENCE;		// fully-local sprites can be done via display lists, so help out
		pUVs = FullTextureUVs;
	}
	if (u32Flags & WHITE_SPRITE)
	{
		pSprite->u32Flags &= ~COLOR_BY_REFERENCE;	// fully-local sprites can be done via display lists, so help out
		u32Flags |= SPRITE_ALL_ONE_COLOR;
		pColors = WhiteColors;
	}

	/* group and priority */

	if (u16Group != SPRITE_NO_UPDATE)
		pSprite->u16Group = u16Group;
	if (u16SortPriority != SPRITE_NO_UPDATE)
		pSprite->u16SortPriority = u16SortPriority;

	/* what to update? */

	if (pVertices != NULL)
	{
		ASSERT((pSprite->u32Flags & VERTEX_BY_REFERENCE) == 0);	// only makes sense if data is local to sprite record, since by-reference updates automatically
		memcpy(pSprite->Vertices, pVertices, 4 * sizeof(Vector3));
	}
	if (pUVs != NULL)
	{
		ASSERT((pSprite->u32Flags & UV_BY_REFERENCE) == 0);		// only makes sense if data is local to sprite record, since by-reference updates automatically
		memcpy(pSprite->UVs, pUVs, 4 * sizeof(Vector2));

		/* update full-texture flag */

//yuch-- this is currently just a nicety since the FULL_TEXTURE_SPRITE flag is never checked. maybe it shouldn't even go
// into the flags?
		pSprite->u32Flags &= ~FULL_TEXTURE_SPRITE;
		pSprite->u32Flags |= (u32Flags & FULL_TEXTURE_SPRITE);
	}
	if (pColors != NULL)
	{
		ASSERT((pSprite->u32Flags & COLOR_BY_REFERENCE) == 0);	// only makes sense if data is local to sprite record, since by-reference updates automatically
		memcpy(pSprite->Colors, pColors, 4 * sizeof(ts_bRGBA));

		/* update white, all-one-color flags */

//yuch-- this is currently just a nicety since the WHITE_SPRITE flag is never checked. maybe it shouldn't even go
// into the flags?
		pSprite->u32Flags &= ~(WHITE_SPRITE | SPRITE_ALL_ONE_COLOR);
		pSprite->u32Flags |= (u32Flags & (WHITE_SPRITE | SPRITE_ALL_ONE_COLOR));

		/* if user didn't specify all-one-color, check it anyway since it improves rendering */

		if (((pSprite->u32Flags & SPRITE_ALL_ONE_COLOR) == 0) &&
		    (memcmp(&pColors[SPRITE_INDEX0], &pColors[SPRITE_INDEX1], sizeof(ts_bRGBA)) == 0) &&
			 (memcmp(&pColors[SPRITE_INDEX0], &pColors[SPRITE_INDEX2], sizeof(ts_bRGBA)) == 0) &&
			 (memcmp(&pColors[SPRITE_INDEX0], &pColors[SPRITE_INDEX3], sizeof(ts_bRGBA)) == 0))
		{
			pSprite->u32Flags |= SPRITE_ALL_ONE_COLOR;
		}
	}

	/* success */

	return(TRUE);
}

/* return pointers to a sprite's data-- can be internal data or data-by-reference. presumably, this routine will not
be called much for data-by-reference pointers since the caller should already have that information. pass in NULL
pointers if you don't care about some stuff */

bool SpriteMgr__GetDataPointers(t_Handle Handle, u32 *pu32Flags, Vector3 **ppVertices, Vector2 **ppUVs, ts_bRGBA **ppColors)
{
	ts_Sprite *pSprite;

	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	pSprite = SPRITE_MGR_GET_RECORD(Handle);
	if (pSprite->Handle != Handle)
		return(FALSE);

	/* fill in flags */

	if (pu32Flags)
		*pu32Flags = pSprite->u32Flags;

	/* fill in pointers */

	if (ppVertices)
	{
		if (pSprite->u32Flags & VERTEX_BY_REFERENCE)
			*ppVertices = pSprite->pVertices;
		else
			*ppVertices = pSprite->Vertices;	// sprite's internal data
	}
	if (ppUVs)
	{
		if (pSprite->u32Flags & UV_BY_REFERENCE)
			*ppUVs = pSprite->pUVs;
		else
			*ppUVs = pSprite->UVs;				// sprite's internal data
	}
	if (ppColors)
	{
		if (pSprite->u32Flags & COLOR_BY_REFERENCE)
			*ppColors = pSprite->pColors;
		else
			*ppColors = pSprite->Colors;		// sprite's internal data
	}

	return(TRUE);
}

Texture* SpriteMgr__GetTexture(t_Handle Handle)
{
	ts_Sprite *pSprite;

	ASSERT(Handle != INVALID_HANDLE);
	pSprite = SPRITE_MGR_GET_RECORD(Handle);
	if( pSprite->Handle != Handle )
		return 0;

	return pSprite->pTexture;
}

u32 SpriteMgr__GetPiece(t_Handle Handle)
{
	ts_Sprite *pSprite;

	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	pSprite = SPRITE_MGR_GET_RECORD(Handle);
	if (pSprite->Handle == Handle)
		return( pSprite->u32Piece);
	else
		return( 0xFFFFFFFF);
}

bool SpriteMgr__ChangeSortPriority(t_Handle Handle, u16 SortValue)
{
	ts_Sprite *pSprite;

	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	pSprite = SPRITE_MGR_GET_RECORD(Handle);
	if (pSprite->Handle != Handle)
		return(FALSE);

	pSprite->u16SortPriority = SortValue;

	return(TRUE);
}

u16 SpriteMgr__GetSortPriority(t_Handle Handle)
{
	ts_Sprite *pSprite;

	ASSERT(Handle != INVALID_HANDLE);
	pSprite = SPRITE_MGR_GET_RECORD(Handle);
	if( pSprite->Handle != Handle )
		return(FALSE);

	return pSprite->u16SortPriority;
}

bool SpriteMgr__ChangeFlag(t_Handle Handle, u32 u32Flags)
{
	ts_Sprite *pSprite;

	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	pSprite = SPRITE_MGR_GET_RECORD(Handle);
	if (pSprite->Handle != Handle)
		return(FALSE);

	pSprite->u32Flags = u32Flags;

	return(TRUE);
}


bool SpriteMgr__ChangeShadeValue(t_Handle Handle, u8 u8Shade)
{
	ts_Sprite *pSprite;

	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	pSprite = SPRITE_MGR_GET_RECORD(Handle);
	if (pSprite->Handle != Handle)
		return(FALSE);

	pSprite->u8Shade = u8Shade;

	return(TRUE);
}

u8 SpriteMgr__GetShadeValue(t_Handle Handle)
{
	ts_Sprite *pSprite;

	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	pSprite = SPRITE_MGR_GET_RECORD(Handle);
	if (pSprite->Handle != Handle)
		return(FALSE);

	return( pSprite->u8Shade);
}

/* enable/disable a sprite */

bool SpriteMgr__EnableSprite(t_Handle Handle)
{
	ts_Sprite *pSprite;

	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	pSprite = SPRITE_MGR_GET_RECORD(Handle);
	if (pSprite->Handle != Handle)
		return(FALSE);
	pSprite->u32Flags &= ~SPRITE_DISABLED;
	return(TRUE);
}
bool SpriteMgr__DisableSprite(t_Handle Handle)
{
	ts_Sprite *pSprite;

	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	pSprite = SPRITE_MGR_GET_RECORD(Handle);
	if (pSprite->Handle != Handle)
		return(FALSE);
	pSprite->u32Flags |= SPRITE_DISABLED;
	return(TRUE);
}

/* enable/disable a group of sprites */

void SpriteMgr__EnableSpriteGroup(u16 u16Group)
{
	ts_Sprite *pSprite;

	pSprite = (ts_Sprite *) SpriteMgr.ActiveList.pHead;
	while (pSprite)
	{
		if (pSprite->u16Group == u16Group)
			pSprite->u32Flags &= ~SPRITE_DISABLED;
		pSprite = pSprite->pNext;
	}
}
void SpriteMgr__DisableSpriteGroup(u16 u16Group)
{
	ts_Sprite *pSprite;

	pSprite = (ts_Sprite *) SpriteMgr.ActiveList.pHead;
	while (pSprite)
	{
		if (pSprite->u16Group == u16Group)
			pSprite->u32Flags |= SPRITE_DISABLED;
		pSprite = pSprite->pNext;
	}
}

/* remove a sprite. return value of FALSE does not necessarily mean an error, it means that sprite was already removed
(perhaps at end of lifetime) */

bool SpriteMgr__KillSprite(t_Handle Handle)
{
	ts_Sprite *pSprite;

	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	pSprite = SPRITE_MGR_GET_RECORD(Handle);
	if (pSprite->Handle != Handle)
		return(FALSE);

	/* clear assigned handle */

	pSprite->Handle = INVALID_HANDLE;

	if (pSprite->pTexture != NULL)
		SpriteMgr.textureArray.RemoveTexture(pSprite->pTexture);
	
	/* move sprite from active to free list */

	DLinkList__RemoveLink(&SpriteMgr.ActiveList, pSprite);
	DLinkList__AddLinkToHead(&SpriteMgr.FreeList, pSprite);
	return(TRUE);
}

/* removes all sprites in group */

void SpriteMgr__KillSpriteGroup(u16 u16Group)
{
	ts_Sprite *pSprite;
	ts_Sprite *pNextSprite;

	/* loop through active sprites */

	pNextSprite = (ts_Sprite *) SpriteMgr.ActiveList.pHead;
	while (pNextSprite)
	{
		/* record next sprite before possibly removing sprite from active list */

		pSprite = pNextSprite;
		pNextSprite = pSprite->pNext;

		/* remove if belongs to group */

		if (pSprite->u16Group == u16Group)
			SpriteMgr__KillSprite(pSprite->Handle);
	}
}

void SpriteMgr__KillSpriteCategory(u16 u16Group, u16 mask)
{
	ts_Sprite *pSprite;
	ts_Sprite *pNextSprite;

	/* loop through active sprites */
	u16Group &= mask;

	pNextSprite = (ts_Sprite *) SpriteMgr.ActiveList.pHead;
	while (pNextSprite)
	{
		/* record next sprite before possibly removing sprite from active list */

		pSprite = pNextSprite;
		pNextSprite = pSprite->pNext;

		/* remove if belongs to group */

		if ( ( pSprite->u16Group & mask) == u16Group)
			SpriteMgr__KillSprite(pSprite->Handle);
	}
}


/* calls a function for all sprites in group */

void SpriteMgr__AffectSpriteGroup(u16 u16Group, SPRITEAFFECTFN routine, void *data)
{
	ts_Sprite *pSprite;
	ts_Sprite *pNextSprite;

	/* loop through active sprites */

	pNextSprite = (ts_Sprite *) SpriteMgr.ActiveList.pHead;
	while (pNextSprite)
	{
		/* record next sprite before possibly removing sprite from active list */

		pSprite = pNextSprite;
		pNextSprite = pSprite->pNext;

		if (pSprite->u16Group == u16Group)
		{
			routine( pSprite->Handle, data);
		}
	}
}

/* calls a function for all sprites in group */

void SpriteMgr__AffectSpriteCategory(u16 u16Group, u16 mask, SPRITEAFFECTFN routine, void *data)
{
	ts_Sprite *pSprite;
	ts_Sprite *pNextSprite;

	/* loop through active sprites */
	u16Group &= mask;

	pNextSprite = (ts_Sprite *) SpriteMgr.ActiveList.pHead;
	while (pNextSprite)
	{
		/* record next sprite before possibly removing sprite from active list */

		pSprite = pNextSprite;
		pNextSprite = pSprite->pNext;

		if ( (pSprite->u16Group & mask) == u16Group)
		{
			routine( pSprite->Handle, data);
		}
	}
}


/* removes all active sprites */

void SpriteMgr__KillAllSprites(void)
{
	/* loop through active sprites */

	while (SpriteMgr.ActiveList.pHead)
	{
		/* kill it */

		SpriteMgr__KillSprite(((ts_Sprite *) SpriteMgr.ActiveList.pHead)->Handle);
	}
}

/* calls a function for all active sprites */

void SpriteMgr__AffectAllSprites( SPRITEKILLFN routine)
{
	ts_Sprite *pSprite;
	ts_Sprite *pNextSprite;

	/* loop through active sprites */

	pNextSprite = (ts_Sprite *) SpriteMgr.ActiveList.pHead;
	while (pNextSprite)
	{
		/* record next sprite before possibly removing sprite from active list */

		pSprite = pNextSprite;
		pNextSprite = pSprite->pNext;

		routine( pSprite->Handle);
	}
}

/* removes all disabled sprites */

void SpriteMgr__KillAllDisabledSprites(void)
{
	ts_Sprite *pSprite;
	ts_Sprite *pNextSprite;

	/* loop through active sprites */

	pNextSprite = (ts_Sprite *) SpriteMgr.ActiveList.pHead;
	while (pNextSprite)
	{
		/* record next sprite before possibly removing sprite from active list */

		pSprite = pNextSprite;
		pNextSprite = pSprite->pNext;

		/* remove if disabled */

		if (pSprite->u32Flags & SPRITE_DISABLED)
			SpriteMgr__KillSprite(pSprite->Handle);
	}
}

/* return size of texture being used by sprite */

#ifndef CDROM
void SpriteMgr__GetTextureSize(t_Handle Handle, u32 *pu32Width, u32 *pu32Height)
{
	ts_Sprite *pSprite;

	/* point to sprite */
	ASSERT(Handle != INVALID_HANDLE);
	pSprite = SPRITE_MGR_GET_RECORD(Handle);
	ASSERT_PTR(pSprite);

	if (pSprite->pTexture == NULL)
	{
		if (pu32Width)
			*pu32Width = 0;
		if (pu32Height)
			*pu32Height = 0;
	}
	else
	{
		Image* img = pSprite->pTexture->GetImage();
		ASSERT_PTR(img);
		if (pu32Width)
			*pu32Width = img->GetWidth();
		if (pu32Height)
			*pu32Height = img->GetHeight();
	}
}
#endif //CDROM

/* grab a record from free pool, assign handle, return handle */

ts_Sprite *SpriteMgr__AllocRecord(void)
{
	/* grab a record from free pool */

	ts_Sprite *pSprite = (ts_Sprite *) DLinkList__GetAndRemoveHead(&SpriteMgr.FreeList);
	ASSERT(pSprite);	// shouldn't run out of records. if so, increase handle count passed to SpriteMgr__Init
	if (pSprite == NULL)
		return(NULL);

	/* build handle */

	ASSERT(pSprite->Handle == INVALID_HANDLE);			// record should not have a handle assigned
	HANDLE_INDEX(pSprite->Handle) = pSprite - SpriteMgr.pSpriteAlloc;	// index into allocated array of records
	HANDLE_ID(pSprite->Handle) = SpriteMgr.u16ID++;
	if (pSprite->Handle == INVALID_HANDLE)
		HANDLE_ID(pSprite->Handle) = SpriteMgr.u16ID++;	// do it again to avoid INVALID_HANDLE
	ASSERT(pSprite->Handle != INVALID_HANDLE);			// record should have a valid handle

	/* success */

	return(pSprite);
}

/* sort the active list by the u16SortPriority field. higher priority sprites are sorted to the end of the list so that
they render last. since z-buffering is off, that means they will show up on top */

void SpriteMgr__SortActiveList(void)
{
	ts_Sprite *pSprite;
	ts_Sprite *pNextSprite;

	/* let each record bubble up against the records above it. this requires a doubly-linked list. disabled sprites just */
	/* get moved to head of list where they can't do any harm */

	pNextSprite = (ts_Sprite *) SpriteMgr.ActiveList.pHead;
	while (pNextSprite)
	{
		/* have to pre-load next record since record being sorted may move */

		pSprite = pNextSprite;
		pNextSprite = pSprite->pNext;

		/* enabled or disabled? */

		if (pSprite->u32Flags & SPRITE_DISABLED)
		{
			/* move disabled sprites to head of list where they are safely out the sort order */

			DLinkList__RemoveLink(&SpriteMgr.ActiveList, pSprite);
			DLinkList__AddLinkToHead(&SpriteMgr.ActiveList, pSprite);
		}
		else
		{
			/* bubble up enabled sprites. stop when we reach head of list or disabled sprites at head of list */

			while ((pSprite->pPrev) && ((pSprite->pPrev->u32Flags & SPRITE_DISABLED) == 0))
			{
	         if (pSprite->u16SortPriority < pSprite->pPrev->u16SortPriority)
					DLinkList__SwapLinks(&SpriteMgr.ActiveList, pSprite->pPrev, pSprite);
				else
					break;	// record is in place
			}
		}
	}

//yuch-- test sort
#if 0
	{
	bool bSortNeeded = FALSE;
	u32 u32LastSortPriority = 0;
	pSprite = (ts_Sprite *) SpriteMgr.ActiveList.pHead;
	while (pSprite)
	{
		if ((pSprite->u32Flags & SPRITE_DISABLED) == 0)
		{
			/* out of sort order? */

			if (pSprite->u16SortPriority < u32LastSortPriority)
				bSortNeeded = TRUE;
			u32LastSortPriority = pSprite->u16SortPriority;
		}
		pSprite = pSprite->pNext;
	}
	ASSERT(bSortNeeded == FALSE);
	}
#endif

}

#ifndef CONSUMER_BUILD
/* render the profiler data as timing bars across the screen, using the sprite manager */

#define BAR_START_X			0.13f
#ifdef _XBOX
#define BAR_START_Y			0.06f	//there's more overscan on an XBox
#else
#define BAR_START_Y			0.04f
#endif
#define BAR_DEFAULT_WIDTH	0.01f
#define BAR_MAX_WIDTH		0.8f
#define BAR_HEIGHT			0.005f
#define BAR_Y_SPACING		(BAR_HEIGHT * 1.5f)
void SpriteMgr__UpdateProfileBars(void)
{
	int i;
	int nBars;
	float fY;
	float fPercent;
	Vector3 Vertices[4];
	Vector3 *pVertices;
	ts_bRGBA Color;

	/* has number of slots changed? */

	nBars = Profiler__GetActiveEntryCount();
	if (nBars != SpriteMgr.nLastBars)
	{
		/* kill existing sprites, if any */

		if (SpriteMgr.pBarHandleList)
		{
			for (i = 0; i < SpriteMgr.nLastBars; i++)
				SpriteMgr__KillSprite(SpriteMgr.pBarHandleList[i]);
			memFree(SpriteMgr.pBarHandleList);
		}
		SpriteMgr.nLastBars = nBars;

		/* set up a new set of sprites */

		SpriteMgr.pBarHandleList = (t_Handle *) memAlloc(nBars * sizeof(t_Handle *));
		fY = BAR_START_Y;
		Vertices[0].x(BAR_START_X);
		Vertices[3].x(BAR_START_X);
		Vertices[1].x(BAR_START_X + BAR_DEFAULT_WIDTH);
		Vertices[2].x(BAR_START_X + BAR_DEFAULT_WIDTH);
		Vertices[0].z(0.0f);
		Vertices[1].z(0.0f);
		Vertices[2].z(0.0f);
		Vertices[3].z(0.0f);
		for (i = 0; i < nBars; i++)
		{
			Vertices[0].y(fY);
			Vertices[1].y(fY);
			Vertices[2].y(fY + BAR_HEIGHT);
			Vertices[3].y(fY + BAR_HEIGHT);
			Color = *Profiler__GetBarColor(i);
			SpriteMgr.pBarHandleList[i] = SpriteMgr__RegisterSprite(TS_NO_TEXTURE, SPRITE_DEFAULT_GROUP, SPRITE_DEFAULT_SORT, SPRITE_ALL_ONE_COLOR, Vertices, NULL, &Color);
			fY += BAR_HEIGHT + BAR_Y_SPACING;
		}
	}

	/* update widths based on current profile data */

	for (i = 0; i < nBars; i++)
	{
		fPercent = Profiler__GetBarFraction(i);
		if (fPercent < 0.005f)
			fPercent = 0.005f;
		if (fPercent > 1.0f)
			fPercent = 1.0f;
		SpriteMgr__GetDataPointers(SpriteMgr.pBarHandleList[i], NULL, &pVertices, NULL, NULL);
		ASSERT(pVertices);
		pVertices[1].x(BAR_START_X + (fPercent * BAR_MAX_WIDTH));
		pVertices[2].x(BAR_START_X + (fPercent * BAR_MAX_WIDTH));
	}
}

/* reset profile bars */

void SpriteMgr__ResetProfileBars(void)
{
	if (SpriteMgr.pBarHandleList)
	{
		memFree(SpriteMgr.pBarHandleList);
		SpriteMgr.pBarHandleList = NULL;
	}
	SpriteMgr.nLastBars = 0;
}

#endif //CONSUMER_BUILD

#ifndef CONSUMER_BUILD
/* support for a row of bars across the bottom of the screen that can be used for various purposes-- requested by bryant as
a way of doing strip-charting */

void SpriteMgr__ActivatePlotBars()
{
	/* if we have <n> bars and <n + 1> gaps (including blank spaces to left and right), and each gap is <GAP> width */
	/* of a bar (e.g. <GAP> = 0.5, gap is half the width of a bar), then the width of each bar can be calculated this way:

			n * width + (n + 1) * (width * GAP) = 1.0

		or:

			width * (n + GAP * n + GAP) = 1.0

		or:

			width = 1.0 / (n + GAP * n + GAP) */

	float fBarWidth = 1.0f / (MAX_PLOT_BARS + (PLOT_BAR_GAP_MULT * MAX_PLOT_BARS) + PLOT_BAR_GAP_MULT);
	float fGapWidth = fBarWidth * PLOT_BAR_GAP_MULT;
	float fBarX = fGapWidth;
	ts_bRGBA Color = {255, 255, 255, BYTE_FULL_ALPHA / 3};
	for (int i = 0; i < MAX_PLOT_BARS; i++, fBarX += fBarWidth + fGapWidth)
	{
		Vector3 Vertices[4];
		Vertices[0].x(fBarX);
		Vertices[3].x(fBarX);
		Vertices[1].x(fBarX + fBarWidth);
		Vertices[2].x(fBarX + fBarWidth);
		Vertices[0].y(PLOT_BAR_BOTTOM);
		Vertices[1].y(PLOT_BAR_BOTTOM);
		Vertices[2].y(PLOT_BAR_BOTTOM);
		Vertices[3].y(PLOT_BAR_BOTTOM);
		Vertices[0].z(0.0f);
		Vertices[1].z(0.0f);
		Vertices[2].z(0.0f);
		Vertices[3].z(0.0f);
		SpriteMgr.PlotBarHandle[i] = SpriteMgr__RegisterSprite(TS_NO_TEXTURE, SPRITE_DEFAULT_GROUP, SPRITE_DEFAULT_SORT, SPRITE_ALL_ONE_COLOR | SPRITE_DISABLED, Vertices, NULL, &Color);
	}

	/* plot bars are now active */

	SpriteMgr.bPlotBarsActivated = true;
}

/* update a particular bar */

void SpriteMgr__SetPlotBar(int nIndex, float fScale)
{
	if ((nIndex < 0) || (nIndex >= MAX_PLOT_BARS))
	{
		ASSERT(false);	// bad index!
		return;
	}

	/* need to activate plot bars? */

	if (SpriteMgr.bPlotBarsActivated == false)
		SpriteMgr__ActivatePlotBars();

	/* get pointers to data to update */

	u32 u32Flags;
	Vector3 *pVertices;
	SpriteMgr__GetDataPointers(SpriteMgr.PlotBarHandle[nIndex], &u32Flags, &pVertices, NULL, NULL);

	/* may just turn it off */

	if (fScale <= 0.0f)
	{
		u32Flags |= SPRITE_DISABLED;
		SpriteMgr__ChangeFlag(SpriteMgr.PlotBarHandle[nIndex], u32Flags);
		return;
	}

	/* set this bar */

	pVertices[1].y(PLOT_BAR_BOTTOM - (PLOT_BAR_HEIGHT * fScale));
	pVertices[0].y(PLOT_BAR_BOTTOM - (PLOT_BAR_HEIGHT * fScale));
 	u32Flags &= ~SPRITE_DISABLED;
	SpriteMgr__ChangeFlag(SpriteMgr.PlotBarHandle[nIndex], u32Flags);
}

/* update bar 0, push all values down one bar-- easier way to do a history graph */

void SpriteMgr__SetPlotBar(float fScale)
{
	/* need to activate plot bars? */

	if (SpriteMgr.bPlotBarsActivated == false)
		SpriteMgr__ActivatePlotBars();

	/* set all downstream bars */

	for (int i = MAX_PLOT_BARS - 1; i > 0; i--)
	{
		/* get pointers to previous bar's data */

		u32 u32Flags;
		Vector3 *pVertices;
		SpriteMgr__GetDataPointers(SpriteMgr.PlotBarHandle[i - 1], &u32Flags, &pVertices, NULL, NULL);

		/* and transfer data to next bar */

		float fOldScale = 0.0f;
		if ((u32Flags & SPRITE_DISABLED) == 0)
			fOldScale = (pVertices[1].y() - PLOT_BAR_BOTTOM) / -PLOT_BAR_HEIGHT;
		SpriteMgr__SetPlotBar(i, fOldScale);
	}

	/* set bar 0 to new value */

	SpriteMgr__SetPlotBar(0, fScale);
}

/* get bar count */

int SpriteMgr__GetPlotBarCount()
{
	return(MAX_PLOT_BARS);
}
#endif //CONSUMER_BUILD

#ifdef PS2
/* build packet for sending down initial sprite setup data for vu1 sprite renderer */

void SpriteMgr__BuildInitDMAPacket(void)
{
	u32 qwc = 0;
	u_char* addr = (u_char*) u8SpriteInitPacket;

/**************** note-- if this changes, update SPRITE_INIT_PACKET_QWC in SpriteMgr.h *********************/

	/* wait for previous microprogram */

	MasterList::Append(addr, MasterList::createDMATAG());
	MasterList::Append(addr, createFLUSH()); /* wait for previous microprogram */
	MasterList::Append(addr, createNOP());
	qwc++;

	/* send down vu1 rendering code */

	u_char* u32CodeStart = (u_char*) &SpriteVU1CodeDMA + (3 * BYTES_PER_QUAD_WORD);	// hack! skip over dma/vif commands generated by assembler
	u32 u32CodeEnd = (u32) &SpritesEnd;
	u32 u32CodeQWC = (u32CodeEnd - (u32)u32CodeStart) / BYTES_PER_QUAD_WORD;
	MasterList::Append(addr, MasterList::createDMATAG(u32CodeQWC, TAGID_REF, u32CodeStart));
	MasterList::Append(addr, createSTCYCL(4, 4));
	if (u32CodeQWC >= 128)
		u32CodeQWC = 0;	// if >= than 256 instructions in code, send 0 in MPG vif code and it uses qwc from dmatag
	MasterList::Append(addr, createMPG(u32CodeQWC * 2, 0));	// num, vuaddr -- each vu instruction is 64 bits, or half a quad word
	qwc++;

	/* we want to send down some matrices and a giftag for the output buffer */

	MasterList::Append(addr, MasterList::createDMATAG(1 + SPRITE_INIT_DATA_DMA_QWC));	// "1 +" for extra vif codes
	MasterList::Append(addr, createNOP());
	MasterList::Append(addr, createNOP());
	qwc++;

	/* extra vif codes for matrix download */

	MasterList::Append(addr, createBASE(SPRITE_VIF1_BASE_QWC));
	MasterList::Append(addr, createOFFSET(SPRITE_DOUBLE_BUFFER_SIZE));
	MasterList::Append(addr, createSTCYCL(1, 1));
	MasterList::Append(addr, SCE_VIF1_SET_UNPACK(0, SPRITE_INIT_DATA_DMA_QWC, V4_32, 0));	// unpack to start of vu ram
	qwc++;

	/************ from here to "end sprite init data" should be SPRITE_INIT_DATA_DMA_QWC quad words *********/

	/* leave space for matrix (sprite to screen) */

	ASSERT(qwc == SPRITE_INIT_MATRIX_QWC);
	addr += 4 * BYTES_PER_QUAD_WORD;
	qwc += 4;

	/* add gs register update giftag */

#define SPRITE_GS_REGISTER_COUNT	0	// currently nothing
	MasterList::Append(
		addr,
		createGIFTAG(
			1,										// nloop
			SPRITE_GS_REGISTER_COUNT,		// nreg-- register count-- 0 means 16
			0xeeeeeeeeeeeeeeee));			// registers (e=A+D)
	qwc++;
	addr += SPRITE_GS_REGISTER_COUNT * BYTES_PER_QUAD_WORD;	// leave room for register values
	qwc += SPRITE_GS_REGISTER_COUNT;

	/* add a "nop" giftag for gs sync control-- see notes in Sprites.vsm near bottom */

	MasterList::Append(
		addr,
		createGIFTAG(
			0,										// nloop
			0,										// nreg-- register count-- 0 means 16
			0xeeeeeeeeeeeeeeee));			// registers (e=A+D)
	qwc++;

	/******** end sprite init data ****************************************************/

	/* next we send some gs registers and start program execution */

#define SPRITE_GS_INIT_REGISTER_COUNT	2	// currently tex1 and clamp
	MasterList::Append(addr, MasterList::createDMATAG(1 + SPRITE_GS_INIT_REGISTER_COUNT));	// "1 +" is for the giftag
	MasterList::Append(addr, createMSCAL(0));	// start at first instruction, no irq
	MasterList::Append(addr, createDIRECT(1 + SPRITE_GS_INIT_REGISTER_COUNT));	// "1 +" is for the giftag
	qwc++;

	/* now direct-to-gif data-- a giftag and 2 registers to set texture clamp/filter defaults */

	MasterList::Append(
		addr,
		createGIFTAG(
			1,											// nloop
			SPRITE_GS_INIT_REGISTER_COUNT,	// nreg-- register count-- 0 means 16
			0xeeeeeeeeeeeeeeee));					// registers-- A + D
	qwc++;
	MasterList::Append(
		addr,
		createAD_TEX1_1(
			0,			// lcm-- LOD calculation method
			0,			// mxl-- maximum mip level
			1,			// mmag-- expand filter (0 = nearest, 1 = linear)
			1,			// mmin-- reduce filter (0 = nearest, 1 = linear, 2..5 are mipmap)
			1,			// mtba-- mipmap base addr
			0,			// L
			0)			// K
		);
	qwc++;
	MasterList::Append(
		addr,
		createAD_CLAMP_1(
			1,			// wms-- horizontal wrap mode
			1,			// wmt-- vertical wrap mode
			0,			// minu-- horizontal clamp parameter 
			0,			// maxu-- horizontal clamp parameter 
			0,			// minv-- vertical clamp parameter 
			0)			// maxv-- vertical clamp parameter 
			);
	qwc++;

	/* done. check that right amount of scrathpad was allocated */

	ASSERT(qwc == SPRITE_INIT_PACKET_QWC);
}

/* build packet for sending down sprite data to be rendered. lots of stuff needs to be filled in before this
can be sent */

void SpriteMgr__BuildDataDMAPacket()
{
	u32 qwc = 0;
	u_char* addr = (u_char*) u8SpriteDataPacket;

/**************** note-- if this changes, update SPRITE_DATA_PACKET_QWC in d_Masterlist.h *********************/

	/* first the gs register values and control word */

	MasterList::Append(addr, MasterList::createDMATAG(SPRITE_DATA_HEADER_QWC));
	MasterList::Append(addr, createSTCYCL(1, 1));
	MasterList::Append4(addr, SCE_VIF1_SET_UNPACK(0 | 0x8000, SPRITE_DATA_HEADER_QWC, V4_32, 0));	// 0x8000 = use xtop register for double buffering
	qwc++;

	/************ from here to "end sprite header data" should be SPRITE_DATA_HEADER_QWC quad words *********/

	/* if we need to add gs registers to be sent at start of sprite group, they go here */

	/* control word */

	MasterList::Append4(addr, 0 /*u32VertexCount*/);	// x-component
	MasterList::Append4(addr, 0);								// unused y-component
	MasterList::Append4(addr, 0);								// unused z-component
	MasterList::Append4(addr, 0);								// unused w-component
	qwc++;

	/* giftag to send down all vertices as a tri-strip */

	SPRITE_DATA_GIFTAG(addr, 0);				// use macro to ensure same as update in SpriteMgr__Render
	MasterList::Append8(addr, 0x512f512f512e5126);		// registers (reading from lsb)-- tex0, st, rgbaq, xyz2, a+d (alpha1), st, rgbaq, xyz2, nop, st, rgbaq, xyz2, nop, st, rgbaq, xyz2
	qwc++;

	/******** end sprite header data ****************************************************/

	/* then tex0 data */

	SPRITE_DATA_TEX0_DMATAG(addr, 0, 0);			// use macro to ensure same as update in SpriteMgr__Render
	MasterList::Append(addr, createSTCYCL(1, 16)); // write one word, skip 15
	SPRITE_DATA_TEX0_UNPACK(addr, 0);				// use macro to ensure same as update in SpriteMgr__Render
	qwc++;

	/* then uv data */

	SPRITE_DATA_UV_DMATAG(addr, 0, 0);				// use macro to ensure same as update in SpriteMgr__Render
	MasterList::Append(addr, createSTCYCL(1, 4));// write one word, skip 3
	SPRITE_DATA_UV_UNPACK(addr, 0);					// use macro to ensure same as update in SpriteMgr__Render
	qwc++;

	/* then color data-- each 4 colors make up a quad word */

	SPRITE_DATA_COLOR_DMATAG(addr, 0, 0);			// use macro to ensure same as update in SpriteMgr__Render
	MasterList::Append(addr, createSTCYCL(1, 4));// write one word, skip 3
	SPRITE_DATA_COLOR_UNPACK(addr, 0);				// use macro to ensure same as update in SpriteMgr__Render
	qwc++;

	/* then vertex data */

	SPRITE_DATA_VERTEX_DMATAG(addr, 0, 0);			// use macro to ensure same as update in SpriteMgr__Render
	MasterList::Append(addr, createSTCYCL(1, 4));// write one word, skip 3
	SPRITE_DATA_VERTEX_UNPACK(addr, 0);				// use macro to ensure same as update in SpriteMgr__Render
	qwc++;

	/* then go */

	MasterList::Append(addr, MasterList::createDMATAG());
	MasterList::Append(addr, createMSCNT());
	MasterList::Append(addr, createSTCYCL(4, 4)); // restore to normal...
	qwc++;

	/* done. check that right amount of scratchpad was allocated */

	ASSERT(qwc == SPRITE_DATA_PACKET_QWC);
}
#endif //PS2

/* JMH - these are some routines used for the new sprite chunk format,
         where pieces are subsets of textures, and piece headers are 
			stored in the .dbl file */
u16 SpriteMgr__GetTexIndex( int iPieceIndex)
{
	return SpriteMgr.pTextureDatabase->GetPiece(iPieceIndex)->u16TexIndex;
}

s16 SpriteMgr__GetTexWidth( int iPieceIndex)
{
	int iTexIndex = SpriteMgr__GetTexIndex( iPieceIndex);
	Texture* pTexture = SpriteMgr.pTextureDatabase->GetTexture(iTexIndex);
	ASSERT_PTR(pTexture);
	Image* pImage = pTexture->GetImage();
	ASSERT_PTR(pImage);
	return pImage->GetWidth();
}

inline s16 SpriteMgr__GetTexHeight( int iPieceIndex)
{
	int iTexIndex = SpriteMgr__GetTexIndex( iPieceIndex);
	Texture* pTexture = SpriteMgr.pTextureDatabase->GetTexture(iTexIndex);
	ASSERT_PTR(pTexture);
	Image* pImage = pTexture->GetImage();
	ASSERT_PTR(pImage);
	return pImage->GetHeight();
}

s16 SpriteMgr__GetWidth( int iPieceIndex)
{
	ts_PieceRecord* pPieceRecord = SpriteMgr.pTextureDatabase->GetPiece(iPieceIndex);
	ASSERT_PTR(pPieceRecord);
	return( pPieceRecord->s16Width);
}

s16 SpriteMgr__GetHeight( int iPieceIndex)
{
	ts_PieceRecord* pPieceRecord = SpriteMgr.pTextureDatabase->GetPiece(iPieceIndex);
	ASSERT_PTR(pPieceRecord);
	return( pPieceRecord->s16Height);
}

inline s16 SpriteMgr__GetXOffset( int iPieceIndex)
{												
	ts_PieceRecord* pPieceRecord = SpriteMgr.pTextureDatabase->GetPiece(iPieceIndex);
	ASSERT_PTR(pPieceRecord);
	return( pPieceRecord->s16OffX);
}

inline s16 SpriteMgr__GetYOffset( int iPieceIndex)
{
	ts_PieceRecord* pPieceRecord = SpriteMgr.pTextureDatabase->GetPiece(iPieceIndex);
	ASSERT_PTR(pPieceRecord);
	return( pPieceRecord->s16OffY);
}

inline u16 SpriteMgr__GetPieces( int iPieceIndex)
{
	ts_PieceRecord* pPieceRecord = SpriteMgr.pTextureDatabase->GetPiece(iPieceIndex);
	ASSERT_PTR(pPieceRecord);
	return( pPieceRecord->u16Pieces);
}

s16 SpriteMgr__GetHotSpotX( int iPieceIndex)
{
	ts_PieceRecord* pPieceRecord = SpriteMgr.pTextureDatabase->GetPiece(iPieceIndex);
	ASSERT_PTR(pPieceRecord);
	return( pPieceRecord->s16HotSpotX);
}

s16 SpriteMgr__GetHotSpotY( int iPieceIndex)
{
	ts_PieceRecord* pPieceRecord = SpriteMgr.pTextureDatabase->GetPiece(iPieceIndex);
	ASSERT_PTR(pPieceRecord);
	return( pPieceRecord->s16HotSpotY);
}

void SpriteMgr__ComputeUV_XYWH( Vector2 *pUVs, float x, float y, float pw, float ph, float tw, float th)
{
	pUVs[SPRITE_INDEX0].x( x / tw);
	pUVs[SPRITE_INDEX0].y( y / th);
	pUVs[SPRITE_INDEX1].x( ( x + pw) / tw);
	pUVs[SPRITE_INDEX1].y( y / th);
	pUVs[SPRITE_INDEX2].x( ( x + pw) / tw);
	pUVs[SPRITE_INDEX2].y( ( y + ph) / th);
	pUVs[SPRITE_INDEX3].x( x / tw);
	pUVs[SPRITE_INDEX3].y( ( y + ph) / th);
}

void SpriteMgr__ComputeUVs( Vector2 *pUVs, int iPieceIndex)
{
	float								x,y,tw,th,pw,ph;

	x = (float) SpriteMgr__GetXOffset( iPieceIndex);
	y = (float) SpriteMgr__GetYOffset( iPieceIndex);
	pw = (float) SpriteMgr__GetWidth( iPieceIndex);
	ph = (float) SpriteMgr__GetHeight( iPieceIndex);
	tw = (float) SpriteMgr__GetTexWidth( iPieceIndex);
	th = (float) SpriteMgr__GetTexHeight( iPieceIndex);

	SpriteMgr__ComputeUV_XYWH( pUVs, x, y, pw, ph, tw, th);
}

#define SCREEN_SIZEX				512
#define SCREEN_SIZEY				448

void SpriteMgr__ComputeVerticesWH( Vector3 *pVert, float left, float top, float w, float h)
{
	pVert->Set(left / SCREEN_SIZEX,
				  top / SCREEN_SIZEY,
				  0);
	pVert++;

	pVert->Set(( left + w) / SCREEN_SIZEX,
				  top / SCREEN_SIZEY,
				  0);
	pVert++;

	pVert->Set(( left + w) / SCREEN_SIZEX,
				  ( top + h) / SCREEN_SIZEY,
				  0);
	pVert++;

	pVert->Set(left / SCREEN_SIZEX,
				  ( top + h) / SCREEN_SIZEY,
				  0);
	pVert++;
}

void SpriteMgr__ComputeVertices( Vector3 *pVert, int iPieceIndex, float left, float top, float scalex, float scaley)
{
	float			w, h;

	w = SpriteMgr__GetWidth( iPieceIndex) * scalex;
	h = SpriteMgr__GetHeight( iPieceIndex) * scaley;

	SpriteMgr__ComputeVerticesWH( pVert, left, top, w, h);
}

t_Handle SpriteMgr__AddSprite( t_Handle hTextureDbl, u32 piece, float x, float y, float scalex, float scaley, u16 sid, u16 priority, u32 color, u8 alpha)
{
	Vector3					Vert[4];
	Vector2					UVs[4];
	ts_bRGBA					Colors[4];
	t_Handle					hHandle;
	int							i, j;
	float						hx, hy;

	SpriteMgr__SetTextureDbl( hTextureDbl);

	i = SpriteMgr__GetPieces( piece);
	do
	{
		for( j = 0; j < 4; j++)
		{
			Colors[j].R = (u8) (( color & 0xff0000) >> 16);
			Colors[j].G = (u8) (( color & 0xff00) >> 8);
			Colors[j].B = (u8) ( color & 0xff);
			Colors[j].A = alpha;
		}

		hx = (float) ( SpriteMgr__GetHotSpotX( piece) * scalex);
		hy = (float) ( SpriteMgr__GetHotSpotY( piece) * scaley);

		SpriteMgr__ComputeVertices( Vert, piece, x + hx, y + hy, scalex, scaley);
		SpriteMgr__ComputeUVs( UVs, piece);

		hHandle = SpriteMgr__RegisterSprite( SpriteMgr__GetTexIndex( piece), sid, priority, SHADE_SPRITE, Vert, UVs, Colors, piece);

		i--;
		piece++;
	} while ( i >= 0);

	return( hHandle);
}

t_Handle SpriteMgr__AddBox( float x, float y, float w, float h, u16 sid, u16 priority, u32 color, u8 alpha)
{
	Vector3					Vert[4];
	ts_bRGBA					Colors[4];
	t_Handle					hHandle;
	int						j;
 
	for( j = 0; j < 4; j++)
	{
		Colors[j].R = (u8) (( color & 0xff0000) >> 16);
		Colors[j].G = (u8) (( color & 0xff00) >> 8);
		Colors[j].B = (u8) ( color & 0xff);
		Colors[j].A = alpha;
	}

	SpriteMgr__ComputeVerticesWH( Vert, x, y, w, h);

	hHandle = SpriteMgr__RegisterSprite( TS_NO_TEXTURE, sid, priority, SHADE_SPRITE, Vert, NULL, Colors);

	return( hHandle);
}

void SpriteMgr__SetBlendMode( t_Handle Handle, u8 u8BlendMode )
{
	ts_Sprite *pSprite;

	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	pSprite = SPRITE_MGR_GET_RECORD(Handle);
	if (pSprite->Handle == Handle)
		pSprite->u8BlendMode = u8BlendMode;

	return;
}

u8 SpriteMgr__GetBlendMode( t_Handle Handle )
{
	ts_Sprite *pSprite;

	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	pSprite = SPRITE_MGR_GET_RECORD(Handle);
	if (pSprite->Handle != Handle)
		return SPRITEBLEND_NORMAL;

	return pSprite->u8BlendMode;
}

#ifndef CDROM
/* sometimes useful to put a sprite up on the screen with a known texture bind for debugging */

t_Handle SpriteMgr__ShowDirectBindSprite(Texture* texture)
{
	ASSERT_PTR(texture);
	Vector3 Vertices[4] = {
		Vector3(0.05f, 0.6f, 0.0f), 
		Vector3(0.40f, 0.6f, 0.0f),
		Vector3(0.40f, 0.95f, 0.0f),
		Vector3(0.05f, 0.95f, 0.0f)};

	return SpriteMgr__RegisterSprite((u32)texture, SPRITE_DEFAULT_GROUP, SPRITE_DEFAULT_SORT, DIRECT_BIND | FULL_TEXTURE_SPRITE | SPRITE_ALL_ONE_COLOR | WHITE_SPRITE, Vertices, NULL, NULL);
}

/* get a head count of active sprites (debug only) */

int SpriteMgr__GetSpritePopulation( void )
{
	ts_Sprite *pSprite = (ts_Sprite *) SpriteMgr.ActiveList.pHead;

	int i;
	for( i = 0; pSprite; i++)
		pSprite = pSprite->pNext;

	return i;
}

/* print out all sprites to a log file, so i can capture the raw data needed to recreate them */

void SpriteMgr__DumpSpriteData()
{
	static int nIndex = 0;
	char FileName[30];
	sprintf(FileName, "SpriteData%03d.log", nIndex++);
	FileStream *pLogFile = FileStream::NewAndOpen(FileName, FileStream::WRITE, true);
	if (pLogFile == NULL)
	{
		ASSERT(false);
		return;
	}

	/* vertices, 4 per sprite */

	pLogFile->Print("Vector3 LoadScreenSpriteVertices[7 * 4] = {\n");
	ts_Sprite *pSprite = (ts_Sprite *) SpriteMgr.ActiveList.pTail;	// start at end because first sprite defined is last in list
	int nSpriteIndex = 0;
	while (pSprite)
	{
		pLogFile->Print("\n\t/* sprite %d */\n\n", nSpriteIndex);
		for (int i = 0; i < 4; i++)
		{
			Vector3 *pVertices;
			if (pSprite->u32Flags & VERTEX_BY_REFERENCE)
				pVertices = pSprite->pVertices;
			else
				pVertices = pSprite->Vertices;	// sprite's internal data
			pLogFile->Print("\tVector3(%ff, %ff, %ff),\n",
								 pVertices[i].X(),
								 pVertices[i].Y(),
								 pVertices[i].Z());
		}
		pSprite = pSprite->pPrev;	// go backwards because first sprite defined is last in list
		nSpriteIndex++;
	}
	pLogFile->Print("};\n");

	/* uv's, 4 per sprite */

	pLogFile->Print("Vector2 LoadScreenSpriteUVs[7 * 4] = {\n");
	pSprite = (ts_Sprite *) SpriteMgr.ActiveList.pTail;	// start at end because first sprite defined is last in list
	nSpriteIndex = 0;
	while (pSprite)
	{
		pLogFile->Print("\n\t/* sprite %d */\n\n", nSpriteIndex);
		for (int i = 0; i < 4; i++)
		{
			Vector2 *pUVs;
			if (pSprite->u32Flags & UV_BY_REFERENCE)
				pUVs = pSprite->pUVs;
			else
				pUVs = pSprite->UVs;	// sprite's internal data
			pLogFile->Print("\tVector2(%ff, %ff),\n",
								 pUVs[i].X(),
								 pUVs[i].Y());
		}
		pSprite = pSprite->pPrev;	// go backwards because first sprite defined is last in list
		nSpriteIndex++;
	}
	pLogFile->Print("};\n");

	/* done */

	pLogFile->Close();
	delete pLogFile;
}
#endif //CDROM

void SpriteMgr__SetTextureByIndex( t_Handle Handle, u32 u32TextureIndex )
{
	ASSERT(Handle != INVALID_HANDLE);
	ts_Sprite* sprite = SPRITE_MGR_GET_RECORD(Handle);
	if (sprite)
	{
		ASSERT(sprite->Handle == Handle);

		Texture* tex = NULL;
		if (u32TextureIndex != TS_NO_TEXTURE)
			tex = SpriteMgr.pTextureDatabase->GetTexture((u16) u32TextureIndex);

		if (sprite->pTexture != tex)
		{
			if (sprite->pTexture)
				SpriteMgr.textureArray.RemoveTexture(sprite->pTexture);
			if (tex)
				SpriteMgr.textureArray.AddTexture(tex);
			sprite->pTexture = tex;
		}
	}
}

void SpriteMgr__SetZTest( bool enable )
{
	SpriteMgr.zTestEnabled = enable;
}
