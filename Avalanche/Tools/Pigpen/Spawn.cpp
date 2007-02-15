/***********************************/
/*                                 */
/* Spawn.cpp                       */
/* Pigpen particle tool  4/5/02    */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* simple effect spawning class    */
/*                                 */
/***********************************/

/* this is a very simple class to record which effect, if any, is associated with a surface type, for 
the collide tab of the particle edit dialog */

/******************* includes ****************************/

/*** pre-compiled headers through .h file (visual studio), can't be inside #if statement. ***/
/*** see .h file for more info.for codewarrior, pre-compiled header is handled through    ***/
/*** xcl compile/link interpreter, and doesn't need any explicit include                  ***/

#include <PigpenPCH.h>

/*** includes that don't belong in pre-compiled header (typically due to low frequency) ***/



/******************* defines *****************************/

/******************* structures **************************/

/******************* local variables *********************/

/******************* externs *****************************/

/******************* class functions *********************/

/* this enables easy serialization of the CObArray-- you just call array's Serialize function and all the
image records serialize themselves, in order-- slick */

IMPLEMENT_SERIAL(CSpawn, CObject, 0)

/////////////////////////////////////////////////////////////////////////////
// CSpawn construction

CSpawn::CSpawn()
{
	m_pEffect = NULL;
	m_nEffectIndex = -1;	// serialization only
	m_u32Flags = SPAWN_USE_DEFAULT_SURFACE;	// use default collision surface spawn settings, until user says otherwise
	m_u32Bitfields = 0;
	m_ulVersion = SPAWN_VERSION;
	m_fBounceEnergy = 0.5f;
}
CSpawn::~CSpawn()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSpawn serialization

void CSpawn::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ASSERT(m_ulVersion == SPAWN_VERSION);
		ar << m_ulVersion;
	
		/* version 0 */

		ar << m_nEffectIndex;
		ar << m_u32Flags;
		ar << m_u32Bitfields;
		ar << m_fBounceEnergy;
	}
	else
	{
		ar >> m_ulVersion;
		if (m_ulVersion > SPAWN_VERSION)
		{
			ASSERT(FALSE);	// shouldn't happen since later version should be caught by doc serialization
			if (PigpenApp.IsCommandLineMode())
				PigpenApp.SetReturnCode(CPigpenApp::RETURN_CODE_INVALID_VERSION);
			else
				AfxMessageBox("Loading spawn records.\nThis executable cannot load this version of Pigpen file.\nIt was saved by a later version of the executable.\nPlease update your Pigpen.exe.", MB_OK | MB_ICONEXCLAMATION);
			AfxThrowArchiveException(CArchiveException::generic);	// abort
		}

		/* as we add new versions, process latest version and then fall through to earlier versions */

		switch(m_ulVersion)
		{
		case 1:
			/* fall through */
		case 0:
			/* version 0 */

			ar >> m_nEffectIndex;
			ar >> m_u32Flags;
			ar >> m_u32Bitfields;
			ar >> m_fBounceEnergy;
			break;
		default:
			ASSERT(FALSE);
			break;
		}

		/* now using latest version */

		m_ulVersion = SPAWN_VERSION;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSpawn implementation

