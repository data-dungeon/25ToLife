//============================================================================
//=
//= ShadowMgr.h
//=
//============================================================================

#ifndef _SHADOW_MGR_H
#define _SHADOW_MGR_H

//============================================================================

#include "Layers/LayersStruct.h"	// low-level include
#include "Layers/SLinkList.h"		// for ts_SLinkList

//============================================================================

class		CInstance;
class		CScene;
struct	ts_Light;
struct	ts_Frustum;
struct	ts_Shadow;

//============================================================================

struct ts_ShadowMgr
{
	// pool of shadow casters
	ts_Shadow*			pShadowAlloc;				// original alloc'd buffer
	ts_SLinkList		ShadowFree;
	ts_SLinkList		ShadowActive;
	u16					u16ID;						// for assigning handles
	int					nMaxShadows;				// record passed value for debugging, not really necessary otherwise

	// status
	bool					bActive;						// will not render registered shadow(s) if false
	bool					bRenderingShadows;		// TRUE while in ShadowMgr__RenderShadows
	bool					bCreatingShadows;			// TRUE while in ShadowMgr__CreateShadows
	bool					bShadowsCreated;			// at least one shadow was created by ShadowMgr__CreateShadows

	#ifndef CONSUMER_BUILD
	// for debugging
	ts_Shadow*			pCurrentShadow;			// current shadow being rendered
	ts_Frustum*			pCurrentFrustum;			// current frustum being used for scene re-rendering
	#endif //CONSUMER_BUILD
};

//============================================================================

extern ts_ShadowMgr ShadowMgr;

//============================================================================

void			ShadowMgr__Init( int nMaxShadows );
void			ShadowMgr__Reset();
void			ShadowMgr__Shutdown();
void			ShadowMgr__Enable();
void			ShadowMgr__Disable();
#ifndef CONSUMER_BUILD
bool			ShadowMgr__IsEmpty();
#endif //CONSUMER_BUILD
t_Handle		ShadowMgr__RegisterShadow( CInstance* pInst, ts_Light* pLight, float fRadiusAdjust = 1.0f, Vector3 *centerOverride = NULL, float *radiusOverride = NULL );
void			ShadowMgr__KillShadow( t_Handle ShadowHandle );
void			ShadowMgr__ReplaceInstance( CInstance* pOldInst, CInstance* pNewInst );
bool			ShadowMgr__AddLight( t_Handle ShadowHandle, ts_Light* pLight, float fRadiusAdjust );
void			ShadowMgr__RemoveLight( t_Handle ShadowHandle, ts_Light* pLight );
void			ShadowMgr__AddLightAll( ts_Light* pLight );
void			ShadowMgr__RemoveLightAll( ts_Light* pLight );
void			ShadowMgr__CreateShadows();
inline bool	ShadowMgr__CreatingShadows() { return ShadowMgr.bCreatingShadows; }
void			ShadowMgr__RenderShadows( CScene* pScene );
inline bool	ShadowMgr__RenderingShadows() { return ShadowMgr.bRenderingShadows; }
#ifndef CONSUMER_BUILD
inline ts_Frustum* ShadowMgr__GetCurrentFrustum() { return ShadowMgr.pCurrentFrustum; }
#endif //CONSUMER_BUILD
void			ShadowMgr__SetShadowRez( u16 u, u16 v );

#if defined(DIRECTX)
void			ShadowMgr__ClearAllInstanceBits( void );
void			ShadowMgr__SetInstanceBits( CInstance * pInst );
void			ShadowMgr__ApplyShadows( unsigned int & startTextureUnit );
#endif

bool			ShadowMgr__UpdateLight( t_Handle ShadowHandle, ts_Light *pLight, float fRadiusAdjust = 1.0f );

//============================================================================

#endif //_SHADOW_MGR_H
