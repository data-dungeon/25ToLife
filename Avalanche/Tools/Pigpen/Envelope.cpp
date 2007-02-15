/***********************************/
/*                                 */
/* Envelope.cpp                    */
/* Pigpen particle tool  12/20/00  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle envelope class         */
/*                                 */
/***********************************/

/* the envelope that controls a particles behavior */

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

/////////////////////////////////////////////////////////////////////////////
// CEnvelope construction

CEnvelope::CEnvelope()
{
	m_ulVersion = ENVELOPE_VERSION;

	/* default points-- flat line at 1.0 */

	m_ControlPoints[SPLINE_POINT0].Set(0.0f,  1.0f);
	m_ControlPoints[SPLINE_POINT1].Set(0.25f, 0.5f);
	m_ControlPoints[SPLINE_POINT2].Set(0.75f, 0.5f);
	m_ControlPoints[SPLINE_POINT3].Set(1.0f,  1.0f);

	/* clip line control points hang out over right side of graph */

	m_ControlPoints[FLOOR_POINT].Set((float) (-GRAPH_CLIP_EXTEND_LEFT) / GRAPH_WIDTH, 0.0f);
	m_ControlPoints[CEILING_POINT].Set((float) (GRAPH_X_OFFSET - GRAPH_CLIP_EXTEND_RIGHT) / GRAPH_WIDTH, 1.0f);

	/* bitfield */

	m_ulBitfields = FLAT_ENVELOPE;
}
CEnvelope::~CEnvelope()
{
}

/////////////////////////////////////////////////////////////////////////////
// CEnvelope serialization

void CEnvelope::Serialize(CArchive& ar)
{
	int i;

	if (ar.IsStoring())
	{
		ASSERT(m_ulVersion == ENVELOPE_VERSION);
		ar << m_ulVersion;
	
		/* version 0 */

		for (i = 0; i < MAX_CONTROL_POINTS; i++)
		{
			ar << m_ControlPoints[i].x();
			ar << m_ControlPoints[i].y();
		}
		ar << m_ulBitfields;
	}
	else
	{
		ar >> m_ulVersion;
		if (m_ulVersion > ENVELOPE_VERSION)
		{
			ASSERT(FALSE);	// shouldn't happen since later version should be caught by doc serialization
			if (PigpenApp.IsCommandLineMode())
				PigpenApp.SetReturnCode(CPigpenApp::RETURN_CODE_INVALID_VERSION);
			else
				AfxMessageBox("Loading envelopes.\nThis executable cannot load this version of Pigpen file.\nIt was saved by a later version of the executable.\nPlease update your Pigpen.exe.", MB_OK | MB_ICONEXCLAMATION);
			AfxThrowArchiveException(CArchiveException::generic);	// abort
		}

		/* as we add new versions, process latest version and then fall through to earlier versions */

		switch(m_ulVersion)
		{
		case 1:
			/* fall through */
		case 0:
			/* version 0 */

			for (i = 0; i < MAX_CONTROL_POINTS; i++)
			{
				float fValue;
				ar >> fValue;
				m_ControlPoints[i].x(fValue);
				ar >> fValue;
				m_ControlPoints[i].y(fValue);
			}
			ar >> m_ulBitfields;

			/* make sure clip line control point x-position is correct */

			m_ControlPoints[FLOOR_POINT].X((float) (-GRAPH_CLIP_EXTEND_LEFT) / GRAPH_WIDTH);
			m_ControlPoints[CEILING_POINT].X((float) (GRAPH_WIDTH + GRAPH_CLIP_EXTEND_RIGHT) / GRAPH_WIDTH);

			/* this may be temporary, but we are currently only using the y-component of the spline curve-- */
			/* that is, we are using the curve y = f(t) instead of (x, y) = f(t). when you use the y-component */
			/* only, the x-position of the "bias handle" spline points doesn't affect the curve. this reduces */
			/* the expressiveness of the curve, but the y-only spline curve is still enough for the envelope */
			/* definition. the issue on the game engine side is that we have a value t that is a fraction of */
			/* the particle's lifetime, 0.0 to 1.0. when you look at a plot of the x-and-y spline curve, you */
			/* want to say, ok, when t = 0.5, then we are half-way along the x-axis, so give me that y-value-- */
			/* essentially, assume that the x-axis is the same as the t-axis. but since x is not linear with */
			/* respect to t (it's cubic), that's not true. so we would have to play numerical iteration games */
			/* to find out what value of t resulted in x = 0.5, then plug that t into the y-equation to get */
			/* the value of the envelope. more expensive than we want. */

			/* ok, the point as it relates to serializing the envelope is that at x = 0.25 for the first bias */
			/* handle spline point and x = 0.75 for the second bias handle spline point, the curves look pretty */
			/* much identical. what i want to avoid is having the particle designers change the x-position of */
			/* these spline points knowing that it doesn't affect the shape of the curve, and then sometime */
			/* later decide that we need the greater expressiveness, implement it, and have the behavior of their */
			/* envelopes change. so force the x-positions of thos spline points at load time until we decide */
			/* differently */

			if ((m_ulBitfields & ENVELOPE_TYPE_MASK) == SPLINE_ENVELOPE)
			{
				m_ControlPoints[SPLINE_POINT1].x(0.25f);
				m_ControlPoints[SPLINE_POINT2].x(0.75f);
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}

		/* now using latest version */

		m_ulLoadVersion = m_ulVersion;	// save version from load, for possible fix-ups
		m_ulVersion = ENVELOPE_VERSION;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEnvelope implementation

/* not all envelope types use all control points */

bool CEnvelope::IsValidControlPoint(int nIndex)
{
	switch(m_ulBitfields & ENVELOPE_TYPE_MASK)
	{
	case SPLINE_ENVELOPE:
	case ADSR_ENVELOPE:
		return(TRUE);	// uses all control points
	case STRAIGHT_ENVELOPE:
	case FLAT_ENVELOPE:
		switch(nIndex)
		{
		case SPLINE_POINT0:
		case SPLINE_POINT3:
		case FLOOR_POINT:
		case CEILING_POINT:
			return(TRUE);
		case SPLINE_POINT1:
		case SPLINE_POINT2:
		default:
			return(FALSE);
		}
	default:
		ASSERT(FALSE);
		return(FALSE);
	}
}

/* to give the user greater control, the area above and below the 0..100 area of the graph is treated differently for
the "bias handle" control points-- they are progressively exaggerated as the point gets farther outside the 0..100 area */

float CEnvelope::AdjustSplinePoint(float fY)
{
	if (fY < 0.0f)
	{
		fY = 1.0f - fY;	// convert to > 1.0
		fY = fY * fY * fY;
		fY = 1.0f - fY;	// convert back to negative
	}
	else if (fY > 1.0f)
		fY = fY * fY * fY;
	return(fY);
}

/* do load-time fix-up on old versions of data. all pointers, etc have been resolved */

void CEnvelope::LoadFixup(void)
{
	/* currently no fix-up required for envelope */
}

/* a useful default for envelopes-- linear from 100% to 0% */

void CEnvelope::Straight100To0(void)
{
	/* default points-- flat line at 1.0 */

	m_ControlPoints[SPLINE_POINT0].Set(0.0f,  1.0f);
	m_ControlPoints[SPLINE_POINT1].Set(0.25f, 0.5f);
	m_ControlPoints[SPLINE_POINT2].Set(0.75f, 0.5f);
	m_ControlPoints[SPLINE_POINT3].Set(1.0f,  0.0f);

	/* bitfield */

	m_ulBitfields = STRAIGHT_ENVELOPE;
}



