/**********************************************/
/*                                            */
/* SceneView.cpp                              */
/* big juju prototype  12/20/01               */
/* ned martin  avalanche software             */
/* visual c++ 6.0                             */
/* view frustum management for scenes         */
/*                                            */
/**********************************************/

/* the view class is provided so that each scene can have it's own view characteristics. the main job of the view is to
define the view frustum for the scene (aspect ratio, fov, near and far clip, fog start and end). the DisplayState record
has default settings, typically set up by processing the light.var script file. each CScene record has a SceneView,
member, which is initialized to the deafult settings in DisplayState when the scene is registered, but from then on it is
independent-- changing the DisplayState settings will not update the independent views */

/******************* includes ****************************/

#include "Display/DisplayPCH.h"





/******************* defines *****************************/

/* default values for scene view */

#define DEFAULT_FIELD_OF_VIEW_Y	65.0f
#define DEFAULT_NEAR_CLIP			1.0f
#define DEFAULT_FAR_CLIP			100.0f
#define DEFAULT_NEAR_FOG			40.0f
#define DEFAULT_FAR_FOG				100.0f

/******************* macros ******************************/

/******************* structures **************************/

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

/******************* local prototypes ********************/

/******************* functions ***************************/

/* constructor */

CSceneView::CSceneView()
{
	memset(this, 0, sizeof(CSceneView));

	#if defined (_XBOX)
	if (DirectX_PresentParams.Flags & D3DPRESENTFLAG_WIDESCREEN)
		SetAspectRatio(16.0f / 9.0f);	// 16 x 9 for widescreen
	else
		SetAspectRatio(4.0f / 3.0f);	// 4 x 3 is standard tv aspect ratio
	#else
	SetAspectRatio(4.0f / 3.0f);	// 4 x 3 is standard tv aspect ratio
	#endif

	SetFieldOfViewY(DEFAULT_FIELD_OF_VIEW_Y);
	SetNearClip(DEFAULT_NEAR_CLIP);
	SetFarClip(DEFAULT_FAR_CLIP);
	SetNearFog(DEFAULT_NEAR_FOG);
	SetFarFog(DEFAULT_FAR_FOG);
	SetClearMode(TRUE);
}

/* implement the view settings for rendering */

void CSceneView::Render()
{
	/* signal we are in this routine */

	m_bInRender = TRUE;

	/* rebuild view frustum if necessary */

	if (m_bFrustumDirty)
	{
		BuildViewFrustum();

		/* record some states for UpdateFog */

		m_bFogWhenFrustumBuilt = m_bFog;
		m_bFarFogPlaneValid = m_ViewFrustum.bFarFogPlaneValid;
		m_bNearFogPlaneValid = m_ViewFrustum.bNearFogPlaneValid;
	}

	/* load projection matrix for this frustum shape */

	MatrixStack__Mode(MS_PROJECTION);
	MatrixStack__Load(&GetProjection());

	/* set fog */

	RenderState__Unlock(RS_FOG);
   RenderState__Request(RS_FOG, m_bFog);
	if (m_bFog)
	{
	   RenderState__Request(RS_FOGSTART, RenderState__ftoi(m_fNearFog));
	   RenderState__Request(RS_FOGEND, RenderState__ftoi(m_fFarFog));
	   RenderState__Request(RS_FOGCOLOR, RenderState__Colorubv((unsigned char *) &m_FogColor));
		RenderState__Request(RS_FOGMODE, m_nFogMode);
		RenderState__Request(RS_FOGDENSITY, RenderState__ftoi(m_fFogDensity));
	}
	else
		RenderState__Lock(RS_FOG);	// lock fog off

	/* set viewport if valid. if not valid, viewport must be set explicitly (e.g. by a call to TextureBind__StartRenderToTexture) */

	if (m_bValidViewport)
		MatrixStack__Viewport(&m_Viewport);

	/* we're done */

	m_bInRender = FALSE;
}

/* clear the frame buffer that scene view uses */

void CSceneView::ClearFrameBuffer()
{
	/* clear frame buffer and/or z-buffer-- just the viewport this scene needs */

	gfxClearFrameBuffer(m_u32ClearMode, &m_ClearColor, &m_Viewport);
}

/* build up the view frustum that matches the settings in the scene view */

