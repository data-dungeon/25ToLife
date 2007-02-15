/***********************************/
/*                                 */
/* Emitter.cpp                     */
/* Pigpen particle tool  01/22/01  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle emitter class          */
/*                                 */
/***********************************/

/* a particle emitter emits particle effects at a given rate. there are various types of emitters,
some specifically designed for a particular game (for example, the tire emitters for road rash) */

/******************* includes ****************************/

/*** pre-compiled headers through .h file (visual studio), can't be inside #if statement. ***/
/*** see .h file for more info.for codewarrior, pre-compiled header is handled through    ***/
/*** xcl compile/link interpreter, and doesn't need any explicit include                  ***/

#include <PigpenPCH.h>

/*** includes that don't belong in pre-compiled header (typically due to low frequency) ***/



/******************* defines *****************************/

/******************* structures **************************/

/******************* local variables *********************/

/******************* global variables ********************/

char *pEmitterTypeString[] = {

	"Standard",
	"Orbiter",
	"Tire back spray",
	"Tire side spray",
	"Tire circle",
	"Flux",
	"Distant",

	/* any new emitter types should have names put here */

	""	// must be last-- signals end of list when filling combo box
};

/******************* externs *****************************/

/******************* class functions *********************/

/* this enables easy serialization of the CObArray-- you just call the array's Serialize function and all the
emitter records serialize themselves, in order-- slick */

IMPLEMENT_SERIAL(CEmitter, CObject, 0)

/////////////////////////////////////////////////////////////////////////////
// CEmitter construction

CEmitter::CEmitter()
{
	m_ulVersion = EMITTER_VERSION;
	m_ulFlags = 0;
	m_Name = "New";
	m_nUsedCount = -1;	// unknown status until analysis is done
	m_pEffect = NULL;
	m_nEffectIndex = -1;
	m_fDelay = 1.0f / 5.0f;	// seconds between emissions
	m_fDelayRandomPct = 0;
	m_fScale = 1.0;
	m_fScaleRandomPct = 0;
	_ASSERT(EMITTER_STANDARD == 0);	// types must be 0-based for emitter dialog combo box to work
	m_nType = EMITTER_STANDARD;
	m_pEngineData = NULL;
	m_nExportDataSize = 0;

	/* see comments in Engine\Game\Particles\ParticleMgr.c, routine ParticleEmitter__Create */
	/* for descriptions of these variables */

	m_fVelPct = 100.0f;
	m_fAdditiveVelPct = 0.0f;
	m_fMinDistance = 0.0f;	// 0 means don't worry about min distance
	m_fGoRadius = 5.0f;	// emitter is activated when bounding sphere intersects a scene view frustum
	m_fRadius = 1.0f;
	m_fAngularVel = 360.0f;
	m_fLifetime = 0.0;	// 0 means forever
	m_fLifetimeRandomPct = 0;
	m_fLOD1Radius = MAX_E_LOD_RADIUS;	// large value for near radius means no lod-- 100% emitter rate all the time
	m_fLOD2Radius = MAX_E_LOD_RADIUS;
	m_fFluxSize = 6.0f;
	m_fInvisDistance = 0.0f;
	m_fNearDistance = 0.0f;
	m_fNearScale = 1.0f;
	m_fFarDistance = 0.0f;
	m_fFarScale = 1.0f;
}
CEmitter::~CEmitter()
{
}

/////////////////////////////////////////////////////////////////////////////
// CEmitter serialization

