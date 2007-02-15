/***************************************************************************/
/***************************************************************************/
#include "Effects/EffectsPCH.h"

#include "hud/hudlib.h"
#include "WatchClient/WatchClient.h"
#include "Effects/fullscreendistort.h"
#include "cmdcon/cmdcon.h"
#include "effects/fsdsys.h"

// We are a singleton
DeclareSingleton(CFSDSystem);

bool g_distortDebug = false;

enum
{
	EVENT_DISTORT_PARAM_CRUCIAL_CHANGE,
	EVENT_DISTORT_PARAM_CHANGE
};

/***************************************************************************/
/***************************************************************************/
CFSDSystem::CFSDSystem()
#ifdef ENABLE_WATCHCLIENT
:	m_messageOwner("FSDSystem")
#endif
{
	pDistort = NULL;

#ifdef ENABLE_WATCHCLIENT
	m_watchHandler.Initialize("ConsoleVarModified", MESSAGE_HANDLE_IF_MASTER, m_messageOwner, this);
#endif
}

/***************************************************************************/
/***************************************************************************/
CFSDSystem::~CFSDSystem()
{
#ifdef ENABLE_WATCHCLIENT
	m_messageOwner.UnregisterAll();
#endif

	// run through the m_distortions deleting each of them.
	for ( int i = m_distortions.Count() - 1; i >= 0 ; i-- )
	{
		// Anything left in this list should be auto delete...
		ASSERT(m_distortions[i]->AutoDelete());
		if (m_distortions[i]->AutoDelete())
			delete m_distortions[i];
	}
}

/***************************************************************************/
/***************************************************************************/
CFSDEffect *CFSDSystem::StartEffect( char const *pEffectName, const Vector3 *worldPos, int flags )
{
	CFSDEffect *pNewDistort = NULL;

	// if a previous effect is running, then tell it to turn off (but continue
	// processing counters)
	if ( pDistort )
		pDistort->DisplayOn( false );

	// attempt to start a new effect
	pDistort = new CFSDEffect( pEffectName, worldPos, flags );
	ASSERT( pDistort );

	// remember our new effect
	m_distortions.Add( pDistort );

	return( pDistort );
}

/***************************************************************************/
/***************************************************************************/
void CFSDSystem::StopEffect( CFSDEffect *pDistortEffect )
{
	// if we have the specified effect in our list, then delete the effect and
	// remove it from the list
	bool found = false;
	for ( int i = 0; i < m_distortions.Count(); i++ )
	{
		if ( m_distortions[i] == pDistortEffect )
		{
			if (m_distortions[i]->AutoDelete())
				delete m_distortions[i];
			m_distortions.Delete( i );
			found = true;
			break;
		}
	}
	// If someone used a ptr to an effect that has been deleted already,
	// there may be other damage going on if they are not using
	// OWNERDELETE
	//ASSERT(found);

	// make sure our most recent effect is displaying
	if ( m_distortions.Count() > 0 )
	{
		pDistort = m_distortions[ m_distortions.Count() - 1 ];
		pDistort->DisplayOn( true );
	}
	else
	{
		// no more effects, so null out our pointer
		pDistort = NULL;
	}
}

/***************************************************************************/
/***************************************************************************/
bool CFSDSystem::IsBusy( void )
{
	return( ( pDistort != NULL ) );
}

/***************************************************************************/
/***************************************************************************/
void CFSDSystem::Update( void )
{
	// if we have the specified effect in our list, then delete the effect and
	// remove it from the list
	int i = 0;
	while ( i < m_distortions.Count() )
	{
		if ( !m_distortions[i]->Update() )
			StopEffect( m_distortions[i] );
		else
			i++;
	}
}

#ifdef ENABLE_WATCHCLIENT
/***************************************************************************/
/***************************************************************************/
void CFSDSystem::WatchHandler::HandleMessage(
void* i_data,
ActorHandle i_sender,
bool i_posted)
{
	if ( d_ref->pDistort )
		d_ref->pDistort->StuffChanged( (u32)i_data );
}
#endif

//##########################################################################
//##########################################################################
//##########################################################################

/***************************************************************************/
/***************************************************************************/
CFSDEffect::CFSDEffect( const char *pEffectName, const Vector3 *pos, int flags )
{
/*
 *	Register to receive events
 */
	distortFlags = flags;
	worldPos = pos;

	pDistortMenu = NULL;
	fRefreshTimer = 0.0f;
	bDisplayOn = true;
	loopSustain = false;

	strcpy( currentContext, pEffectName );

	InitParams();

	RegisterWatchVariables( pEffectName );

	effectAttackTimer = effectAttackTime;
	effectSustainTimer = effectSustainTime;
	effectDecayTimer = effectDecayTime;

	InitEffect();
}

/***************************************************************************/
/***************************************************************************/
CFSDEffect::~CFSDEffect()
{
	StopEffect();

#ifdef FSD_DEBUG_MENU
	delete( pDistortMenu );
#endif
}

