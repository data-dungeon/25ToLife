
// engine includes.

#include "Display/Common/RenderState.h"

//
// Inline to try to help the ICache...
//


inline u8 RenderMgr__GetNumVBlanksToSyncTo(void)
{
	return RenderMgr.nNumVBlanksToSyncTo;
}

inline bool RenderMgr__WireframeOn(void)
{
	return(RenderMgr.bWireframe);
}

#ifndef CONSUMER_BUILD

/* render control variable state */

inline bool RenderMgr__LightingOn(void)
{
	if ((RenderState__Get(RS_LIGHTING) == RS_FALSE) && (RenderState__Locked(RS_LIGHTING)))
		return(FALSE);
	return(RenderMgr.bLighting);
}
inline bool RenderMgr__VertexColorsOn(void)
{
	return(RenderMgr.bVertexColors);
}
inline bool RenderMgr__FogEnabled(void)
{
	return(RenderMgr.bFogEnable);
}
inline u8 RenderMgr__BoneRender(void)
{
	return(RenderMgr.nBoneRender);
}
inline bool RenderMgr__BackfaceCullPatchOn(void)
{
	return(RenderMgr.bBackfaceCullPatch);
}
inline bool RenderMgr__ForceZoomOn(void)
{
	return(RenderMgr.bForceZoom);
}
inline float RenderMgr__ForceZoomValue(void)
{
	return(RenderMgr.fForceZoom);
}
inline bool RenderMgr__DiffuseMapsOn(void)
{
	return(RenderMgr.bDiffuseMaps);
}
inline bool RenderMgr__EnvironmentMapsOn(void)
{
	return(RenderMgr.bEnvironmentMaps);
}
inline bool RenderMgr__LODsOn(void)
{
	return(RenderMgr.bLODs);
}
inline u8 RenderMgr__LODsForce(void)
{
	return(RenderMgr.nLODs);
}
inline bool RenderMgr__ForceAlphaTest(void)
{
	return(RenderMgr.bForceAlphaTest);
}
inline u8 RenderMgr__ForceAlphaTestValue(void)
{
	return(RenderMgr.nForceAlphaTest);
}
inline bool RenderMgr__DrawBoxesOn(void)
{
	return(RenderMgr.bDrawBoxes);
}
inline bool RenderMgr__OverrideBrightnessOn(void)
{
	return(RenderMgr.bOverrideBrightness);
}
inline bool RenderMgr__VSyncOn(void)
{
	return(RenderMgr.bVSync);
}
inline bool RenderMgr__UseDisplayLists(void)
{
	return(RenderMgr.bUseDisplayLists);
}
inline bool RenderMgr__VisualizeSceneTree(void)
{
	return(RenderMgr.bVisualizeSceneTree);
}
inline bool RenderMgr__ViewBackBuffer(void)
{
	return(RenderMgr.bViewBackBuffer);
}
inline bool RenderMgr__ShadowDebugOn(void)
{
	return(RenderMgr.bShadowDebug);
}

#endif // CONSUMER_BUILD

inline bool RenderMgr__CartoonRenderer(void)
{
	return RenderMgr.bCartoonRenderer;
}

