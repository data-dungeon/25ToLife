/***********************************/
/*                                 */
/* Particle.cpp                    */
/* Pigpen particle tool  12/10/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle class                  */
/*                                 */
/***********************************/

/* a particle is a graphic pointer and a bunch of behavior controls */

/******************* includes ****************************/

/*** pre-compiled headers through .h file (visual studio), can't be inside #if statement. ***/
/*** see .h file for more info.for codewarrior, pre-compiled header is handled through    ***/
/*** xcl compile/link interpreter, and doesn't need any explicit include                  ***/

#include <PigpenPCH.h>

/*** includes that don't belong in pre-compiled header (typically due to low frequency) ***/



/******************* defines *****************************/

#define DEFAULT_LIFETIME	3.0f	// in seconds

/******************* structures **************************/

/******************* local variables *********************/

int CParticle::nSurfaceCount = 0;
int CParticle::nPredefinedSurfaceCount = 0;
int CParticle::nEndOfLifeSurfaceCount = 0;
int CParticle::nDefaultCollisionSurfaceIndex = 0;
int *CParticle::pnSurfaceTranslation = NULL;

/******************* externs *****************************/

extern "C" void WriteLogFile(const char *pcFmt, ...);

/******************* class functions *********************/

/* this enables easy serialization of the CObArray-- you just call array's Serialize function and all the
particle records serialize themselves, in order-- slick */

IMPLEMENT_SERIAL(CParticle, CObject, 0)

/////////////////////////////////////////////////////////////////////////////
// CParticle construction

CParticle::CParticle()
{
	m_ulVersion = PARTICLE_VERSION;
	m_u32Flags = PART_MAINTAIN_ASPECT_RATIO;

	m_Name = "New";
	m_nUsedCount = -1;	// unknown status until analysis is done
	m_pEngineData = NULL;
	m_nExportDataSize = 0;

	/* display-related */

	m_pGraphic = NULL;
	m_nGraphicIndex = -1;
	m_nHardwiredGraphic = PARTICLE_HARDWIRED_GRAPHIC_NONE;
	m_u32RandomFlags = 0;
	m_u32CollideFlags = PART_COLLIDES_WORLD;
	m_u32Bitfields = 0;
	m_fFPS = DEFAULT_FPS;
	m_fFPSRandomPct = 0;
	m_fZBias = 0.0f;

	/* size */

	m_fXSize					= 1.0f;
	m_fXSizeRandomPct		= 0.0f;
	m_fYSize					= 1.0f;
	m_fYSizeRandomPct		= 0.0f;

	/* movement */

	m_fXPos					= 0.0f;
	m_fYPos					= 0.0f;
	m_fZPos					= 0.0f;
	m_fPosRandom			= 0.0f;
	m_fXVel					= 0.0f;
	m_fYVel					= 0.0f;
	m_fZVel					= 0.0f;
	m_fVelRandomPct		= 0.0f;
	m_fVelCone1				= 0.0f;
	m_fVelCone2				= 0.0f;
	m_fGravity				= 0.0f;
	m_fAccel					= 0.0f;
	m_fXRotVel				= 0.0f;
	m_fYRotVel				= 0.0f;
	m_fZRotVel				= 0.0f;
	m_fRotVelRandomPct	= 0.0f;
	m_fRotAccel				= 0.0f;
	m_u32Flags |= (PART_STANDARD_GRAVITY | PART_NO_DISTORTION);
	
	/* envelopes */

	m_nCurrentEnvelope = 0;
	m_u32Flags |= ENVELOPE_HIDE_ALL;

	/* colors for color pair color control */

	m_Color[0] = 0x00ffffff;	// white-- color at 100%
	m_Color[1] = 0x00000000;	// black-- color at 0%

	/* collision */

	m_fCollideBounceEnergy	= 0.5f;

	/* lifetime */

	m_fLifetime = DEFAULT_LIFETIME;
	m_fLifetimeRandomPct = 0;
	m_fLeash = 0.0f;
	m_fLeashRandomPct = 0;

	/* orbit */

	m_fOrbitRadius = 0.0f;
	m_fOrbitRadiusRandomPct = 0.0f;
	m_fOrbitRadiusVel = 0.0f;
	m_fOrbitRadiusVelRandomPct = 0.0f;
	m_fOrbitRotVel = 0.0f;
	m_fOrbitRotVelRandomPct = 0.0f;

	/* piggyback emitters */

	m_pPiggybackEmitter = NULL;
	m_nPiggybackEmitterIndex = -1;

	/* effect spawning-- one record per surface type in doc */

	for (int i = 0; i < nSurfaceCount; i++)
	{
		CSpawn *pSpawn = new CSpawn;
		if (i < nEndOfLifeSurfaceCount)
			pSpawn->SetFlags(SPAWN_END_OF_LIFE);				// end of life spawn records can be edited regardless of "particle collides" status
		if (i < nPredefinedSurfaceCount)
			pSpawn->ClearFlags(SPAWN_USE_DEFAULT_SURFACE);	// predefined surface spawn records do not have "use default" checked
		m_Spawn.Add(pSpawn);
	}
	m_pCurrentSpawn = NULL;
}
CParticle::~CParticle()
{
	while (m_Spawn.GetSize())
	{
		delete m_Spawn.GetAt(0);
		m_Spawn.RemoveAt(0);
	}
}

//xxx don't have "none" in spawn combo box, since we have "spawns" checkbox? or get rid of "spawns" checkbox?

/////////////////////////////////////////////////////////////////////////////
// CParticle serialization

