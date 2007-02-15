/***********************************/
/*                                 */
/* Surface.cpp                     */
/* Pigpen particle tool  4/4/02    */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* collision surface type class    */
/*                                 */
/***********************************/

/* this is a very simple class to let the user define a set of collision surface types for a pigpen file,
for particle effect spawning purposes in the collide tab of the particle edit dialog. each particle can
define a spawning behavior for each defined collision surface type. there are 3 pre-defined surface types:
end-of-life, end-of-life-underwater, and default. the end-of-life entries are not really surface types,
but the spawn-when-die definition shares a lot of the dialog with spawn-when-collide, so they are put
there. the default surface type is used to define a behavior whenever a particle collides with an "unknown"
surface, that is, a surface that the user has not defined a spawn behavior for. there may be lots of
surface types in the surface list, but the use only has to define behavior for the ones he wants, all others
will use the default spawn behavior */

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

IMPLEMENT_SERIAL(CSurface, CObject, 0)

/////////////////////////////////////////////////////////////////////////////
// CSurface construction

CSurface::CSurface()
{
	m_ulVersion = SURFACE_VERSION;
	m_ulFlags = 0;
	m_Name = "";
}
CSurface::~CSurface()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSurface serialization

void CSurface::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ASSERT(m_ulVersion == SURFACE_VERSION);
		ar << m_ulVersion;
	
		/* version 0 */

		ar << m_Name;
		ar << m_ulFlags;
	}
	else
	{
		ar >> m_ulVersion;
		if (m_ulVersion > SURFACE_VERSION)
		{
			ASSERT(FALSE);	// shouldn't happen since later version should be caught by doc serialization
			if (PigpenApp.IsCommandLineMode())
				PigpenApp.SetReturnCode(CPigpenApp::RETURN_CODE_INVALID_VERSION);
			else
				AfxMessageBox("Loading surface types.\nThis executable cannot load this version of Pigpen file.\nIt was saved by a later version of the executable.\nPlease update your Pigpen.exe.", MB_OK | MB_ICONEXCLAMATION);
			AfxThrowArchiveException(CArchiveException::generic);	// abort
		}

		/* as we add new versions, process latest version and then fall through to earlier versions */

		switch(m_ulVersion)
		{
		case 1:
			/* fall through */
		case 0:
			/* version 0 */

			ar >> m_Name;
			ar >> m_ulFlags;
			break;
		default:
			ASSERT(FALSE);
			break;
		}

		/* now using latest version */

		m_ulVersion = SURFACE_VERSION;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSurface implementation