void CSceneView::BuildViewFrustum()
{
	m_bFrustumDirty = FALSE;
#ifndef PS2
	Frustum__Build(
		&m_ViewFrustum,
		m_fNearClip,
		m_fFarClip,
		m_bFog,
		m_fNearFog,
		m_fFarFog,
		m_fFieldOfViewY,
		m_fAspectRatio);
#else
	///// HACK /////
	// TJC - for some reason, this is not correctly determining when an object will cross the far clip plane.
	// its off by a very small amount.  for now Im just pushing the frustum a little to make up for the
	// deficiency.  The problem is that flat objects (trees) at the edge of farclip, dont always have clipping
	// turned on when they should.  This adjustment makes the clipping turn on a little sooner.
	///// HACK /////
	Frustum__Build(
		&m_ViewFrustum,
		m_fNearClip,
		m_fFarClip * 0.99f,
		m_bFog,
		m_fNearFog,
		m_fFarFog,
		m_fFieldOfViewY,
		m_fAspectRatio);

	/* for ps2, we also want the frustum that encloses the 4096 by 4096 pixel rendering space. the actual frame buffer */
	/* sits in the middle of this space (typically, anyway. i think that it's position is controlled by xyoffset register, */
	/* though why anyone would want to have it anyplace else but the center i don't know). the "guard band" is the */
	/* space surrounding the frame buffer. pixels can be rendered into that space without clipping. the ps2 renderer is */
	/* much faster rendering unclipped models than clipped models, so we want to mark models as unclipped if they fall */
	/* within the guard band frustum, not just if they fall within the actual view frustum */

	/* the guard band frustum is determined by both the field of view and the screen's pixel resolution. we have this */
	/* (top view): 

            <---- 2048 pixels --->

            <- sw / 2 ->                  sw = screen width in pixels
        ^   c----------+---------r        c = center of screen, r = right side of screen
        |   |         /        /
        |   |        /       /
        |   | fov/2 /      /              fov = field of views, in degrees
        |   |<---->/     /
        d   |     /    /                  d = distance from camera to screen plane (in pixels, i guess)
        |   |    /   /
        |   |   /  /
        |   |  / /
        |   | //
        v   |/
            e                             e = eye point (camera)

	/* we want to know the angle c-e-r to calculate the left and right planes of the guard band frustum. we know */
	/* (fov / 2) = arctan ((sw / 2) / d), or, solving for d, d = (sw / 2) / (tan (fov / 2)). then the angle we want */
	/* is arctan (2048 / d). putting it all together, we have:

		angle / 2 = arctan (2048 * (tan (fov / 2)) / (sw / 2))

	/* let's give it a try. this gets a bit complicated-- we're essentially determining the x field of view angle */
	/* so that we can calculate the aspect ratio of the guard band so we can pass the aspect ratio to the frustum */
	/* building code so it can calculate the x field of view, but this only happens once on the ps2, and passing */
	/* the aspect ratio seems a bit more useful than passing the x field of view */

	///// HACK /////
	// TJC - I changed the 2048.0f to 1020.0f because otherwise, clipping wasnt always on when it needed to be..
	// I dont think this calculation is correct.   We have clipping turned off when sometimes it should
	// be turned on.
	///// HACK /////
	#define PIXEL_WIDTH_TO_DRAW 1020.0f
	float fHalfViewYAngleRadians = Math::Deg2Rad(m_fFieldOfViewY / 2.0f);
	float fHalfViewXAngleRadians = Math::ArcTan(Math::Tan(fHalfViewYAngleRadians) * m_fAspectRatio);
	float fHalfGuardBandXAngleTan = PIXEL_WIDTH_TO_DRAW * Math::Tan(fHalfViewXAngleRadians) / (m_nViewportWidth / 2.0f);
	float fHalfGuardBandYAngleTan = PIXEL_WIDTH_TO_DRAW * Math::Tan(fHalfViewYAngleRadians) / (m_nViewportHeight / 2.0f);
	float fGuardBandFieldOfViewY = 2.0f * Math::Rad2Deg(Math::ArcTan(fHalfGuardBandYAngleTan));
	float fGuardBandAspectRatio = fHalfGuardBandXAngleTan / fHalfGuardBandYAngleTan;
	Frustum__Build(
		&m_GuardBandFrustum,
		m_fNearClip,
		m_fFarClip,
		FALSE,								// fog not a factor for guard band frustum
		0.0f,									// fog start
		0.0f,									// fog end
		fGuardBandFieldOfViewY,
		fGuardBandAspectRatio);
#endif //PS2
}