void CParticle::Serialize(CArchive& ar)
{
	int i;

	if (ar.IsStoring())
	{
		ASSERT(m_ulVersion == PARTICLE_VERSION);
		ar << m_ulVersion;

		/* version 16-- collision flags */

		ar << m_u32CollideFlags;

		/* version 15-- nothing to do-- just a flag change */

		/* version 14 */

		ar << m_fZBias;

		/* version 13 */

		ar << m_nHardwiredGraphic;

		/* version 12 */

		ar << m_fOrbitRadius;
		ar << m_fOrbitRadiusRandomPct;
		ar << m_fOrbitRadiusVel;
		ar << m_fOrbitRadiusVelRandomPct;
		ar << m_fOrbitRotVel;
		ar << m_fOrbitRotVelRandomPct;

		/* version 11 */

		ar << m_nPiggybackEmitterIndex;

		/* version 10 */

		ar << GetSpawnCount();
		for (i = 0; i < GetSpawnCount(); i++)
			GetSpawn(i)->Serialize(ar);

		/* version 9 */

		ar << m_fLeash;
		ar << m_fLeashRandomPct;
		
		/* version 8-- obsolete, kept for compatibility (changed with version 10) */

		int nCauses = 0;
		ar << nCauses;
//		for (i = 0; i < SPAWN_CAUSE_COUNT; i++)
//			ar << m_nSpawnEffectIndex[i];

		/* version 7 */

		ar << m_fVelCone1;

		/* version 6 */

		int nObsolete = 0;
		ar << nObsolete;	// now obsolete, but remains for compatibility (changed with version 8)
		ar << m_fCollideBounceEnergy;
		ar << m_fObsoleteSpawnBounceEnergy;

		/* version 5 */

		ar << m_Color[0];
		ar << m_Color[1];

		/* version 4 */

		ar << m_fObsoletePreviewRate;	// now obsolete, but remains for compatibility

		/* version 3 */

		ar << m_fXPos;
		ar << m_fYPos;
		ar << m_fZPos;
		ar << m_fPosRandom;
		ar << m_fXSizeRandomPct;
		ar << m_fYSizeRandomPct;

		/* version 2 */

		ar << m_fXVel;
		ar << m_fYVel;
		ar << m_fZVel;
		ar << m_fVelRandomPct;
		ar << m_fVelCone2;
		ar << m_fGravity;
		ar << m_fAccel;
		ar << m_fXRotVel;
		ar << m_fYRotVel;
		ar << m_fZRotVel;
		ar << m_fRotVelRandomPct;
		ar << m_fRotAccel;

		/* version 1 */

		ar << m_fLifetime;
		ar << m_fLifetimeRandomPct;
	
		/* version 0 */

		ar << m_u32Flags;
		ar << m_Name;
		ar << m_nGraphicIndex;
		ar << m_u32RandomFlags;
		ar << m_u32Bitfields;
		ar << m_fFPS;
		ar << m_fFPSRandomPct;
		ar << m_fXSize;
		ar << m_fYSize;

		/* envelopes */

		int nEnvelopes = MAX_ENVELOPES;
		ar << nEnvelopes;
		ar	<< m_nCurrentEnvelope;
		for (i = 0; i < MAX_ENVELOPES; i++)
			m_Envelope[i].Serialize(ar);
	}
	else
	{
		ar >> m_ulVersion;
		if (m_ulVersion > PARTICLE_VERSION)
		{
			ASSERT(FALSE);	// shouldn't happen since later version should be caught by doc serialization
			if (PigpenApp.IsCommandLineMode())
				PigpenApp.SetReturnCode(CPigpenApp::RETURN_CODE_INVALID_VERSION);
			else
				AfxMessageBox("Loading particles.\nThis executable cannot load this version of Pigpen file.\nIt was saved by a later version of the executable.\nPlease update your Pigpen.exe.", MB_OK | MB_ICONEXCLAMATION);
			AfxThrowArchiveException(CArchiveException::generic);	// abort
		}

		/* as we add new versions, process latest version and then fall through to earlier versions */

		switch(m_ulVersion)
		{
		case 16:
			ar >> m_u32CollideFlags;

			/* fall through */
		case 15:
			/* nothing to do-- just a flag change */

			/* fall through */
		case 14:
			ar >> m_fZBias;

			/* fall through */
		case 13:
			ar >> m_nHardwiredGraphic;

			/* fall through */
		case 12:
			ar >> m_fOrbitRadius;
			ar >> m_fOrbitRadiusRandomPct;
			ar >> m_fOrbitRadiusVel;
			ar >> m_fOrbitRadiusVelRandomPct;
			ar >> m_fOrbitRotVel;
			ar >> m_fOrbitRotVelRandomPct;

			/* fall through */
		case 11:
			ar >> m_nPiggybackEmitterIndex;

			/* fall through */
		case 10:
			int nSpawnCount;
			ar >> nSpawnCount;

			/* because pre-defined surface types can be added outside of the file save/load process, there */
			/* may be more surface types than spawn records in the file being loaded. use a translation table */
			/* to figure out what goes where. see comment in CPipgenDoc::Serialize where m_pnSurfaceTranslation */
			/* is malloc'd */

			for (i = 0; i < nSpawnCount; i++)
				GetSpawn(pnSurfaceTranslation[i])->Serialize(ar);

			/* fall through */
		case 9:
			ar >> m_fLeash;
			ar >> m_fLeashRandomPct;

			/* fall through */
		case 8:
			int nCauses;
			ar >> nCauses;

			/* now obsolete, replaced by version 10. but for pre-version-10 files, there was just one spawn */
			/* cause-- end of life. put this into first spawn record to preserve as much info as possible */

			for (i = 0; i < nCauses; i++)
			{
				ASSERT(m_ulVersion < 10);	// versions 10 and greater write out 0 for nCauses
				ASSERT(nCauses == 1);		// pre-version-10 only had one spawn cause-- end of life
				int nObsolete;
				ar >> nObsolete;
				GetSpawn(0)->SetEffectIndex(nObsolete);	// put index into end-of-life spawn record
			}

//			if (nCauses > SPAWN_CAUSE_COUNT)
//				nCauses = SPAWN_CAUSE_COUNT;	// in case stored count is larger than current SPAWN_CAUSE_COUNT
//			for (i = 0; i < nCauses; i++)
//				ar >> m_nSpawnEffectIndex[i];
//			for (i = nCauses; i < SPAWN_CAUSE_COUNT; i++)
//				m_nSpawnEffectIndex[i] = -1;	// in case stored count is smaller than current SPAWN_CAUSE_COUNT

			/* fall through */
		case 7:
			ar >> m_fVelCone1;

			/* fall through */
		case 6:
			ar >> i;	//superseded by version 8 array processing
			ar >> m_fCollideBounceEnergy;
			ar >> m_fObsoleteSpawnBounceEnergy;

			/* fall through */
		case 5:
			ar >> m_Color[0];
			ar >> m_Color[1];

			/* fall through */
		case 4:
			ar >> m_fObsoletePreviewRate;

			/* fall through */
		case 3:
			ar >> m_fXPos;
			ar >> m_fYPos;
			ar >> m_fZPos;
			ar >> m_fPosRandom;
			ar >> m_fXSizeRandomPct;
			ar >> m_fYSizeRandomPct;

			/* fall through */
		case 2:
			ar >> m_fXVel;
			ar >> m_fYVel;
			ar >> m_fZVel;
			ar >> m_fVelRandomPct;
			ar >> m_fVelCone2;
			ar >> m_fGravity;
			ar >> m_fAccel;
			ar >> m_fXRotVel;
			ar >> m_fYRotVel;
			ar >> m_fZRotVel;
			ar >> m_fRotVelRandomPct;
			ar >> m_fRotAccel;

			/* fall through */
		case 1:
			ar >> m_fLifetime;
			ar >> m_fLifetimeRandomPct;
	
			/* fall through */
		case 0:
			/* version 0 */

			ar >> m_u32Flags;
			ar >> m_Name;
			ar >> m_nGraphicIndex;
			ar >> m_u32RandomFlags;
			ar >> m_u32Bitfields;
			ar >> m_fFPS;
			ar >> m_fFPSRandomPct;
			ar >> m_fXSize;
			ar >> m_fYSize;

			/* envelopes */

			int nEnvelopes;
			ar >> nEnvelopes;
			ar	>> m_nCurrentEnvelope;
			for (i = 0; i < nEnvelopes; i++)
				m_Envelope[i].Serialize(ar);
			break;
		default:
			ASSERT(FALSE);
			break;
		}

		/* clear associated flag on load-- it's temporary */

		m_u32Flags &= ~PART_ASSOCIATED;

		/* now using latest version */

		m_ulLoadVersion = m_ulVersion;	// save version from load, for possible fix-ups
		m_ulVersion = PARTICLE_VERSION;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CParticle implementation

CString& CParticle::GetGraphicName(void)
{
	static CString NameNone("None");

	if (m_pGraphic == NULL)
		return(NameNone);
	else
		return(m_pGraphic->GetName());
}

/* fill in data for export. return size of data, 0 if error or overflow */

int CParticle::FillExportBuffer(char *pBuffer, int nMaxSize, bool bPreview)
{
	/* should have valid data at this point */

	ASSERT(GetExportStatus() == EXPORT_STATUS_VALID);
	ASSERT((m_pGraphic) || (GetHardwiredGraphic() != PARTICLE_HARDWIRED_GRAPHIC_NONE));
	if (m_pGraphic)
		ASSERT(m_pGraphic->GetImageCount() != 0);

	/* point to buffer to fill, clear data to make sure spaces between packed fields is 0 */

	ts_ParticleSource *pParticleSource = (ts_ParticleSource *) pBuffer;
	memset(pParticleSource, 0, nMaxSize);

	/* animation speed */

	pParticleSource->u8FPS = (u8) m_fFPS;
	pParticleSource->u8FPSRandomPct = (u8) m_fFPSRandomPct;

	/* lifetime */

	pParticleSource->u8Lifetime = CONVERT_FLOAT_TO_U8(m_fLifetime, MAX_P_LIFETIME);
	pParticleSource->u8LifetimeRandomPct = (u8) m_fLifetimeRandomPct;

	/* leash */

	if (IsFlagSet(PART_DIE_AT_END_OF_LEASH))
	{
		pParticleSource->u16Leash = CONVERT_FLOAT_TO_U16(m_fLeash, MAX_P_LEASH);
		pParticleSource->u8LeashRandomPct = (u8) m_fLeashRandomPct;
	}
	else
	{
		pParticleSource->u16Leash = 0;
		pParticleSource->u8LeashRandomPct = 0;
	}

	/* size */

	pParticleSource->u16XSize = CONVERT_FLOAT_TO_U16(m_fXSize, MAX_P_SIZE);
	pParticleSource->u8XSizeRandomPct = (u8) m_fXSizeRandomPct;
	pParticleSource->u16YSize = CONVERT_FLOAT_TO_U16(m_fYSize, MAX_P_SIZE);
	pParticleSource->u8YSizeRandomPct = (u8) m_fYSizeRandomPct;
	pParticleSource->u8YSizeEnvelope = Y_SIZE_ENVELOPE;
	if (IsMaintainAspectRatio())
	{
		pParticleSource->u16YSize = pParticleSource->u16XSize;
		pParticleSource->u8YSizeRandomPct = pParticleSource->u8XSizeRandomPct;
		pParticleSource->u8YSizeEnvelope = X_SIZE_ENVELOPE;
	}

	/* starting and ending image indices */

	if ((m_pGraphic) && (m_pGraphic->GetExportStatus() == EXPORT_STATUS_UNKNOWN))
	{
		ASSERT(bPreview);	// this should only happen in preview mode when changing a particle's graphic
		return(0);
	}
	if (m_pGraphic)
	{
//WriteLogFile("particle %s, graphic %s, first index = %d", GetName().GetBuffer(10), m_pGraphic->GetName().GetBuffer(10), m_pGraphic->GetImage(0)->GetTempIndex());
		pParticleSource->u16FirstIndex = m_pGraphic->GetImage(0)->GetTempIndex();
		pParticleSource->u16LastIndex = m_pGraphic->GetImage(m_pGraphic->GetImageCount() - 1)->GetTempIndex();
	}
	else
	{
		switch(GetHardwiredGraphic())
		{
		case PARTICLE_HARDWIRED_GRAPHIC_FRONT_BUFFER:
//WriteLogFile("particle %s, front buffer", GetName().GetBuffer(10));
			pParticleSource->u16FirstIndex = BIND_FRONT_BUFFER;
			pParticleSource->u16LastIndex = BIND_FRONT_BUFFER;
			break;
		case PARTICLE_HARDWIRED_GRAPHIC_BACK_BUFFER:
//WriteLogFile("particle %s, back buffer", GetName().GetBuffer(10));
			pParticleSource->u16FirstIndex = BIND_BACK_BUFFER;
			pParticleSource->u16LastIndex = BIND_BACK_BUFFER;
			break;
		default:
			ASSERT(false);	// unknown case
			return(0);
		}
	}
	ASSERT(pParticleSource->u16LastIndex >= pParticleSource->u16FirstIndex);

	/* position */

	pParticleSource->vInitialPos.Set(m_fXPos, m_fYPos, m_fZPos);
	pParticleSource->u16PosRandom = CONVERT_FLOAT_TO_U16(m_fPosRandom, MAX_P_POS_RANDOM);

	/* velocity */

	Vector3 vInitialVel;
	if (IsBullet())
	{
		vInitialVel.Clear();

		/* camera looks down negative z-axis, bullet velocity should be negative */

		if (m_fZVel < 0.0f)
			vInitialVel.z(m_fZVel);
		else
			vInitialVel.z(-m_fZVel);
	}
	else
		vInitialVel.Set(m_fXVel, m_fYVel, m_fZVel);

	/* convert velocity to unit vector and magnitude. this is for space savings and easier randomization */

	Vector3 vUnitVector = vInitialVel;
	if ((vUnitVector.x() != 0.0f) || (vUnitVector.y() != 0.0f) || (vUnitVector.z() != 0.0f))
		vUnitVector.Normalize();
	pParticleSource->s8InitialVelX = CONVERT_FLOAT_TO_S8(vUnitVector.x(), 1.0f);
	pParticleSource->s8InitialVelY = CONVERT_FLOAT_TO_S8(vUnitVector.y(), 1.0f);
	pParticleSource->s8InitialVelZ = CONVERT_FLOAT_TO_S8(vUnitVector.z(), 1.0f);
	pParticleSource->fInitialVel = vInitialVel.Length();

	/* velocty randomness. velocity gets converted to direction and magnitude. RandomPct affects */
	/* magnitude, cone affects direction */

	pParticleSource->u8VelRandomPct = (u8) m_fVelRandomPct;
	pParticleSource->u8VelConeStart = CONVERT_FLOAT_TO_U8(Math::Deg2Rad(m_fVelCone1), MAX_P_VEL_CONE);
	ASSERT(m_fVelCone2 - m_fVelCone1 >= 0.0f);	// delta should be >= 0
	pParticleSource->u8VelConeDelta = CONVERT_FLOAT_TO_U8(Math::Deg2Rad(m_fVelCone2 - m_fVelCone1), MAX_P_VEL_CONE);

	/* gravity and acceleration */

	pParticleSource->fGravity = m_fGravity;
	pParticleSource->fAccel = m_fAccel;

	/* rotation */

	pParticleSource->vInitialRotVelRadians.Set(Math::Deg2Rad(m_fXRotVel), Math::Deg2Rad(m_fYRotVel), Math::Deg2Rad(m_fZRotVel));
	if (m_fRotVelRandomPct < 0.0f)
		pParticleSource->u8RotVelRandomPct = RANDOM_CENTER_ON_ZERO;
	else
		pParticleSource->u8RotVelRandomPct = (u8) m_fRotVelRandomPct;
	pParticleSource->fRotAccelRadians = Math::Deg2Rad(m_fRotAccel);

	/* orbit */

	pParticleSource->u8OrbitRadius = CONVERT_FLOAT_TO_U8(m_fOrbitRadius, MAX_P_ORBIT_RADIUS);
	pParticleSource->u8OrbitRadiusRandomPct = (u8) m_fOrbitRadiusRandomPct;
	pParticleSource->s8OrbitRadiusVel = CONVERT_FLOAT_TO_S8(m_fOrbitRadiusVel, MAX_P_ORBIT_RADIUS_VEL);
	pParticleSource->u8OrbitRadiusVelRandomPct = (u8) m_fOrbitRadiusVelRandomPct;
	pParticleSource->s16OrbitRotVel = CONVERT_FLOAT_TO_S16(Math::Deg2Rad(m_fOrbitRotVel), MAX_P_ORBIT_ROTVEL);
	pParticleSource->u8OrbitRotVelRandomPct = (u8) m_fOrbitRotVelRandomPct;

	/* envelopes */

	for (int i = 0; i < MAX_ENVELOPES; i++)
	{
		pParticleSource->Envelope[i].fSplinePoint0Y = m_Envelope[i].m_ControlPoints[SPLINE_POINT0].y();
		pParticleSource->Envelope[i].SplinePoint1 = m_Envelope[i].m_ControlPoints[SPLINE_POINT1];
		pParticleSource->Envelope[i].SplinePoint2 = m_Envelope[i].m_ControlPoints[SPLINE_POINT2];
		pParticleSource->Envelope[i].fSplinePoint3Y = m_Envelope[i].m_ControlPoints[SPLINE_POINT3].y();
		pParticleSource->Envelope[i].fFloorY = m_Envelope[i].m_ControlPoints[FLOOR_POINT].y();
		pParticleSource->Envelope[i].fCeilingY = m_Envelope[i].m_ControlPoints[CEILING_POINT].y();
		pParticleSource->Envelope[i].u8Type = m_Envelope[i].GetType();

		/* if clip lines are at same position, treat as a flat line */

//yuch-- check converted 8 bit values for ==, not float values
		if (pParticleSource->Envelope[i].fFloorY == pParticleSource->Envelope[i].fCeilingY)
			pParticleSource->Envelope[i].u8Type = FLAT_ENVELOPE;

		/* if endpoints for straight line are the same, treat as flat */

//yuch-- check converted 16 bit values for ==, not float values
		if ((pParticleSource->Envelope[i].u8Type == STRAIGHT_ENVELOPE) &&
			 (pParticleSource->Envelope[i].fSplinePoint0Y == pParticleSource->Envelope[i].fSplinePoint3Y))
			pParticleSource->Envelope[i].u8Type = FLAT_ENVELOPE;

		/* for flat line, envelope evaluator just returns fSplinePoint0Y, so make sure it's clipped */
		/* correctly */

		if (pParticleSource->Envelope[i].u8Type == FLAT_ENVELOPE)
		{
			if (pParticleSource->Envelope[i].fSplinePoint0Y > pParticleSource->Envelope[i].fCeilingY)
				pParticleSource->Envelope[i].fSplinePoint0Y = pParticleSource->Envelope[i].fCeilingY;
			else if (pParticleSource->Envelope[i].fSplinePoint0Y < pParticleSource->Envelope[i].fFloorY)
				pParticleSource->Envelope[i].fSplinePoint0Y = pParticleSource->Envelope[i].fFloorY;
		}

		/* for spline, "bias handle" control points y-position is adjusted to give greater range */

		if (pParticleSource->Envelope[i].u8Type == SPLINE_ENVELOPE)
		{
			pParticleSource->Envelope[i].SplinePoint1.y(m_Envelope[i].AdjustSplinePoint(pParticleSource->Envelope[i].SplinePoint1.y()));
			pParticleSource->Envelope[i].SplinePoint2.y(m_Envelope[i].AdjustSplinePoint(pParticleSource->Envelope[i].SplinePoint2.y()));
		}
	}

	/* colors for color pair color control */

	if (GetColorControl() == PART_COLOR_RGB)
		memset(pParticleSource->Color, 0, 2 * sizeof(ts_bRGBA));	// colors not used in rgb mode
	else
	{
		for (int j = 0; j < 2; j++)
		{
			pParticleSource->Color[j].R = (float) (m_Color[j] & 0xff);
			pParticleSource->Color[j].G = (float) ((m_Color[j] >> 8) & 0xff);
			pParticleSource->Color[j].B = (float) ((m_Color[j] >> 16) & 0xff);

			/* cut in half for ps2 use */

			if (nHackExportMode == EXPORT_PS2)
			{
				pParticleSource->Color[j].R /= 2;
				pParticleSource->Color[j].G /= 2;
				pParticleSource->Color[j].B /= 2;
			}
		}
	}

	/* collision */

	pParticleSource->u8CollideBounceEnergy = CONVERT_FLOAT_TO_U8(m_fCollideBounceEnergy, MAX_P_BOUNCE);
 
	/* consolidate flags */

	pParticleSource->u32Flags = 0;
	m_u32TestFlags = 0;	// for error checking
	AddFlag(m_u32Flags, pParticleSource, PART_RANDOM_POS_PLANE, PARTICLE_RANDOM_POS_PLANE);
	AddFlag(m_u32Flags, pParticleSource, PART_BULLET, PARTICLE_BULLET);
	AddFlag(m_u32Flags, pParticleSource, PART_STANDARD_GRAVITY, PARTICLE_STANDARD_GRAVITY);
	AddFlag(m_u32Flags, pParticleSource, PART_INIT_ROT_RANDOM, PARTICLE_INIT_ROT_RANDOM);
	AddFlag(m_u32CollideFlags, pParticleSource, PART_COLLIDE_STOP_ROTATING, PARTICLE_COLLIDE_STOP_ROTATING);
	AddFlag(m_u32CollideFlags, pParticleSource, PART_COLLIDE_STOP_ANIMATING, PARTICLE_COLLIDE_STOP_ANIMATING);
	AddFlag(m_u32Flags, pParticleSource, PART_ALIGN_TO_VELOCITY, PARTICLE_SOURCE_ALIGN_TO_VELOCITY);
	AddFlag(m_u32Flags, pParticleSource, PART_PERPENDICULAR_TO_VELOCITY, PARTICLE_PERPENDICULAR_TO_VELOCITY);
	AddFlag(m_u32Flags, pParticleSource, PART_LIGHTED, PARTICLE_LIGHTED);
	AddFlag(m_u32Flags, pParticleSource, PART_NO_DISTORTION, PARTICLE_NO_DISTORTION);
	AddFlag(m_u32Flags, pParticleSource, PART_ZBUFFER_WRITE, PARTICLE_ZBUFFER_WRITE);
	AddFlag(m_u32Flags, pParticleSource, PART_NO_VIEW_CULL, PARTICLE_NO_VIEW_CULL);
	AddFlag(m_u32Flags, pParticleSource, PART_FORCEFIELDS_AFFECT, PARTICLE_FORCEFIELDS_AFFECT);
	AddFlag(m_u32Flags, pParticleSource, PART_ADDITIVE_RENDER, PARTICLE_ADDITIVE_RENDER);

	/* some flags are disallowed by other settings-- pigpen tries to keep the flags in a valid state, but no harm */
	/* confirming here */

	if ((GetOrient() != PART_ORIENT_FREE) || (IsAlignToVelocity() == false))
		pParticleSource->u32Flags &= ~PARTICLE_PERPENDICULAR_TO_VELOCITY;

	/* some flags are set by analysis, not by user */

	if ((GetGraphic()) && (GetGraphic()->AllImagesSingleBitAlpha()))
		AddFlag(1, pParticleSource, 1, PARTICLE_ALPHA_REF);	// passing 1's enables export flag bit for setting-- there is no corresponding internal (PART_*) flag bit
	if ((m_pGraphic == NULL) &&
		 ((GetHardwiredGraphic() == PARTICLE_HARDWIRED_GRAPHIC_FRONT_BUFFER) ||
		  (GetHardwiredGraphic() == PARTICLE_HARDWIRED_GRAPHIC_BACK_BUFFER)))
	{
		AddFlag(1, pParticleSource, 1, PARTICLE_RENDER_FOR_DISTORT);	// passing 1's enables export flag bit for setting-- there is no corresponding internal (PART_*) flag bit
	}
	
	/* special cases */

	if (pParticleSource->u32Flags & PARTICLE_PERPENDICULAR_TO_VELOCITY)
		pParticleSource->u32Flags &= ~PARTICLE_SOURCE_ALIGN_TO_VELOCITY;	// PARTICLE_PERPENDICULAR_TO_VELOCITY is enough, don't want ps2 vu code to act on align-to-vel flag

	/* random flags */

	if (pParticleSource->u8FPSRandomPct != 0)	// don't bother adding flag if no randomness
		AddFlag(m_u32RandomFlags, pParticleSource, PART_FPS_CONT_RANDOM, PARTICLE_FPS_CONT_RANDOM);
	if (IsMaintainAspectRatio())	// don't bother adding flag if not maintaining aspect ratio
		AddFlag(m_u32RandomFlags, pParticleSource, PART_RANDOM_SIZE_MAINTAIN_ASPECT_RATIO, PARTICLE_RANDOM_SIZE_MAINTAIN_ASPECT_RATIO);
	AddFlag(m_u32RandomFlags, pParticleSource, PART_RANDOM_START_FRAME, PARTICLE_RANDOM_START_FRAME);
	AddFlag(m_u32RandomFlags, pParticleSource, PART_ORBIT_INIT_ROT_RANDOM, PARTICLE_ORBIT_INIT_ROT_RANDOM);

	/* bitfields */

	AddBitfield(pParticleSource, PART_ORIENT_MASK, PART_ORIENT_SHIFT, PARTICLE_ORIENT_MASK, PARTICLE_ORIENT_SHIFT);
	AddBitfield(pParticleSource, PART_ANIM_MASK, PART_ANIM_SHIFT, PARTICLE_ANIM_MASK, PARTICLE_ANIM_SHIFT);
	AddBitfield(pParticleSource, PART_COLOR_MASK, PART_COLOR_SHIFT, PARTICLE_COLOR_MASK, PARTICLE_COLOR_SHIFT);
	AddBitfield(pParticleSource, PART_COLLIDE_MASK, PART_COLLIDE_SHIFT, PARTICLE_COLLIDE_MASK, PARTICLE_COLLIDE_SHIFT);
	AddBitfield(pParticleSource, PART_EJECT_MASK, PART_EJECT_SHIFT, PARTICLE_EJECT_MASK, PARTICLE_EJECT_SHIFT);
	AddBitfield(pParticleSource, PART_ORBIT_PLANE_MASK, PART_ORBIT_PLANE_SHIFT, PARTICLE_ORBIT_PLANE_MASK, PARTICLE_ORBIT_PLANE_SHIFT);

	/* piggyback emitter */

	pParticleSource->u16PiggybackEmitterIndex = NO_PIGGYBACK_EMITTER;
	if ((HasPiggybackEmitter()) && (m_pPiggybackEmitter))
	{
		if (m_pPiggybackEmitter->GetExportStatus() == EXPORT_STATUS_UNKNOWN)
		{
			ASSERT(bPreview);	// this should only happen in preview mode when changing a particle's graphic
			return(0);			// need to re-build preview .dbl
		}

		/* record index. it is possible that emitter is empty, so its export status may be invalid */

		if (m_pPiggybackEmitter->GetExportStatus() == EXPORT_STATUS_VALID)
			pParticleSource->u16PiggybackEmitterIndex = m_pPiggybackEmitter->GetTempIndex();
	}

	/* effect spawning */
	
	if (ExportSpawnEffects(pParticleSource, bPreview) == false)
	{
		ASSERT(bPreview);	// this should only happen in preview mode when changing a particle's graphic
		return(0);			// need to re-build preview .dbl
	}

	/* collision detection settings */

	pParticleSource->u8CollisionFlags = 0;
	if (Collides())
	{
		if (CollidesWithWorld())
			pParticleSource->u8CollisionFlags |= PARTICLE_COLLIDE_WORLD;
		if (CollidesWithActors())
			pParticleSource->u8CollisionFlags |= PARTICLE_COLLIDE_ACTORS;
		if (CollidesWithWater())
			pParticleSource->u8CollisionFlags |= PARTICLE_COLLIDE_WATER;
		if (IsCollideFlagSet(PART_COLLIDE_CREATE_WATER_SPRITE))
			pParticleSource->u8CollisionFlags |= PARTICLE_COLLIDE_CREATE_WATER_SPRITE;
	}

	/* floats on water overrides collision types-- a particle that floats on water just finds the water height at */
	/* it's x/z position in the world */

	if (IsFlagSet(PART_FLOATS_ON_WATER))
		pParticleSource->u8CollisionFlags = PARTICLE_FLOAT_ON_WATER;

	/* water flags not allowed in preview-- we have no water to test them against */

	if (bPreview)
		pParticleSource->u8CollisionFlags &= ~(PARTICLE_COLLIDE_WATER | PARTICLE_FLOAT_ON_WATER);

	/* z-bias for rendering */

	pParticleSource->s16ZBias = CONVERT_FLOAT_TO_S16(m_fZBias, MAX_P_ZBIAS);

	/* align-to-velocity flag overrides rotation values */

	if (IsAlignToVelocity())
	{
		pParticleSource->vInitialRotVelRadians.Clear();
		pParticleSource->u8RotVelRandomPct = 0;
		pParticleSource->fRotAccelRadians = 0.0f;
		pParticleSource->u32Flags &= ~PARTICLE_INIT_ROT_RANDOM;
	}

	/* calculate length of record */

	u16 u16RecordSize = sizeof(ts_ParticleSource);
	if (pParticleSource->u16SpawnEffectCount > 1)
		u16RecordSize += (pParticleSource->u16SpawnEffectCount - 1) * sizeof(ts_SpawnEffect);	// "- 1" because SpawnEffect[1] array is already in ts_ParticleSource
	ASSERT(u16RecordSize <= nMaxSize);

	/* maybe endian swap it. swapping of SpawnEffect array is not handled by swap routine since it is variable length. */
	/* do it first since u16SpawnEffectCount may get swapped */

	for (i = 0; i < pParticleSource->u16SpawnEffectCount; i++)
	{
		ts_SpawnEffect *pSpawnEffectSwapped = DBLUtil__SwapEndian(&pParticleSource->SpawnEffect[i]);
		if (pSpawnEffectSwapped != &pParticleSource->SpawnEffect[i])
			memcpy(&pParticleSource->SpawnEffect[i], pSpawnEffectSwapped, sizeof(ts_SpawnEffect));
	}
	ts_ParticleSource *pParticleSourceSwapped = DBLUtil__SwapEndian(pParticleSource);
	if (pParticleSourceSwapped != pParticleSource)
		memcpy(pParticleSource, pParticleSourceSwapped, sizeof(ts_ParticleSource) - sizeof(ts_SpawnEffect));	// if swapped, copy swapped version back into export buffer

	/*** can't refer to contents of pParticleSource after this point since it may be endian-swapped ***/

	/* record and return size of data that got filled in */

	SetExportDataSize(u16RecordSize);
	return(GetExportDataSize());
}

/* export spawn effects to export record */

bool CParticle::ExportSpawnEffects(ts_ParticleSource *pParticleSource, bool bPreview)
{
	/* analyze spawn effects */

	bool bSpawnsAtEndOfLife = false;
	bool bSpawnsOnCollision = false;
	for (int i = 0; i < GetSpawnCount(); i++)
	{
		CSpawn *pSpawn = GetSpawn(i);

		/* spawn record is active if:

			it has a spawn effect assigned, and
			it is not marked "use default" (pre-defined surface types do not use default), and
			it is marked "spawns", and
			it is either an end-of-life spawn OR the particle is marked as "particle collides" */

		if (pSpawn->ValidForExport())
		{
			if (pSpawn->EndOfLife())
				bSpawnsAtEndOfLife = true;
			else if (CollidesValid())
				bSpawnsOnCollision = true;
		}
	}

	/* nothing yet */

	pParticleSource->u16SpawnEffectCount = 0;
	pParticleSource->u16EndOfLifeSpawnIndex = 0;

	/* if previewing, always export all spawn effects, starting at 0-- this is so that we can link up */
	/* to collision surface choice in preview so user can test different effects, and also to prevent */
	/* buffer overruns during real-time preview updating */

	bool bResult;
	if (bPreview)
	{
		bSpawnsAtEndOfLife = true;
		bSpawnsOnCollision = true;

		/* for .dbl output, collision surface export skips pre-defined surfaces so that we can use */
		/* 0-based collision material index to access array. for preview, we want export order to */
		/* correspond to surface list, starting after end-of-life effects, so export pre-defined, */
		/* non-end-of-life, spawn effects (like default collision) first */

		for (int i = nEndOfLifeSurfaceCount; i < nPredefinedSurfaceCount; i++)
		{
			bResult = ExportSpawnEffect(&pParticleSource->SpawnEffect[pParticleSource->u16SpawnEffectCount], GetSpawn(i));
			if (bResult == false)
				return(false);
			pParticleSource->u16SpawnEffectCount++;
		}
	}

	/* if there are collision spawns, right out a spawn effect record for each non-pre-defined surface type. if */
	/* a surface is not marked as active, use default collision surface spawn effect data */

	if (bSpawnsOnCollision)
	{
		for (int i = nPredefinedSurfaceCount; i < nSurfaceCount; i++)
		{
			CSpawn *pSpawn = GetSpawn(i);
			if (pSpawn->UseDefault())
				bResult = ExportSpawnEffect(&pParticleSource->SpawnEffect[pParticleSource->u16SpawnEffectCount], GetSpawn(nDefaultCollisionSurfaceIndex));
			else
				bResult = ExportSpawnEffect(&pParticleSource->SpawnEffect[pParticleSource->u16SpawnEffectCount], pSpawn);
			if (bResult == false)
				return(false);
			pParticleSource->u16SpawnEffectCount++;
		}
	}

	/* add end-of-life spawns */

	if (bSpawnsAtEndOfLife)
	{
		pParticleSource->u16EndOfLifeSpawnIndex = pParticleSource->u16SpawnEffectCount;
		for (int i = 0; i < nEndOfLifeSurfaceCount; i++)
		{
			bResult = ExportSpawnEffect(&pParticleSource->SpawnEffect[pParticleSource->u16SpawnEffectCount], GetSpawn(i));
			if (bResult == false)
				return(false);
			pParticleSource->u16SpawnEffectCount++;
		}
	}

	/* success */

	return(true);
}

/* fill in a single spawn effect record */

bool CParticle::ExportSpawnEffect(ts_SpawnEffect *pSpawnEffect, CSpawn *pSpawn)
{
	memset(pSpawnEffect, 0, sizeof(ts_SpawnEffect));
	if (pSpawn->ValidForExport())
	{
		/* preview check */

		if (pSpawn->GetEffect()->GetExportStatus() == EXPORT_STATUS_UNKNOWN)
			return(false);		// need to re-build preview .dbl

		/* fill in record */

		pSpawnEffect->u16Effect = pSpawn->GetEffect()->GetTempIndex();
		pSpawnEffect->u8Flags |= SPAWN_ACTIVE;
		pSpawnEffect->u8Flags |= ((pSpawn->m_u32Bitfields & SPAWN_TYPE_MASK) >> SPAWN_TYPE_SHIFT) << SPAWN_SHIFT;
		pSpawnEffect->u8BounceEnergy = CONVERT_FLOAT_TO_U8(pSpawn->GetBounceEnergy(), MAX_P_BOUNCE);
	}

	/* success. if not active, or no effect, SPAWN_ACTIVE bit will not be set, and game engine will ignore spawn request */

	return(true);
}

/* add a flag bit to the export flags field, with some error checking */

void CParticle::AddFlag(u32 u32Flags, ts_ParticleSource *pParticleSource, u32 u32InternalFlag, u32 u32ExportFlag)
{
	/* this bit should be unique, of course, so make sure nothing else sets it */

	ASSERT((m_u32TestFlags & u32ExportFlag) == 0);	// if this hits, there is overlap in export flag bits defined in g_ParticleDB.h
	m_u32TestFlags |= u32ExportFlag;
	if (u32Flags & u32InternalFlag)
		pParticleSource->u32Flags |= u32ExportFlag;
}

/* add a bitfield to the export flags field, with some error checking */

void CParticle::AddBitfield(ts_ParticleSource *pParticleSource, u32 u32InternalMask, u32 u32InternalShift, u32 u32ExportMask, u32 u32ExportShift)
{
	/* none of bits used by this bitfield should have been set yet */

	ASSERT((m_u32TestFlags & u32ExportMask) == 0);	// if this hits, there is overlap in export flag bits defined in g_ParticleDB.h
	m_u32TestFlags |= u32ExportMask;

	/* masks should be the same */

	ASSERT((u32InternalMask >> u32InternalShift) == (u32ExportMask >> u32ExportShift));

	/* add the bitfield */

	pParticleSource->u32Flags |= ((m_u32Bitfields & u32InternalMask) >> u32InternalShift) << u32ExportShift;
}

/* determine if particle is valid for export. an invalid particle points to a NULL or an empty graphic */

int CParticle::AnalyzeForExport(bool bPreview)
{
	if (GetExportStatus() == EXPORT_STATUS_UNKNOWN)
	{
		/* analyze data */

		bool bValid = false;
		if (m_pGraphic)
			bValid = (m_pGraphic->AnalyzeForExport() == EXPORT_STATUS_VALID);		// valid image
		else
			bValid = (GetHardwiredGraphic() != PARTICLE_HARDWIRED_GRAPHIC_NONE);	// valid special graphic
		if (bValid)
		{
			SetExportStatus(EXPORT_STATUS_VALID);
			IncUsedCount();

			/* particle is valid, analyze effects marked for spawning */

			for (int i = 0; i < GetSpawnCount(); i++)
			{
				if (GetSpawn(i)->ValidForExport())
					GetSpawn(i)->GetEffect()->AnalyzeForExport(bPreview);
			}

			/* in preview mode, we don't export all records for better speed. but since a particle */
			/* can spawn an emitter too, we have to wrap trickle-down analysis back to emitters (and */
			/* from emitters to effects). under normal .dbl export, this isn't an issue because all valid */
			/* effects and emitters will be exported. but for preview mode, trickle down */

			if ((bPreview) && (HasPiggybackEmitter()) && (GetPiggybackEmitter()))
				GetPiggybackEmitter()->AnalyzeForExport(bPreview);
		}
		else
		{
			char acMessage[100];
			sprintf(acMessage, "Particle: %s", GetName().GetBuffer(10));
			AddConditionalInfoToErrorLog(acMessage);
			HANDLE_ERROR(RETURN_EMPTY_PARTICLE);
			SetExportStatus(EXPORT_STATUS_INVALID);
		}
	}
	return(GetExportStatus());
}

/* check if envelope is marked as hidden for envelope dialog */

bool CParticle::EnvelopeHidden(int nIndex)
{
	switch (nIndex)
	{
	case X_SIZE_ENVELOPE:
		return((m_u32Flags & ENVELOPE_HIDE_X_SIZE) != 0);
	case Y_SIZE_ENVELOPE:
		return((m_u32Flags & ENVELOPE_HIDE_Y_SIZE) != 0);
	case RED_ENVELOPE:
		return((m_u32Flags & ENVELOPE_HIDE_RED) != 0);
	case GREEN_ENVELOPE:
		return((m_u32Flags & ENVELOPE_HIDE_GREEN) != 0);
	case BLUE_ENVELOPE:
		return((m_u32Flags & ENVELOPE_HIDE_BLUE) != 0);
	case ALPHA_ENVELOPE:
		return((m_u32Flags & ENVELOPE_HIDE_ALPHA) != 0);
	}
	ASSERT(FALSE);
	return(TRUE);
}

/* we can use memcpy to copy records as long as we do some prep work first. for example, memcpying a cstring is not
a happy thing */

void CParticle::PrepareForMemcpy(void)
{
	/* save contents of name */

	strncpy(m_NameString, m_Name.GetBuffer(10), PARTICLE_MAX_NAME_LENGTH);
	m_NameString[PARTICLE_MAX_NAME_LENGTH - 1] = 0;

	/* free up CString */

	SetName("");
	m_Name.ReleaseBuffer();
	m_Name.FreeExtra();
}
void CParticle::RestoreAfterMemcpy(void)
{
	/* set CString from temporary string */

	SetName(m_NameString);
}

/* return particle's image, if any */

CImage *CParticle::GetImage(bool *pbShowAlphaMask)
{
	CGraphic *pGraphic = GetGraphic();
	if (pGraphic)
	{
		*pbShowAlphaMask = pGraphic->IsShowAlphaMask();
		return(pGraphic->GetIconImage());
	}
	return(NULL);
}

/* do load-time fix-up on old versions of data. all pointers, etc have been resolved */

void CParticle::LoadFixup(void)
{
	if (GetLoadVersion() < 9)
	{
		/* at version 9, we added an explicit PART_ZBUFFER_WRITE flag, where prior to version 9 it */
		/* was set at export based on the graphic's alpha status-- feathered alpha images were assumed */
		/* to be things like smoke that we wanted an additive alpha effect for. set the bit to the */
		/* appropriate default */

		ASSERT(IsFlagSet(PART_ZBUFFER_WRITE) == FALSE);	// should never be set for pre-9 versions
		if ((GetGraphic()) && (GetGraphic()->AllImagesFeatheredAlpha() == FALSE))
			SetFlag(PART_ZBUFFER_WRITE);
	}
	if (GetLoadVersion() < 13)
	{
		/* at version 13, we added the PART_COLLIDES_WORLD, PART_COLLIDES_ACTORS, and PART_COLLIDE_WATER */
		/* flag bits. PART_COLLIDES_WORLD is set by default, so any earlier versions should get this bit set */

		SetFlag(PART_COLLIDES_WORLD);
	}
	if (GetLoadVersion() < 15)
	{
		/* at version 15, we added the PART_PERPENDICULAR_TO_VELOCITY flag, which uses an old bit in the flags word. */
		/* PART_PERPENDICULAR_TO_VELOCITY is clear by default, so any earlier versions should get this bit cleared */

		ClearFlag(PART_PERPENDICULAR_TO_VELOCITY);
	}
	if (GetLoadVersion() < 16)
	{
		/* at version 16, we added the PART_FLOATS_ON_WATER flag, which uses an old bit in the flags word. */
		/* PART_FLOATS_ON_WATER is clear by default, so any earlier versions should get this bit cleared */

		ClearFlag(PART_FLOATS_ON_WATER);

		/* we also add m_u32CollideFlags, which collects some collision-related flag bits-- transfer them */
		/* over, and then clear old versions */

		if (IsFlagSet(OLD_PART_COLLIDES))
			SetCollideFlag(PART_COLLIDES);
		if (IsFlagSet(OLD_PART_COLLIDES_WORLD))
			SetCollideFlag(PART_COLLIDES_WORLD);
		if (IsFlagSet(OLD_PART_COLLIDES_ACTORS))
			SetCollideFlag(PART_COLLIDES_ACTORS);
		if (IsFlagSet(OLD_PART_COLLIDES_WATER))
			SetCollideFlag(PART_COLLIDES_WATER);
		if (IsFlagSet(OLD_PART_COLLIDE_STOP_ROTATING))
			SetCollideFlag(PART_COLLIDE_STOP_ROTATING);
		if (IsFlagSet(OLD_PART_COLLIDE_STOP_ANIMATING))
			SetCollideFlag(PART_COLLIDE_STOP_ANIMATING);
		ClearFlag(OLD_PART_COLLIDES);
		ClearFlag(OLD_PART_COLLIDES_WORLD);
		ClearFlag(OLD_PART_COLLIDES_ACTORS);
		ClearFlag(OLD_PART_COLLIDES_WATER);
		ClearFlag(OLD_PART_COLLIDE_STOP_ROTATING);
		ClearFlag(OLD_PART_COLLIDE_STOP_ANIMATING);
	}

	/* allow envelopes to fix themselves up */

	for (int i = 0; i < MAX_ENVELOPES; i++)
		m_Envelope[i].LoadFixup();
}

/* test if data object is associated with passed data object, for association testing. returns true if a new association is found */

int CParticle::TestAssociate(CObject *pTestObject, int nType)
{
	/* do nothing if already marked as associated */

	if (IsAssociated())
		return(ASSOCIATE_NONE);

	/* should never be passed itself */

	ASSERT(this != pTestObject);

	/* type-specific association test */

	switch (nType)
	{
	case EFFECTS:
		/* does passed effect use this particle? */

		if (((CEffect *) pTestObject)->UsesParticle(this))
		{
			SetAssociateFlags();	// sets both associated and new-associate flags
			return(ASSOCIATE_EFFECT_USES_PARTICLE);
		}

		/* is passed effect one of the particle's spawn effects? */

		int i;
		for (i = 0; i < GetSpawnCount(); i++)
		{
			if (GetSpawn(i)->GetEffect() == pTestObject)
			{
				SetAssociateFlags();	// sets both associated and new-associate flags
				return(ASSOCIATE_EFFECT_SPAWNED_BY_PARTICLE);
			}
		}
		break;
	case PARTICLES:
		break;
	case GRAPHICS:
		/* does this particle use passed graphic? */

		if (GetGraphic() == pTestObject)
		{
			SetAssociateFlags();	// sets both associated and new-associate flags
			return(ASSOCIATE_GRAPHIC_USED_BY_PARTICLE);
		}
		break;
	case EMITTERS:
		/* does this particle use emitter (as a piggyback)? */

		if (GetPiggybackEmitter() == pTestObject)
		{
			SetAssociateFlags();	// sets both associated and new-associate flags
			return(ASSOCIATE_EMITTER_PIGGYBACKED_BY_PARTICLE);
		}
		break;
	case SWOOSHES:
		break;
	case SWOOSH_EMITTERS:
		break;
	default:
		ASSERT(false);
		break;
	}

	/* no association */

	return(ASSOCIATE_NONE);
}

/////////////////////////////////////////////////////////////////////////////
// CParticle operations

CParticle& CParticle::operator=( CParticle &Source )
{
	/* prepare records for memcpy-- this basically tidies up CStrings */

	PrepareForMemcpy();
	Source.PrepareForMemcpy();

	/* particle contains a CObArray of CSpawn records-- save off dest particle's array */

	ASSERT(GetSpawnCount() == Source.GetSpawnCount());	// should be the same size, since both should have been init'd this session with same surface count
	char SaveSpawn[sizeof(CObArray)];	// can't make it a CObArray or it will destruct
	memcpy(&SaveSpawn, &m_Spawn, sizeof(CObArray));	// saves pointers to spawn records, and memory malloc'd for CObArray itself

	/* do the memcpy */

	memcpy(this, &Source, sizeof(CParticle));

	/* restore spawn record array */

	memcpy(&m_Spawn, &SaveSpawn, sizeof(CObArray));

	/* copy spawn record contents over */

	for (int i = 0; i < GetSpawnCount(); i++)
		memcpy(m_Spawn.GetAt(i), Source.m_Spawn.GetAt(i), sizeof(CSpawn));

	/* clean up after memcpy */

	RestoreAfterMemcpy();
	Source.RestoreAfterMemcpy();

	/* done */

	return *this;
}