/***************************************************************************/
/***************************************************************************/
void CFSDEffect::InitEffect( void )
{
	Calculate();

	if ( bDisplayOn )
	{
		dHandle1 = INVALID_HANDLE;
		dHandle2 = INVALID_HANDLE;

		FullScreenDistort::Init( gridX, gridY, opacity, (FullScreenDistort::MODE) mode );
		FullScreenDistort::SetFilterColor( cColor.ABGR ); // this color supports overbrighting! so 0x80 is 1.0. (its in ABGR order)

		if ( bCenterOnScreen )
			FullScreenDistort::SetFilterColorPoint( cpCenterX, cpCenterY, temp_cpInnerRadius, temp_cpOuterRadius, cpColor.ABGR );

		if ( xOn )
			dHandle1 = FullScreenDistort::Add( FullScreenDistort::X, xCenterX, xCenterY, temp_xInnerRadius, temp_xOuterRadius, (bool) (xInverseArea != 0), xAttackTime, xSustainTime, xDecayTime, (bool) (xUseOmega != 0), (bool) (xFeedBack != 0), new SigGen( (SigGen::TYPE) xSigType, xSigFreq, xSigAmp ) );
		if ( yOn )
			dHandle2 = FullScreenDistort::Add( FullScreenDistort::Y, yCenterX, yCenterY, temp_yInnerRadius, temp_yOuterRadius, (bool) (yInverseArea != 0), yAttackTime, ySustainTime, yDecayTime, (bool) (yUseOmega != 0), (bool) (yFeedBack != 0), new SigGen( (SigGen::TYPE) ySigType, ySigFreq, ySigAmp ) );

		FullScreenDistort::UseOSTimer( (distortFlags & DISTORT_FLAGS_USE_OSTIMER) != 0 );
	}
}

void CFSDEffect::Calculate( void )
{
	float tRatio = 1.0f;

	if ( effectAttackTimer < effectAttackTime )
		tRatio -= effectAttackTimer / effectAttackTime;
	else if ( ( effectSustainTimer == 0.0f ) && ( effectDecayTimer != effectDecayTime ) )
		tRatio = effectDecayTimer / effectDecayTime;

	cColor.ABGR = ( ( cStartColorA + ((int) ( ( cColorA - cStartColorA ) * tRatio ) )) << 24 ) |
								( ( cStartColorB + ((int) ( ( cColorB - cStartColorB ) * tRatio ) )) << 16 ) |
								( ( cStartColorG + ((int) ( ( cColorG - cStartColorG ) * tRatio ) )) << 8 ) |
								( ( cStartColorR + ((int) ( ( cColorR - cStartColorR ) * tRatio ) )) );

	cpColor.ABGR =	( ( cpStartColorA + ((int) ( ( cpColorA - cpStartColorA ) * tRatio ) )) << 24 ) |
									( ( cpStartColorB + ((int) ( ( cpColorB - cpStartColorB ) * tRatio ) )) << 16 ) |
									( ( cpStartColorG + ((int) ( ( cpColorG - cpStartColorG ) * tRatio ) )) << 8 ) |
									( ( cpStartColorR + ((int) ( ( cpColorR - cpStartColorR ) * tRatio ) )) );

	tempOpacity = startOpacity + ( opacity - startOpacity ) * tRatio;

	temp_cpInnerRadius = cpStartInnerRadius + ( cpInnerRadius - cpStartInnerRadius ) * tRatio;
	temp_cpOuterRadius = cpStartOuterRadius + ( cpOuterRadius - cpStartOuterRadius ) * tRatio;
	temp_xInnerRadius = xInnerRadius;
	temp_xOuterRadius = xOuterRadius;
	temp_yInnerRadius = yInnerRadius;
	temp_yOuterRadius = yOuterRadius;
	bCenterOnScreen = false;

	// check to see if we should center on the initiator of the effect
	if (worldPos && (cpCenterOnInitiator || xCenterOnInitiator || yCenterOnInitiator))
	{
		// Put it in screen space
		Vector2 vScreenCoords;
		float z;
		Camera &camera = g_cameraDirector.CurrentCamera();

		Vector3Packed world = *worldPos;
		if ( camera.ToScreen(world, vScreenCoords, z) )
		{
			if ( cpCenterOnInitiator )
			{
				cpCenterX = vScreenCoords.x();
				cpCenterY = vScreenCoords.y();

				if ( z > 0.0f )
				{
					temp_cpInnerRadius = ( cpStartInnerRadius + ( cpInnerRadius - cpStartInnerRadius ) * tRatio ) / z;
					temp_cpOuterRadius = ( cpStartOuterRadius + ( cpOuterRadius - cpStartOuterRadius ) * tRatio ) / z;
				}
			}
			if ( xCenterOnInitiator )
			{
				xCenterX = vScreenCoords.x();
				xCenterY = vScreenCoords.y();

				if ( z > 0.0f )
				{
					temp_xInnerRadius = xInnerRadius / z;
					temp_xOuterRadius = xOuterRadius / z;
				}
			}
			if ( yCenterOnInitiator )
			{
				yCenterX = vScreenCoords.x();
				yCenterY = vScreenCoords.y();

				if ( z > 0.0f )
				{
					temp_yInnerRadius = yInnerRadius / z;
					temp_yOuterRadius = yOuterRadius / z;
				}
			}
		}
	}

	if ( temp_cpOuterRadius > 1.0f )
		temp_cpOuterRadius = 1.0f;
	if ( temp_cpInnerRadius > temp_cpOuterRadius )
		temp_cpInnerRadius = temp_cpOuterRadius;
}