void CEmitter::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ASSERT(m_ulVersion == EMITTER_VERSION); 
		ar << m_ulVersion;

		/* version 8-- added invisible distance, min distance and scale, and max distance and scale */

		ar << m_fInvisDistance;
		ar << m_fNearDistance;
		ar << m_fNearScale;
		ar << m_fFarDistance;
		ar << m_fFarScale;

		/* version 7--  no new fields, change existing field m_fRate, m_fRateRandomPct to m_fDelay, to m_fDelayRandomPct */

		/* version 6-- added flux emitter */

		ar << m_fFluxSize;

		/* version 5-- emitters have a near and far lod radius */

		ar << m_fLOD1Radius;	// near
		ar << m_fLOD2Radius;	// far

		/* version 4-- emitters have a scale factor to apply to particle size */

		ar << m_fScale;
		ar << m_fScaleRandomPct;

		/* version 3 */

		ar << m_fGoRadius;
		ar << m_fLifetime;
		ar << m_fLifetimeRandomPct;

		/* version 2-- no new data fields, but EMITTER_MOVE_ONLY flag added */

		/* version 1 */

		ar << m_nType;
		ar << m_fVelPct;
		ar << m_fAdditiveVelPct;
		ar << m_fMinDistance;
		ar << m_fRadius;
		ar << m_fAngularVel;

		/* version 0 */

		ar << m_ulFlags;
		ar << m_Name;
		ar << m_nEffectIndex;
		ar << m_fDelay;
		ar << m_fDelayRandomPct;
	}
	else
	{
		ar >> m_ulVersion;
		if (m_ulVersion > EMITTER_VERSION)
		{
			ASSERT(FALSE);	// shouldn't happen since later version should be caught by doc serialization
			if (PigpenApp.IsCommandLineMode())
				PigpenApp.SetReturnCode(CPigpenApp::RETURN_CODE_INVALID_VERSION);
			else
				AfxMessageBox("Loading emitters.\nThis executable cannot load this version of Pigpen file.\nIt was saved by a later version of the executable.\nPlease update your Pigpen.exe.", MB_OK | MB_ICONEXCLAMATION);
			AfxThrowArchiveException(CArchiveException::generic);	// abort
		}

		/* as we add new versions, process latest version and then fall through to earlier versions */

		switch(m_ulVersion)
		{
		case 8:
			/* version 8-- added invisible distance, min distance and scale, and max distance and scale */

			ar >> m_fInvisDistance;
			ar >> m_fNearDistance;
			ar >> m_fNearScale;
			ar >> m_fFarDistance;
			ar >> m_fFarScale;

			/* fall through */
		case 7:
			/* version 7 */

			/* no new fields, change existing field m_fRate, m_fRateRandomPct to m_fDelay, to m_fDelayRandomPct */

			/* fall through */
		case 6:
			/* version 6 */

			ar >> m_fFluxSize;

			/* fall through */
		case 5:
			/* version 5 */

			ar >> m_fLOD1Radius;
			ar >> m_fLOD2Radius;

			/* fall through */
		case 4:
			/* version 4 */

			ar >> m_fScale;
			ar >> m_fScaleRandomPct;

			/* fall through */
		case 3:
			/* version 3 */

			ar >> m_fGoRadius;
			ar >> m_fLifetime;
			ar >> m_fLifetimeRandomPct;

			/* fall through */
		case 2:
			/* version 2-- no new data fields, but EMITTER_MOVE_ONLY flag added */

			/* fall through */
		case 1:
			/* version 1 */

			ar >> m_nType;
			ar >> m_fVelPct;
			ar >> m_fAdditiveVelPct;
			ar >> m_fMinDistance;
			ar >> m_fRadius;
			ar >> m_fAngularVel;

			/* fall through */
		case 0:
			/* version 0 */

			ar >> m_ulFlags;
			ar >> m_Name;
			ar >> m_nEffectIndex;
			ar >> m_fDelay;
			ar >> m_fDelayRandomPct;
			break;
		default:
			ASSERT(FALSE);
			break;
		}

		/* clear associated flag on load-- it's temporary */

		m_ulFlags &= ~EMITTER_ASSOCIATED;

		/* now using latest version */

		m_ulLoadVersion = m_ulVersion;	// save version from load, for possible fix-ups
		m_ulVersion = EMITTER_VERSION;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEmitter implementation

CString& CEmitter::GetEffectName(void)
{
	static CString NameNone("None");

	if (m_pEffect == NULL)
		return(NameNone);
	else
		return(m_pEffect->GetName());
}

/* fill in data for export. return size of data, 0 if error or overflow */

int CEmitter::FillExportBuffer(char *pBuffer, int nMaxSize, bool bPreview)
{
	/* should have valid data at this point */

	ASSERT(GetExportStatus() == EXPORT_STATUS_VALID);
	ASSERT(m_pEffect);	// shouldn't be exporting if no effect

	/* export */

	ts_EmitterSource *pEmitterSource = (ts_EmitterSource *) pBuffer;

	/* preview check */

	if (m_pEffect->GetExportStatus() == EXPORT_STATUS_UNKNOWN)
	{
		ASSERT(bPreview);	// this should only happen in preview mode when changing a particle's graphic
		return(0);			// need to re-build preview .dbl
	}

	/* effect id and emitter type */

	pEmitterSource->u16EffectID = m_pEffect->GetTempIndex();
	pEmitterSource->u8Type = (u8) m_nType;

	/* fill in data */

	pEmitterSource->u16Delay = CONVERT_FLOAT_TO_U16(m_fDelay, MAX_E_DELAY);
	pEmitterSource->u8DelayRandomPct = (u8) m_fDelayRandomPct;
	pEmitterSource->u8Scale = CONVERT_FLOAT_TO_U8(m_fScale, MAX_E_SCALE);
	pEmitterSource->u8ScaleRandomPct = (u8) m_fScaleRandomPct;
	if (IsMoveOnly())
		pEmitterSource->u8VelPct = (u8) m_fVelPct;
	else
		pEmitterSource->u8VelPct = 0;	// 0 means use particle source data velocity
	pEmitterSource->u8AdditiveVelPct = (u8) m_fAdditiveVelPct;
	if (IsMinDistEnabled())
		pEmitterSource->u8MinDistance = CONVERT_FLOAT_TO_U8(m_fMinDistance, MAX_E_MIN_DIST);
	else
		pEmitterSource->u8MinDistance = 0;	// 0 means skip min distance check
	pEmitterSource->u16GoRadius = CONVERT_FLOAT_TO_U16(m_fGoRadius, MAX_E_GO_RADIUS);
	pEmitterSource->u8Radius = CONVERT_FLOAT_TO_U8(m_fRadius, MAX_E_RADIUS);
	pEmitterSource->s16AngularVelRadians = CONVERT_FLOAT_TO_U8(m_fAngularVel, MAX_E_ANGULAR_VEL);

	/* flags */

	pEmitterSource->u8Flags = 0;
	if ((IsScaleContRandom()) && (pEmitterSource->u8ScaleRandomPct != 0))
		pEmitterSource->u8Flags |= EMITTER_SOURCE_SCALE_CONT_RANDOM;
	if ((IsDelayContRandom()) && (pEmitterSource->u8DelayRandomPct != 0))
		pEmitterSource->u8Flags |= EMITTER_SOURCE_DELAY_CONT_RANDOM;
	if (IsMoveOnly())
		pEmitterSource->u8Flags |= EMITTER_SOURCE_ACTIVE_MOVE_ONLY;

	/* flux emitters do not have lod control */

	if (m_nType == EMITTER_FLUX)
	{
		pEmitterSource->u16LOD1Radius = CONVERT_FLOAT_TO_U16(MAX_E_LOD_RADIUS, MAX_E_LOD_RADIUS);
		pEmitterSource->u16LOD2Radius = CONVERT_FLOAT_TO_U16(MAX_E_LOD_RADIUS, MAX_E_LOD_RADIUS);
	}
	else
	{
		pEmitterSource->u16LOD1Radius = CONVERT_FLOAT_TO_U16(m_fLOD1Radius, MAX_E_LOD_RADIUS);
		pEmitterSource->u16LOD2Radius = CONVERT_FLOAT_TO_U16(m_fLOD2Radius, MAX_E_LOD_RADIUS);
	}

	/* lifetime */

	pEmitterSource->u8Lifetime = CONVERT_FLOAT_TO_U8(m_fLifetime, MAX_E_LIFETIME);
	pEmitterSource->u8LifetimeRandomPct = (u8) m_fLifetimeRandomPct;

	/* flux emitter size-- cut it down to half size */

	pEmitterSource->u8FluxHalfSize = CONVERT_FLOAT_TO_U8(m_fFluxSize * 0.5f, MAX_E_FLUX_HALF_SIZE);

	/* distant emitter */

	if (m_nType == EMITTER_DISTANT)
	{
		pEmitterSource->u16InvisDistance = CONVERT_FLOAT_TO_U16(m_fInvisDistance, MAX_E_LOD_RADIUS);
		pEmitterSource->u16NearDistance = CONVERT_FLOAT_TO_U16(m_fNearDistance, MAX_E_LOD_RADIUS);
		pEmitterSource->u8NearScale = CONVERT_FLOAT_TO_U8(m_fNearScale, MAX_E_SCALE);
		pEmitterSource->u16FarDistance = CONVERT_FLOAT_TO_U16(m_fFarDistance, MAX_E_LOD_RADIUS);
		pEmitterSource->u8FarScale = CONVERT_FLOAT_TO_U8(m_fFarScale, MAX_E_SCALE);
	}
	else
	{
		pEmitterSource->u16InvisDistance = 0;
		pEmitterSource->u16NearDistance = 0;
		pEmitterSource->u8NearScale = 0;
		pEmitterSource->u16FarDistance = 0;
		pEmitterSource->u8FarScale = 0;
	}

	/* maybe endian swap it */

	ts_EmitterSource *pEmitterSourceSwapped = DBLUtil__SwapEndian(pEmitterSource);
	if (pEmitterSourceSwapped != pEmitterSource)
		memcpy(pEmitterSource, pEmitterSourceSwapped, sizeof(ts_EmitterSource));	// if swapped, copy swapped version back into export buffer

	/* record and return size of data that got filled in */

	SetExportDataSize(sizeof(ts_EmitterSource));
	ASSERT(GetExportDataSize() <= nMaxSize);	// if this hits, we've overrun our budget-- probably a preview thing?
	return(GetExportDataSize());
}

/* determine if emitter is valid for export. typically, don't trickle down for emitters-- all non-empty effects
are valid for export regardles of whether they are referenced by an emitter, because you can start particle
effects directly. but in preview mode, where we are trying to export a smaller subset of the data for speed,
trickle down to catch everything needed */

int CEmitter::AnalyzeForExport(bool bPreview)
{
	if (GetExportStatus() == EXPORT_STATUS_UNKNOWN)
	{
		/* analyze data. don't export emitters that refer to empty effects */

		if (m_pEffect != NULL)
		{
			/* in preview mode, trickle down */

			if (bPreview)
				m_pEffect->AnalyzeForExport(bPreview);

			/* check status */

			if (m_pEffect->GetExportStatus() == EXPORT_STATUS_VALID)
			{
				SetExportStatus(EXPORT_STATUS_VALID);
				IncUsedCount();
			}
			else
			{
				char acMessage[100];
				sprintf(acMessage, "Emitter: %s, effect: %s", GetName().GetBuffer(10), m_pEffect->GetName().GetBuffer(10));
				AddConditionalInfoToErrorLog(acMessage);
				HANDLE_ERROR(RETURN_EMPTY_EMITTER);
				SetExportStatus(EXPORT_STATUS_INVALID);
			}
		}
		else
			SetExportStatus(EXPORT_STATUS_INVALID);
	}
	return(GetExportStatus());
}

/* set up a dummy emitter for when previewing is called from effect or particle dialog */

void CEmitter::SetupDummy(CEffect *pEffect)
{
	m_pEffect = pEffect;
}

/* we can use memcpy to copy records as long as we do some prep work first. for example, memcpying a cstring is not
a happy thing */

void CEmitter::PrepareForMemcpy(void)
{
	/* save contents of name */

	strncpy(m_NameString, m_Name.GetBuffer(10), EMITTER_MAX_NAME_LENGTH);
	m_NameString[EMITTER_MAX_NAME_LENGTH - 1] = 0;

	/* free up CString */

	SetName("");
	m_Name.ReleaseBuffer();
	m_Name.FreeExtra();
}
void CEmitter::RestoreAfterMemcpy(void)
{
	/* set CString from temporary string */

	SetName(m_NameString);
}

/* get image to use with emitter */

CImage *CEmitter::GetImage(bool *pbShowAlphaMask)
{
	if (m_pEffect)
		return(m_pEffect->GetImage(pbShowAlphaMask));
	return(NULL);
}

/* do load-time fix-up on old versions of data. all pointers, etc have been resolved */

void CEmitter::LoadFixup(void)
{
	if (GetLoadVersion() == 1)
	{
		/* EMITTER_MOVE_ONLY flag added with version 2-- force it to be set for tire spray emitters */

		if ((m_nType == EMITTER_TIRE_SIDE_SPRAY) || (m_nType == EMITTER_TIRE_BACK_SPRAY))
			m_ulFlags |= EMITTER_MOVE_ONLY;
	}

	if (GetLoadVersion() < 7)
	{
		/* at version 7, i changed m_fRate and m_fRateRandomPct to m_fDelay and m_fDelayRandomPct. rate was in */
		/* emissions per second, and delay is in seconds. mainly, this lets us go to slower emission rates than */
		/* 1 per second (e.g., one every 3 seconds). if the loaded file is an earlier version, convert rate to */
		/* delay */

		if (m_fDelay == 0.0f)
			m_fDelay = MAX_E_DELAY;
		else
		{
			m_fDelay = 1.0f / m_fDelay;
			if (m_fDelay < MIN_E_DELAY)
				m_fDelay = MIN_E_DELAY;
			if (m_fDelay > MAX_E_DELAY)
				m_fDelay = MAX_E_DELAY;
		}
	}
}

/* maintain the relationship lod1 radius <= lod2 radius. return true if correction made */

bool CEmitter::CheckLOD1Radius(void)
{
	if (m_fLOD1Radius <= m_fLOD2Radius)
		return(false);
	m_fLOD2Radius = m_fLOD1Radius;
	return(true);
}
bool CEmitter::CheckLOD2Radius(void)
{
	if (m_fLOD2Radius >= m_fLOD1Radius)
		return(false);
	m_fLOD1Radius = m_fLOD2Radius;
	return(true);
}

/* maintain the relationship invis distance <= near distance <= far distance */

bool CEmitter::CheckInvisDistance(void)
{
	if (m_fInvisDistance <= m_fNearDistance)
		return(false);
	m_fNearDistance = m_fInvisDistance;
	CheckNearDistance();	//near distance changed, have to check it
	return(true);
}
bool CEmitter::CheckNearDistance(void)
{
	if ((m_fNearDistance >= m_fInvisDistance) && (m_fNearDistance <= m_fFarDistance))
		return(false);
	if (m_fNearDistance < m_fInvisDistance)
		m_fInvisDistance = m_fNearDistance;
	if (m_fNearDistance > m_fFarDistance)
		m_fFarDistance = m_fNearDistance;
	return(true);
}
bool CEmitter::CheckFarDistance(void)
{
	if (m_fFarDistance >= m_fNearDistance)
		return(false);
	m_fNearDistance = m_fFarDistance;
	CheckNearDistance();	//near distance changed, have to check it
	return(true);
}

/* test if data object is associated with passed data object, for association testing. returns true if a new association is found */

int CEmitter::TestAssociate(CObject *pTestObject, int nType)
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
		/* does this emitter use passed effect? */

		if (pTestObject == GetEffect())
		{
			SetAssociateFlags();	// sets both associated and new-associate flags
			return(ASSOCIATE_EFFECT_USED_BY_EMITTER);
		}
		break;
	case PARTICLES:
		/* does passed particle use this emitter (as a piggyback)? */

		if (((CParticle *) pTestObject)->GetPiggybackEmitter() == this)
		{
			SetAssociateFlags();	// sets both associated and new-associate flags
			return(ASSOCIATE_PARTICLE_PIGGYBACKS_EMITTER);
		}
		break;
	case GRAPHICS:
		break;
	case EMITTERS:
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
// CEmitter operations

/* copy one emitter to another */

CEmitter &CEmitter::operator=( CEmitter &Source )
{
	/* prepare records for memcpy-- this basically tidies up CStrings */

	PrepareForMemcpy();
	Source.PrepareForMemcpy();

	/* do the memcpy */

	memcpy(this, &Source, sizeof(CEmitter));

	/* clean up after memcpy */

	RestoreAfterMemcpy();
	Source.RestoreAfterMemcpy();
	return *this;
}