/* update fog status. this is a bit of a hack. we currently allow fog to be turned off on an instance-by-instance
(via checking the INSTANCE_NO_FOG bit). it is not clear if it ever desirable to have an unfogged instance when fog 
is on (i can't think of case off hand). but we have to update the view frustum fog status when this happens. it
should never happen that an instance turns on fog when the scene view has turned it off-- the scene view locks the
RS_FOG state. only the opposite-- an instance can turn fog off (or back on) when the scene view has turned it on */

void CSceneView::UpdateFog(bool bFog)
{
	if (bFog)
	{
		/* restore states to state when frustum was built */

		ASSERT(m_bFogWhenFrustumBuilt);	// if this hits, something is trying to turn on fog when scene view says all fog should be off
		m_ViewFrustum.bFarFogPlaneValid = m_bFarFogPlaneValid;
		m_ViewFrustum.bNearFogPlaneValid = m_bNearFogPlaneValid;
	}
	else
	{
		/* disable fog check in view frustum */

		m_ViewFrustum.bFarFogPlaneValid = FALSE;
		m_ViewFrustum.bNearFogPlaneValid = FALSE;
	}
}

/* set which buffers should be cleared-- always clear z-buffer */

void CSceneView::SetClearMode(bool bClearColorBuffer)
{
	if (bClearColorBuffer)
		m_u32ClearMode = RS_DEPTH_BUFFER_BIT | RS_COLOR_BUFFER_BIT;
	else
		m_u32ClearMode = RS_DEPTH_BUFFER_BIT;
}

/* set the viewport to use for this scene view. this is for game view scenes-- those that are presented to the
player. things like rendering a scene into a texture ignores the viewport */

void CSceneView::SetViewport(float fX, float fY, float fWidth, float fHeight)
{
	/* record values */

	m_Viewport.fX = fX;
	m_Viewport.fY = fY;
	m_Viewport.fWidth = fWidth;
	m_Viewport.fHeight = fHeight;

	/* calculate size of frame buffer viewport in pixels */

	m_nViewportWidth = (int) ((float) Display__GetScreenWidth() * fWidth);
	m_nViewportHeight = (int) ((float) Display__GetScreenHeight() * fHeight);

	/* calculate aspect ratio, taking viewport into account */

	m_fAspectRatio = Display__GetScreenAspectRatio() * (fWidth / fHeight);
	m_bProjectionDirty = true;

	/* this scene view is ts_Viewport-based */

	m_bValidViewport = TRUE;

	/* rebuild frustum-- aspect ratio may have changed */

	m_bFrustumDirty = TRUE;
}

/* set the size of the viewport in pixels. this is for scene views that are used to render into a texture */

void CSceneView::SetViewport(int nWidth, int nHeight)
{
	m_nViewportWidth = nWidth;
	m_nViewportHeight = nHeight;
	m_bValidViewport = FALSE;

#ifdef PS2
	/* currently, only need to rebuild frustum for ps2, since guard band frustum uses viewport height and width in */
	/* its calculations */

	m_bFrustumDirty = TRUE;
#endif //PS2
}

/* recalculate the viewport. used to support resizing window on pc */

void CSceneView::ResetViewport()
{
	if (m_bValidViewport)
		SetViewport(m_Viewport.fX, m_Viewport.fY, m_Viewport.fWidth, m_Viewport.fHeight);
}

/* get the projection matrix for this view */

Matrix4x4 &CSceneView::GetProjection()
{
	/* may need to re-calculate */

	if (m_bProjectionDirty)
	{
		MatrixStack__Mode(MS_PROJECTION);
		MatrixStack__Push();
		MatrixStack__Identity();
		MatrixStack__Perspective(
			m_fFieldOfViewY,
			m_fAspectRatio,
			m_fNearClip,
			m_fFarClip);
		m_mProjection = *MatrixStack__Get(MS_PROJECTION);
		MatrixStack__Pop();
		m_bProjectionDirty = false;
	}
	return(m_mProjection);
}

// TJC - because of the way we cheat with PS2 builds, these resolve warnings.
#undef DEFAULT_FAR_CLIP
#undef DEFAULT_NEAR_FOG
#undef DEFAULT_FAR_FOG