/***************************************************************************/
/***************************************************************************/
void CFSDEffect::StopEffect( void )
{
	if ( bDisplayOn )
	{
		if ( dHandle1 != INVALID_HANDLE )
		{
			FullScreenDistort::Remove( dHandle1 );
			dHandle1 = INVALID_HANDLE;
		}
		if ( dHandle2 != INVALID_HANDLE )
		{
			FullScreenDistort::Remove( dHandle2 );
			dHandle2 = INVALID_HANDLE;
		}
		FullScreenDistort::Shutdown();
	}
}

/***************************************************************************/
/***************************************************************************/
bool CFSDEffect::Update( void )
{
	float deltaSec = g_timer.GetFrameSec();

	if ( ( deltaSec > 0.0f ) && ( distortFlags & DISTORT_FLAGS_USE_OSTIMER ) )
		deltaSec = g_timer.GetOSFrameSec();

	if ( fRefreshTimer > 0.0f )
	{
		Math::MoveToZero( &fRefreshTimer, deltaSec );
		if ( fRefreshTimer == 0.0f )
		{
			StopEffect();
			effectAttackTimer = 0.0f;
			effectSustainTimer = effectSustainTime;
			effectDecayTimer = effectDecayTime;
			InitEffect();
		}
	}

	if ( effectAttackTimer > 0.0f )
		Math::MoveToZero( &effectAttackTimer, deltaSec );
	else if ( effectSustainTimer > 0.0f )
	{
		if (!loopSustain)
			Math::MoveToZero( &effectSustainTimer, deltaSec );
	}
	else if ( effectDecayTimer > 0.0f )
	{
		Math::MoveToZero( &effectDecayTimer, deltaSec );

		// if we are not debugging this effect and we have timed out, then
		// tell system to stop this effect
		if (Math::Zero(effectDecayTimer))
		{
			if ( !g_distortDebug )
				return( false );
			else
				fRefreshTimer = 1.0f;
		}
	}

	// update
	Calculate();

	// draw
	if ( bDisplayOn )
	{
		FullScreenDistort::SetFilterColor( cColor.ABGR ); // this color supports overbrighting! so 0x80 is 1.0. (its in ABGR order)
		if ( bCenterOnScreen )
			FullScreenDistort::SetFilterColorPoint( cpCenterX, cpCenterY, temp_cpInnerRadius, temp_cpOuterRadius, cpColor.ABGR );
		FullScreenDistort::SetOpacity( tempOpacity );
	}

	return( true );
}

// state of the effect
CFSDEffect::State CFSDEffect::GetState()
{
	if ( dHandle1 == INVALID_HANDLE && dHandle2 == INVALID_HANDLE )
		return FSDFX_DONE;

	if ( effectAttackTimer > 0.0f )
		return FSDFX_ATTACK;
	else if ( effectSustainTimer > 0.0f )
		return FSDFX_SUSTAIN;
	else if ( effectDecayTimer > 0.0f )
		return FSDFX_DECAY;

	return FSDFX_DONE;
}

// Reset the effect to sustain
bool CFSDEffect::ResetToSustain()
{
	State state = GetState();
	if (state == FSDFX_DONE)
		return false;

	effectSustainTimer = effectSustainTime;
	effectDecayTimer = effectDecayTime;
	return true;
}

void CFSDEffect::DisplayOn( bool flag )
{
	if ( !flag )
	{
		StopEffect();
		bDisplayOn = false;
	}
	else
	if ( !bDisplayOn )
	{
		bDisplayOn = true;
		InitEffect();
	}
}

/***************************************************************************/
/***************************************************************************/
void CFSDEffect::StuffChanged(u32 u32Event)
{
	ts_ABGR cColor;
	ts_ABGR cpColor;

	cColor.ABGR = ( cColorA << 24 ) | ( cColorB << 16 ) | ( cColorG << 8 ) | cColorR;
	cpColor.ABGR = ( cpColorA << 24 ) | ( cpColorB << 16 ) | ( cpColorG << 8 ) | cpColorR;

	switch( u32Event )	// use mask because some events have variable category bits
	{
	case EVENT_DISTORT_PARAM_CRUCIAL_CHANGE:
	case EVENT_DISTORT_PARAM_CHANGE:
		fRefreshTimer = 1.0f;
		break;
	}
}

