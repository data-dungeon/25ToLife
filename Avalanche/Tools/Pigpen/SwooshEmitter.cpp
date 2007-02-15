/***********************************/
/*                                 */
/* SwooshEmitter.cpp               */
/* Pigpen particle tool  11/25/02  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* swoosh emitter class            */
/*                                 */
/***********************************/

/* a swoosh emitter emits swooshes, generally in some kind of geometric shape */

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

char *pSwooshEmitterTypeString[] = {

	"Cylinder",

	/* any new emitter types should have names put here */

	""	// must be last-- signals end of list when filling combo box
};

/******************* externs *****************************/

/******************* class functions *********************/

/* this enables easy serialization of the CObArray-- you just call the array's Serialize function and all the
emitter records serialize themselves, in order-- slick */

IMPLEMENT_SERIAL(CPigpenSwooshEmitter, CObject, 0)

/////////////////////////////////////////////////////////////////////////////
// CPigpenSwooshEmitter construction

CPigpenSwooshEmitter::CPigpenSwooshEmitter()
{
	m_ulVersion = SWOOSH_EMITTER_VERSION;
	m_u32Flags = 0;
	m_Name = "New";
	m_nUsedCount = -1;	// unknown status until analysis is done
	m_pSwoosh = NULL;
	m_nSwooshIndex = -1;
	_ASSERT(SWOOSH_EMITTER_CYLINDER == 0);	// types must be 0-based for emitter dialog combo box to work
	m_nType = SWOOSH_EMITTER_CYLINDER;
	m_fLifetime = 0.0f;
	m_fLifetimeRandomPct = 0.0f;
	m_pEngineData = NULL;
	m_nExportDataSize = 0;

	/* cylinder type data */

	m_fCylinderRadius = 3.0f;
	m_fCylinderRadiusRandomPct = 0.0f;
	m_fCylinderHeight = 10.0f;
	m_fCylinderSwooshYVel = 3.0f;
	m_fCylinderSwooshYVelRandomPct = 0.0f;
	m_fCylinderSwooshRotVel = 0.0f;
	m_fCylinderDeltaTime = 0.25f;
	m_fCylinderDeltaAngle = 15.0f;
	m_fCylinderSwooshWidth = 0.25f;
	m_fCylinderSwooshWidthRandomPct = 0.0f;
	m_fCylinderPointsOnCircle = 4.0f;
	m_fCylinderSwooshLife = 1.0f;
	m_fCylinderSwooshLifeRandomPct = 0.0f;
	m_fCylinderPointsAtOnce = 0.0f;	// obsolete
}
CPigpenSwooshEmitter::~CPigpenSwooshEmitter()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPigpenSwooshEmitter serialization