/***************************************************************************/
/***************************************************************************/
void CFSDEffect::InitParams( void )
{
	// init params
	gridX = 4;
	gridY = 3;
	opacity = 0.6f;
	mode = FullScreenDistort::NORMAL;

	// color params
	cColorA = 0x80;
	cColorB = 0x80;
	cColorG = 0x80;
	cColorR = 0x80;

	// color point params
	cpCenterX = FSD_CENTERX;
	cpCenterY = FSD_CENTERY;
	cpInnerRadius = 0.25f;
	cpOuterRadius = 0.5f;
	cpColorA = 0x80;
	cpColorB = 0x80;
	cpColorG = 0x80;
	cpColorR = 0x80;
	cpCenterOnInitiator = false;

	// x grid params
	xOn = true;
	xCenterX = FSD_CENTERX;
	xCenterY = FSD_CENTERY;
	xInnerRadius = 0.0f;
	xOuterRadius = 0.6f;
	xInverseArea = true;
	xAttackTime = 0.0f;
	xSustainTime = 0.0f;
	xDecayTime = 0.0f;
	xUseOmega = true;
	xFeedBack = false;
	xSigType = SigGen::SIN;
	xSigFreq = 0.75f;
	xSigAmp = 0.02f;
	xCenterOnInitiator = false;

	// y grid params
	yOn = true;
	yCenterX = FSD_CENTERX;
	yCenterY = FSD_CENTERY;
	yInnerRadius = 0.0f;
	yOuterRadius = 0.6f;
	yInverseArea = true;
	yAttackTime = 0.0f;
	ySustainTime = 0.0f;
	yDecayTime = 0.0f;
	yUseOmega = true;
	yFeedBack = false;
	ySigType = SigGen::SIN;
	ySigFreq = 0.6f;
	ySigAmp = 0.015f;
	yCenterOnInitiator = false;

	// effects params
	effectAttackTime = 0.0f;
	effectSustainTime = 0.0f;
	effectDecayTime = 0.0f;
	startOpacity = 0.6f;
	cStartColorA = 0x80;
	cStartColorB = 0x80;
	cStartColorG = 0x80;
	cStartColorR = 0x80;
	cpStartInnerRadius = 0.25f;
	cpStartOuterRadius = 0.5f;
	cpStartColorA = 0x80;
	cpStartColorB = 0x80;
	cpStartColorG = 0x80;
	cpStartColorR = 0x80;
}