void CPigpenSwooshEmitter::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ASSERT(m_ulVersion == SWOOSH_EMITTER_VERSION); 
		ar << m_ulVersion;

		/* version 2 */

		ar << m_fCylinderSwooshWidthRandomPct;
		ar << m_fCylinderSwooshYVelRandomPct;
		ar << m_fCylinderSwooshLife;
		ar << m_fCylinderSwooshLifeRandomPct;

		/* version 1 */

		ar << m_fCylinderRadiusRandomPct;
		ar << m_fCylinderDeltaAngle;

		/* version 0 */

		ar << m_u32Flags;
		ar << m_Name;
		ar << m_nSwooshIndex;
		ar << m_nType;
		ar << m_fLifetime;
		ar << m_fLifetimeRandomPct;
		ar << m_fCylinderRadius;
		ar << m_fCylinderHeight;
		ar << m_fCylinderSwooshYVel;
		ar << m_fCylinderSwooshRotVel;
		ar << m_fCylinderDeltaTime;
		ar << m_fCylinderSwooshWidth;
		ar << m_fCylinderPointsOnCircle;
		ar << m_fCylinderPointsAtOnce;	// obsolete
	}
	else
	{
		ar >> m_ulVersion;
		if (m_ulVersion > SWOOSH_EMITTER_VERSION)
		{
			ASSERT(FALSE);	// shouldn't happen since later version should be caught by doc serialization
			if (PigpenApp.IsCommandLineMode())
				PigpenApp.SetReturnCode(CPigpenApp::RETURN_CODE_INVALID_VERSION);
			else
				AfxMessageBox("Loading swoosh emitters.\nThis executable cannot load this version of Pigpen file.\nIt was saved by a later version of the executable.\nPlease update your Pigpen.exe.", MB_OK | MB_ICONEXCLAMATION);
			AfxThrowArchiveException(CArchiveException::generic);	// abort
		}

		/* as we add new versions, process latest version and then fall through to earlier versions */

		switch(m_ulVersion)
		{
		case 2:
			/* version 2 */

			ar >> m_fCylinderSwooshWidthRandomPct;
			ar >> m_fCylinderSwooshYVelRandomPct;
			ar >> m_fCylinderSwooshLife;
			ar >> m_fCylinderSwooshLifeRandomPct;

			/* and fall through */
		case 1:
			/* version 1 */

			ar >> m_fCylinderRadiusRandomPct;
			ar >> m_fCylinderDeltaAngle;

			/* and fall through */
		case 0:
			/* version 0 */

			ar >> m_u32Flags;
			ar >> m_Name;
			ar >> m_nSwooshIndex;
			ar >> m_nType;
			ar >> m_fLifetime;
			ar >> m_fLifetimeRandomPct;
			ar >> m_fCylinderRadius;
			ar >> m_fCylinderHeight;
			ar >> m_fCylinderSwooshYVel;
			ar >> m_fCylinderSwooshRotVel;
			ar >> m_fCylinderDeltaTime;
			ar >> m_fCylinderSwooshWidth;
			ar >> m_fCylinderPointsOnCircle;
			ar >> m_fCylinderPointsAtOnce;	// obsolete
			break;
		default:
			ASSERT(FALSE);
			break;
		}

		/* clear associated flag on load-- it's temporary */

		m_u32Flags &= ~SWOO_EMITTER_ASSOCIATED;

		/* now using latest version */

		m_ulLoadVersion = m_ulVersion;	// save version from load, for possible fix-ups
		m_ulVersion = SWOOSH_EMITTER_VERSION;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPigpenSwooshEmitter implementation

CString& CPigpenSwooshEmitter::GetSwooshName(void)
{
	static CString NameNone("None");

	if (m_pSwoosh == NULL)
		return(NameNone);
	else
		return(m_pSwoosh->GetName());
}

/* fill in data for export. return size of data, 0 if error or overflow */

int CPigpenSwooshEmitter::FillExportBuffer(char *pBuffer, int nMaxSize, bool bPreview)
{
	/* should have valid data at this point */

	ASSERT(GetExportStatus() == EXPORT_STATUS_VALID);
	ASSERT(m_pSwoosh);	// shouldn't be exporting if no swoosh

	/* export */

	ts_SwooshEmitterSource *pSwooshEmitterSource = (ts_SwooshEmitterSource *) pBuffer;

	/* preview check */

	if (m_pSwoosh->GetExportStatus() == EXPORT_STATUS_UNKNOWN)
	{
		ASSERT(bPreview);	// this should only happen in preview mode when changing a particle's graphic
		return(0);			// need to re-build preview .dbl
	}

	/* swoosh id and emitter type */

	pSwooshEmitterSource->u16SwooshID = m_pSwoosh->GetTempIndex();
	switch(m_nType)
	{
	case SWOOSH_EMITTER_CYLINDER:
		pSwooshEmitterSource->u16Flags = SWOOSH_EMITTER_SOURCE_CYLINDER;
		break;
	case SWOOSH_EMITTER_PREVIEW:
		pSwooshEmitterSource->u16Flags = SWOOSH_EMITTER_SOURCE_PREVIEW;
		break;
	default:
		ASSERT(false);	// unhandled case
		break;
	}

	/* lifetime */

	pSwooshEmitterSource->fLifetime = m_fLifetime;
	pSwooshEmitterSource->u8LifetimeRandomPct = (u8) m_fLifetimeRandomPct;

	/* lod-- this is the distance at which the emitter should deactivate all it's swooshes-- the swooshes */
	/* themselves will fade out with distance according to their own LOD setting */

	pSwooshEmitterSource->fLOD = m_pSwoosh->GetLOD();
	switch(m_nType)
	{
	case SWOOSH_EMITTER_CYLINDER:
		pSwooshEmitterSource->fLOD += Math::Max(m_fCylinderRadius, m_fCylinderHeight);
		break;
	}

	/* cylinder-type specific */

	pSwooshEmitterSource->fCylinderRadius = m_fCylinderRadius;
	pSwooshEmitterSource->u8CylinderRadiusRandomPct = (int) m_fCylinderRadiusRandomPct;
	pSwooshEmitterSource->fCylinderHeight = m_fCylinderHeight;
	pSwooshEmitterSource->fCylinderSwooshYVel = m_fCylinderSwooshYVel;
	pSwooshEmitterSource->u8CylinderSwooshYVelRandomPct = (int) m_fCylinderSwooshYVelRandomPct;
	pSwooshEmitterSource->fCylinderSwooshRotVel = Math::Deg2Rad(m_fCylinderSwooshRotVel);
	if (IsSparseCylinder())	// swooshes start randomly in cylinder and disappear after a while, they don't start at bottom and go all the way up
	{
		pSwooshEmitterSource->fCylinderSwooshLife = m_fCylinderSwooshLife;
		pSwooshEmitterSource->u8CylinderSwooshLifeRandomPct = (int) m_fCylinderSwooshLifeRandomPct;
	}
	else
	{
		pSwooshEmitterSource->fCylinderSwooshLife = 0.0f;
		pSwooshEmitterSource->u8CylinderSwooshLifeRandomPct = 0;
	}
	pSwooshEmitterSource->fCylinderDeltaTime = m_fCylinderDeltaTime;
	pSwooshEmitterSource->fCylinderDeltaAngle = Math::Deg2Rad(m_fCylinderDeltaAngle);
	pSwooshEmitterSource->fCylinderSwooshWidth = m_fCylinderSwooshWidth;
	pSwooshEmitterSource->u8CylinderSwooshWidthRandomPct = (int) m_fCylinderSwooshWidthRandomPct;
	pSwooshEmitterSource->u32CylinderPointsOnCircle = (u32) m_fCylinderPointsOnCircle;

	/* maybe endian swap it */

	ts_SwooshEmitterSource *pSwooshEmitterSourceSwapped = DBLUtil__SwapEndian(pSwooshEmitterSource);
	if (pSwooshEmitterSourceSwapped != pSwooshEmitterSource)
		memcpy(pSwooshEmitterSource, pSwooshEmitterSourceSwapped, sizeof(ts_SwooshEmitterSource));	// if swapped, copy swapped version back into export buffer

	/* record and return size of data that got filled in */

	SetExportDataSize(sizeof(ts_SwooshEmitterSource));
	ASSERT(GetExportDataSize() <= nMaxSize);	// if this hits, we've overrun our budget-- probably a preview thing?
	return(GetExportDataSize());
}

/* determine if emitter is valid for export. typically, don't trickle down for emitters-- all non-empty effects
are valid for export regardles of whether they are referenced by an emitter, because you can start particle
effects directly. but in preview mode, where we are trying to export a smaller subset of the data for speed,
trickle down to catch everything needed */

int CPigpenSwooshEmitter::AnalyzeForExport(bool bPreview)
{
	if (GetExportStatus() == EXPORT_STATUS_UNKNOWN)
	{
		/* analyze data. don't export emitters that refer to empty effects */

		if (m_pSwoosh != NULL)
		{
			/* in preview mode, trickle down */

			if (bPreview)
				m_pSwoosh->AnalyzeForExport(bPreview);

			/* check status */

			if (m_pSwoosh->GetExportStatus() == EXPORT_STATUS_VALID)
			{
				SetExportStatus(EXPORT_STATUS_VALID);
				IncUsedCount();
			}
			else
			{
				char acMessage[100];
				sprintf(acMessage, "Swoosh emitter: %s, swoosh: %s", GetName().GetBuffer(10), m_pSwoosh->GetName().GetBuffer(10));
				AddConditionalInfoToErrorLog(acMessage);
				HANDLE_ERROR(RETURN_EMPTY_SWOOSH_EMITTER);
				SetExportStatus(EXPORT_STATUS_INVALID);
			}
		}
		else
			SetExportStatus(EXPORT_STATUS_INVALID);
	}
	return(GetExportStatus());
}

/* set up a dummy emitter for when previewing is called from effect or particle dialog */

void CPigpenSwooshEmitter::SetupDummy(CSwoosh *pSwoosh)
{
	m_pSwoosh = pSwoosh;
	m_nType = SWOOSH_EMITTER_PREVIEW;
}

/* we can use memcpy to copy records as long as we do some prep work first. for example, memcpying a cstring is not
a happy thing */

void CPigpenSwooshEmitter::PrepareForMemcpy(void)
{
	/* save contents of name */

	strncpy(m_NameString, m_Name.GetBuffer(10), EMITTER_MAX_NAME_LENGTH);
	m_NameString[EMITTER_MAX_NAME_LENGTH - 1] = 0;

	/* free up CString */

	SetName("");
	m_Name.ReleaseBuffer();
	m_Name.FreeExtra();
}
void CPigpenSwooshEmitter::RestoreAfterMemcpy(void)
{
	/* set CString from temporary string */

	SetName(m_NameString);
}

/* get image to use with emitter */

CImage *CPigpenSwooshEmitter::GetImage(bool *pbShowAlphaMask)
{
	if (m_pSwoosh)
		return(m_pSwoosh->GetImage(pbShowAlphaMask));
	return(NULL);
}

/* do load-time fix-up on old versions of data. all pointers, etc have been resolved */

void CPigpenSwooshEmitter::LoadFixup(void)
{
	/* nothing to do for swoosh emitters */
}

/* test if data object is associated with passed data object, for association testing. returns true if a new association is found */

int CPigpenSwooshEmitter::TestAssociate(CObject *pTestObject, int nType)
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
		break;
	case PARTICLES:
		break;
	case GRAPHICS:
		break;
	case EMITTERS:
		break;
	case SWOOSHES:
		/* does this swoosh emitter use passed swoosh? */

		if (GetSwoosh() == pTestObject)
		{
			SetAssociateFlags();	// sets both associated and new-associate flags
			return(ASSOCIATE_SWOOSH_USED_BY_SWOOSH_EMITTER);
		}
		break;
	case SWOOSH_EMITTERS:
		break;
	}

	/* no association */

	return(ASSOCIATE_NONE);
}

/////////////////////////////////////////////////////////////////////////////
// CPigpenSwooshEmitter operations

/* copy one emitter to another */

CPigpenSwooshEmitter &CPigpenSwooshEmitter::operator=( CPigpenSwooshEmitter &Source )
{
	/* prepare records for memcpy-- this basically tidies up CStrings */

	PrepareForMemcpy();
	Source.PrepareForMemcpy();

	/* do the memcpy */

	memcpy(this, &Source, sizeof(CPigpenSwooshEmitter));

	/* clean up after memcpy */

	RestoreAfterMemcpy();
	Source.RestoreAfterMemcpy();
	return *this;
}