/***************************************************************************/
/***************************************************************************/
void CFSDEffect::RegisterWatchVariables( const char *context )
{
	// context cleanup is done at the beginning rather than the end so that
	//	adjustments to the variables can be saved out via the watch window.

	g_console.DeleteContext( context );

	char script[128];
	strcpy(script, "var\\fsdfx\\");
	strcat(script, context);
	g_console.BindScript( context, script );

	// init params
	g_console.CreateVar( context, "gridX", gridX );
	g_console.CreateVar( context, "gridY", gridY );
	g_console.CreateVar( context, "opacity", opacity );
	g_console.CreateVar( context, "mode", mode );

	// color params
	g_console.CreateVar( context, "cColorA", cColorA );
	g_console.CreateVar( context, "cColorB", cColorB );
	g_console.CreateVar( context, "cColorG", cColorG );
	g_console.CreateVar( context, "cColorR", cColorR );

	// color point params
	g_console.CreateVar( context, "cpCenterX", cpCenterX );
	g_console.CreateVar( context, "cpCenterY", cpCenterY );
	g_console.CreateVar( context, "cpInnerRadius", cpInnerRadius );
	g_console.CreateVar( context, "cpOuterRadius", cpOuterRadius );
	g_console.CreateVar( context, "cpColorA", cpColorA );
	g_console.CreateVar( context, "cpColorB", cpColorB );
	g_console.CreateVar( context, "cpColorG", cpColorG );
	g_console.CreateVar( context, "cpColorR", cpColorR );
	g_console.CreateVar( context, "cpCenterOnInitiator", cpCenterOnInitiator );

	// x grid params
	g_console.CreateVar( context, "xOn", xOn );
	g_console.CreateVar( context, "xCenterX", xCenterX );
	g_console.CreateVar( context, "xCenterY", xCenterY );
	g_console.CreateVar( context, "xInnerRadius", xInnerRadius );
	g_console.CreateVar( context, "xOuterRadius", xOuterRadius );
	g_console.CreateVar( context, "xInverseArea", xInverseArea );
	g_console.CreateVar( context, "xAttackTime", xAttackTime );
	g_console.CreateVar( context, "xSustainTime", xSustainTime );
	g_console.CreateVar( context, "xDecayTime", xDecayTime );
	g_console.CreateVar( context, "xUseOmega", xUseOmega );
	g_console.CreateVar( context, "xFeedBack", xFeedBack );
	g_console.CreateVar( context, "xSigType", xSigType );
	g_console.CreateVar( context, "xSigFreq", xSigFreq );
	g_console.CreateVar( context, "xSigAmp", xSigAmp );
	g_console.CreateVar( context, "xCenterOnInitiator", xCenterOnInitiator );

	// y grid params
	g_console.CreateVar( context, "yOn", yOn );
	g_console.CreateVar( context, "yCenterX", yCenterX );
	g_console.CreateVar( context, "yCenterY", yCenterY );
	g_console.CreateVar( context, "yInnerRadius", yInnerRadius );
	g_console.CreateVar( context, "yOuterRadius", yOuterRadius );
	g_console.CreateVar( context, "yInverseArea", yInverseArea );
	g_console.CreateVar( context, "yAttackTime", yAttackTime );
	g_console.CreateVar( context, "ySustainTime", ySustainTime );
	g_console.CreateVar( context, "yDecayTime", yDecayTime );
	g_console.CreateVar( context, "yUseOmega", yUseOmega );
	g_console.CreateVar( context, "yFeedBack", yFeedBack );
	g_console.CreateVar( context, "ySigType", ySigType );
	g_console.CreateVar( context, "ySigFreq", ySigFreq );
	g_console.CreateVar( context, "ySigAmp", ySigAmp );
	g_console.CreateVar( context, "yCenterOnInitiator", yCenterOnInitiator );

	// effects params
	g_console.CreateVar( context, "effectAttackTime", effectAttackTime );
	g_console.CreateVar( context, "effectSustainTime", effectSustainTime );
	g_console.CreateVar( context, "effectDecayTime", effectDecayTime );
	g_console.CreateVar( context, "startOpacity", startOpacity );
	g_console.CreateVar( context, "cStartColorA", cStartColorA );
	g_console.CreateVar( context, "cStartColorB", cStartColorB );
	g_console.CreateVar( context, "cStartColorG", cStartColorG );
	g_console.CreateVar( context, "cStartColorR", cStartColorR );
	g_console.CreateVar( context, "cpStartInnerRadius", cpStartInnerRadius );
	g_console.CreateVar( context, "cpStartOuterRadius", cpStartOuterRadius );
	g_console.CreateVar( context, "cpStartColorA", cpStartColorA );
	g_console.CreateVar( context, "cpStartColorB", cpStartColorB );
	g_console.CreateVar( context, "cpStartColorG", cpStartColorG );
	g_console.CreateVar( context, "cpStartColorR", cpStartColorR );

	g_console.ExecuteContext( context );

#ifdef ENABLE_WATCHCLIENT
	WatchClient* instance = WatchClient::Instance();

	// init params
	instance->RegisterVariable( (char *)context, &gridX, gridX, 4, 64, "gridX", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &gridY, gridY, 4, 64, "gridY", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
 	instance->RegisterVariable( (char *)context, &opacity, opacity, 0.0f, 1.0f, "opacity", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &mode, mode, (int) FullScreenDistort::NORMAL, (int) FullScreenDistort::GREYSCALE, "mode", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );

	// color params
	instance->RegisterVariable( (char *)context, &cColorR, cColorR, 0x00, 0xff, "cColorR", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cColorG, cColorG, 0x00, 0xff, "cColorG", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cColorB, cColorB, 0x00, 0xff, "cColorB", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cColorA, cColorA, 0x00, 0xff, "cColorA", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );

	// color point params
	instance->RegisterVariable( (char *)context, &cpCenterX, cpCenterX, 0.0f, 1.0f, "cpCenterX", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cpCenterY, cpCenterY, 0.0f, 1.0f, "cpCenterY", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cpInnerRadius, cpInnerRadius, 0.0f, 1.0f, "cpInnerRadius", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cpOuterRadius, cpOuterRadius, 0.0f, 1.0f, "cpOuterRadius", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cpColorR, cpColorR, 0x00, 0xff, "cpColorR", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cpColorG, cpColorG, 0x00, 0xff, "cpColorG", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cpColorB, cpColorB, 0x00, 0xff, "cpColorB", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cpColorA, cpColorA, 0x00, 0xff, "cpColorA", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cpCenterOnInitiator, cpCenterOnInitiator, false, true, "cpCenterOnInitiator", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );

	// x grid params
	instance->RegisterVariable( (char *)context, &xOn, xOn, false, true, "xOn", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &xCenterX, xCenterX, 0.0f, 1.0f, "xCenterX", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &xCenterY, xCenterY, 0.0f, 1.0f, "xCenterY", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &xInnerRadius, xInnerRadius, 0.0f, 1.0f, "xInnerRadius", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &xOuterRadius, xOuterRadius, 0.0f, 1.0f, "xOuterRadius", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &xInverseArea, xInverseArea, false, true, "xInverseArea", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &xAttackTime, xAttackTime, 0.0f, 1000.0f, "xAttackTime", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &xSustainTime, xSustainTime, 0.0f, 1000.0f, "xSustainTime", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &xDecayTime, xDecayTime, 0.0f, 1000.0f, "xDecayTime", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &xUseOmega, xUseOmega, false, true, "xUseOmega", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &xFeedBack, xFeedBack, false, true, "xFeedBack", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &xSigType, xSigType, (int) SigGen::NONE, (int) SigGen::FASTSMOOTHNOISE, "xSigType", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &xSigFreq, xSigFreq, 0.0f, 10.0f, "xSigFreq", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &xSigAmp, xSigAmp, 0.0f, 0.1f, "xSigAmp", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &xCenterOnInitiator, xCenterOnInitiator, false, true, "xCenterOnInitiator", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );

	// y grid params
	instance->RegisterVariable( (char *)context, &yOn, yOn, false, true, "yOn", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &yCenterX, yCenterX, 0.0f, 1.0f, "yCenterX", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &yCenterY, yCenterY, 0.0f, 1.0f, "yCenterY", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &yInnerRadius, yInnerRadius, 0.0f, 1.0f, "yInnerRadius", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &yOuterRadius, yOuterRadius, 0.0f, 1.0f, "yOuterRadius", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &yInverseArea, yInverseArea, false, true, "yInverseArea", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &yAttackTime, yAttackTime, 0.0f, 1000.0f, "yAttackTime", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &ySustainTime, ySustainTime, 0.0f, 1000.0f, "ySustainTime", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &yDecayTime, yDecayTime, 0.0f, 1000.0f, "yDecayTime", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &yUseOmega, yUseOmega, false, true, "yUseOmega", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &yFeedBack, yFeedBack, false, true, "yFeedBack", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &ySigType, ySigType, (int) SigGen::NONE, (int) SigGen::FASTSMOOTHNOISE, "ySigType", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &ySigFreq, ySigFreq, 0.0f, 10.0f, "ySigFreq", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &ySigAmp, ySigAmp, 0.0f, 0.1f, "ySigAmp", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &yCenterOnInitiator, yCenterOnInitiator, false, true, "yCenterOnInitiator", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );

	// effects params
	instance->RegisterVariable( (char *)context, &effectAttackTime, effectAttackTime, 0.0f, 5.0f, "effectAttackTime", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &effectSustainTime, effectSustainTime, 0.0f, 10.0f, "effectSustainTime", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &effectDecayTime, effectDecayTime, 0.0f, 5.0f, "effectDecayTime", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &startOpacity, startOpacity, 0.0f, 1.0f, "startOpacity", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cStartColorR, cStartColorR, 0x00, 0xff, "cStartColorR", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cStartColorG, cStartColorG, 0x00, 0xff, "cStartColorG", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cStartColorB, cStartColorB, 0x00, 0xff, "cStartColorB", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cStartColorA, cStartColorA, 0x00, 0xff, "cStartColorA", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cpStartInnerRadius, cpStartInnerRadius, 0.0f, 1.0f, "cpStartInnerRadius", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cpStartOuterRadius, cpStartOuterRadius, 0.0f, 1.0f, "cpStartOuterRadius", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cpStartColorR, cpStartColorR, 0x00, 0xff, "cpStartColorR", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cpStartColorG, cpStartColorG, 0x00, 0xff, "cpStartColorG", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cpStartColorB, cpStartColorB, 0x00, 0xff, "cpStartColorB", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );
	instance->RegisterVariable( (char *)context, &cpStartColorA, cpStartColorA, 0x00, 0xff, "cpStartColorA", &g_fsdSystem.m_messageOwner, (void *)EVENT_DISTORT_PARAM_CRUCIAL_CHANGE );

#endif //ENABLE_WATCHCLIENT

#ifdef FSD_DEBUG_MENU
	DebugMenu *debugMenu = GetDebugMenu();
	if ( g_distortDebug && debugMenu )
	{
		pDistortMenu = debugMenu->AddMenuItem( (char *)context );
		debugMenu->Branch();

		// init params
		debugMenu->AddMenuItem( "Init" );
		debugMenu->Branch();

		debugMenu->AddMenuItem( "Refresh", 0, RefreshDistortion, this );
		AddDebugVar( "gridX", &gridX, 4, 64, 1 );
		AddDebugVar( "gridY", &gridY, 4, 64, 1 );
		AddDebugVar( "opacity", &opacity, 0.0f, 1.0f, 0.1f );
		AddDebugVar( "Mode", &mode, (int) FullScreenDistort::NORMAL, (int) FullScreenDistort::GREYSCALE, 1 );
		debugMenu->SetCallBack( InitChanged, this );
		debugMenu->Return();

		// color params
		debugMenu->AddMenuItem( "Color" );
		debugMenu->Branch();

		debugMenu->AddMenuItem( "Refresh", 0, RefreshDistortion, this );
		AddDebugVar( "cColorR", &cColorR, 0x0, 0xff, 0x01 );
		AddDebugVar( "cColorG", &cColorG, 0x0, 0xff, 0x01 );
		AddDebugVar( "cColorB", &cColorB, 0x0, 0xff, 0x01 );
		AddDebugVar( "cColorA", &cColorA, 0x0, 0xff, 0x01 );
		debugMenu->Return();

		// color point params
		debugMenu->AddMenuItem( "Color Point" );
		debugMenu->Branch();

		debugMenu->AddMenuItem( "Refresh", 0, RefreshDistortion, this );
		AddDebugVar( "cpCenterX", &cpCenterX, 0.0f, 1.0f, 0.1f );
		AddDebugVar( "cpCenterY", &cpCenterY, 0.0f, 1.0f, 0.1f );
		AddDebugVar( "cpInnerRadius", &cpInnerRadius, 0.0f, 10.0f, 0.1f );
		AddDebugVar( "cpOuterRadius", &cpOuterRadius, 0.0f, 10.0f, 0.1f );
		AddDebugVar( "cpColorR", &cpColorR, 0x0, 0xff, 0x02 );
		AddDebugVar( "cpColorG", &cpColorG, 0x0, 0xff, 0x02 );
		AddDebugVar( "cpColorB", &cpColorB, 0x0, 0xff, 0x02 );
		AddDebugVar( "cpColorA", &cpColorA, 0x0, 0xff, 0x02 );
		AddDebugVar( "cpCenterOnInitiator", &cpCenterOnInitiator, 0, 1, 1 );
		debugMenu->Return();

		// x grid params
		debugMenu->AddMenuItem( "X Grid" );
		debugMenu->Branch();

		debugMenu->AddMenuItem( "Refresh", 0, RefreshDistortion, this );
		AddDebugVarX( "xOn", &xOn, 0, 1, 1 );
		AddDebugVarX( "xCenterX", &xCenterX, 0.0f, 1.0f, 0.1f );
		AddDebugVarX( "xCenterY", &xCenterY, 0.0f, 1.0f, 0.1f );
		AddDebugVarX( "xInnerRadius", &xInnerRadius, 0.0f, 10.0f, 0.1f );
		AddDebugVarX( "xOuterRadius", &xOuterRadius, 0.0f, 10.0f, 0.1f );
		AddDebugVarX( "xInverseArea", &xInverseArea, 0, 1, 1 );
		AddDebugVarX( "xAttackTime", &xAttackTime, 0.0f, 5.0f, 0.1f );
		AddDebugVarX( "xSustainTime", &xSustainTime, 0.0f, 100.0f, 0.1f );
		AddDebugVarX( "xDecayTime", &xDecayTime, 0.0f, 5.0f, 0.1f );
		AddDebugVarX( "xUseOmega", &xUseOmega, 0, 1, 1 );
		AddDebugVarX( "xFeedBack", &xFeedBack, 0, 1, 1 );
		AddDebugVarX( "xSigType", &xSigType, (int) SigGen::NONE, (int) SigGen::FASTSMOOTHNOISE, 1 );
		AddDebugVarX( "xSigFreq", &xSigFreq, 0.0f, 10.0f, 0.1f );
		AddDebugVarX( "xSigAmp", &xSigAmp, 0.0f, 10.0f, 0.01f );
		AddDebugVarX( "xCenterOnInitiator", &xCenterOnInitiator, 0, 1, 1 );
		debugMenu->Return();

		// y grid params
		debugMenu->AddMenuItem( "Y Grid" );
		debugMenu->Branch();

		debugMenu->AddMenuItem( "Refresh", 0, RefreshDistortion, this );
		AddDebugVarX( "yOn", &yOn, 0, 1, 1 );
		AddDebugVarY( "yCenterX", &yCenterX, 0.0f, 1.0f, 0.1f );
		AddDebugVarY( "yCenterY", &yCenterY, 0.0f, 1.0f, 0.1f );
		AddDebugVarY( "yInnerRadius", &yInnerRadius, 0.0f, 10.0f, 0.1f );
		AddDebugVarY( "yOuterRadius", &yOuterRadius, 0.0f, 10.0f, 0.1f );
		AddDebugVarY( "yInverseArea", &yInverseArea, 0, 1, 1 );
		AddDebugVarY( "yAttackTime", &yAttackTime, 0.0f, 5.0f, 0.1f );
		AddDebugVarY( "ySustainTime", &ySustainTime, 0.0f, 100.0f, 0.1f );
		AddDebugVarY( "yDecayTime", &yDecayTime, 0.0f, 5.0f, 0.1f );
		AddDebugVarY( "yUseOmega", &yUseOmega, 0, 1, 1 );
		AddDebugVarY( "yFeedBack", &yFeedBack, 0, 1, 1 );
		AddDebugVarY( "ySigType", &ySigType, (int) SigGen::NONE, (int) SigGen::FASTSMOOTHNOISE, 1 );
		AddDebugVarY( "ySigFreq", &ySigFreq, 0.0f, 10.0f, 0.1f );
		AddDebugVarY( "ySigAmp", &ySigAmp, 0.0f, 10.0f, 0.01f );
		AddDebugVarY( "yCenterOnInitiator", &yCenterOnInitiator, 0, 1, 1 );
		debugMenu->Return();

		// effects params
		debugMenu->AddMenuItem( "Effects" );
		debugMenu->Branch();

		debugMenu->AddMenuItem( "Refresh", 0, RefreshDistortion, this );
		AddDebugVar( "effectAttackTime", &effectAttackTime, 0.0f, 5.0f, 0.1f );
		AddDebugVar( "effectSustainTime", &effectSustainTime, 0.0f, 20.0f, 0.1f );
		AddDebugVar( "effectDecayTime", &effectDecayTime, 0.0f, 5.0f, 0.1f );
		AddDebugVar( "startOpacity", &startOpacity, 0.0f, 1.0f, 0.1f );
		AddDebugVar( "cStartColorR", &cStartColorR, 0x0, 0xff, 0x02 );
		AddDebugVar( "cStartColorG", &cStartColorG, 0x0, 0xff, 0x02 );
		AddDebugVar( "cStartColorB", &cStartColorB, 0x0, 0xff, 0x02 );
		AddDebugVar( "cStartColorA", &cStartColorA, 0x0, 0xff, 0x02 );
		AddDebugVar( "cpStartInnerRadius", &cpStartInnerRadius, 0.0f, 20.0f, 0.1f );
		AddDebugVar( "cpStartOuterRadius", &cpStartOuterRadius, 0.0f, 20.0f, 0.1f );
		AddDebugVar( "cpStartColorR", &cpStartColorR, 0x0, 0xff, 0x02 );
		AddDebugVar( "cpStartColorG", &cpStartColorG, 0x0, 0xff, 0x02 );
		AddDebugVar( "cpStartColorB", &cpStartColorB, 0x0, 0xff, 0x02 );
		AddDebugVar( "cpStartColorA", &cpStartColorA, 0x0, 0xff, 0x02 );
		debugMenu->Return();

		// save context
		char str[128];
		sprintf( str, "Save %s", context );
		debugMenu->AddMenuItem( str, 0, SaveDistortContext, this );
		debugMenu->Return();
	}
#endif
}

void CFSDEffect::SaveDistortContext( int command, PARAM param )
{
	CFSDEffect *pFSDE = (CFSDEffect *) param;
	g_console.SaveContext( pFSDE->currentContext );
}

void CFSDEffect::RefreshDistortion( int command, PARAM param )
{
	CFSDEffect *pFSDE = (CFSDEffect *) param;
	pFSDE->fRefreshTimer = 0.25f;
}

void CFSDEffect::AddDebugVar( const char *varName, int *var, int low, int high, int step )
{
	DebugMenu *debugMenu = GetDebugMenu();
	debugMenu->AddMenuItem( (char *)varName, var );
	debugMenu->SetRange( (float) low, (float) high );
	debugMenu->SetStep( (float) step );
}

void CFSDEffect::AddDebugVar( const char *varName, float *var, float low, float high, float step )
{
	DebugMenu *debugMenu = GetDebugMenu();
	debugMenu->AddMenuItem( (char *)varName, var );
	debugMenu->SetRange( low, high );
	debugMenu->SetStep( step );
}

void CFSDEffect::AddDebugVarX( const char *varName, int *var, int low, int high, int step )
{
	DebugMenu *debugMenu = GetDebugMenu();
	debugMenu->AddMenuItem( (char *)varName, var );
	debugMenu->SetRange( (float) low, (float) high );
	debugMenu->SetStep( (float) step );
}

void CFSDEffect::AddDebugVarX( const char *varName, float *var, float low, float high, float step )
{
	DebugMenu *debugMenu = GetDebugMenu();
	debugMenu->AddMenuItem( (char *)varName, var );
	debugMenu->SetRange( low, high );
	debugMenu->SetStep( step );
}

void CFSDEffect::AddDebugVarY( const char *varName, int *var, int low, int high, int step )
{
	DebugMenu *debugMenu = GetDebugMenu();
	debugMenu->AddMenuItem( (char *)varName, var );
	debugMenu->SetRange( (float) low, (float) high );
	debugMenu->SetStep( (float) step );
}

void CFSDEffect::AddDebugVarY( const char *varName, float *var, float low, float high, float step )
{
	DebugMenu *debugMenu = GetDebugMenu();
	debugMenu->AddMenuItem( (char *)varName, var );
	debugMenu->SetRange( low, high );
	debugMenu->SetStep( step );
}

void CFSDEffect::InitChanged( PARAM param )
{
	CFSDEffect *pFSDE = (CFSDEffect *) param;

	pFSDE->Calculate();

	FullScreenDistort::SetFilterColor( pFSDE->cColor.ABGR ); // this color supports overbrighting! so 0x80 is 1.0. (its in ABGR order)
	if ( pFSDE->bCenterOnScreen )
		FullScreenDistort::SetFilterColorPoint( pFSDE->cpCenterX, pFSDE->cpCenterY, pFSDE->temp_cpInnerRadius, pFSDE->temp_cpOuterRadius, pFSDE->cpColor.ABGR );
	FullScreenDistort::SetMode( (FullScreenDistort::MODE) pFSDE->mode );
	FullScreenDistort::SetOpacity( pFSDE->tempOpacity );
}

void CFSDEffect::XChanged( PARAM param )
{
	CFSDEffect *pFSDE = (CFSDEffect *) param;

	pFSDE->Calculate();

	FullScreenDistort::Remove( pFSDE->dHandle1 );
	pFSDE->dHandle1 = FullScreenDistort::Add( FullScreenDistort::X,
															pFSDE->xCenterX,
															pFSDE->xCenterY,
															pFSDE->temp_xInnerRadius,
															pFSDE->temp_xOuterRadius,
															(bool) (pFSDE->xInverseArea != 0),
															pFSDE->xAttackTime,
															pFSDE->xSustainTime,
															pFSDE->xDecayTime,
															(bool) (pFSDE->xUseOmega != 0),
															(bool) (pFSDE->xFeedBack != 0),
															new SigGen( (SigGen::TYPE) pFSDE->xSigType, pFSDE->xSigFreq, pFSDE->xSigAmp ) );
}

void CFSDEffect::YChanged( PARAM param )
{
	CFSDEffect *pFSDE = (CFSDEffect *) param;

	pFSDE->Calculate();

	FullScreenDistort::Remove( pFSDE->dHandle2 );
	pFSDE->dHandle2 = FullScreenDistort::Add( FullScreenDistort::Y,
															pFSDE->yCenterX,
															pFSDE->yCenterY,
															pFSDE->temp_yInnerRadius,
															pFSDE->temp_yOuterRadius,
															(bool) (pFSDE->yInverseArea != 0),
															pFSDE->yAttackTime,
															pFSDE->ySustainTime,
															pFSDE->yDecayTime,
															(bool) (pFSDE->yUseOmega != 0),
															(bool) (pFSDE->yFeedBack != 0),
															new SigGen( (SigGen::TYPE) pFSDE->ySigType, pFSDE->ySigFreq, pFSDE->ySigAmp ) );
}
