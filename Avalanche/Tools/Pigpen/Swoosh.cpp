/***********************************/
/*                                 */
/* Swoosh.cpp                      */
/* Pigpen particle tool  10/29/02  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* swoosh class                    */
/*                                 */
/***********************************/

/* a swoosh is a graphic pointer and a bunch of behavior controls */

/******************* includes ****************************/

/*** pre-compiled headers through .h file (visual studio), can't be inside #if statement. ***/
/*** see .h file for more info.for codewarrior, pre-compiled header is handled through    ***/
/*** xcl compile/link interpreter, and doesn't need any explicit include                  ***/

#include <PigpenPCH.h>

/*** includes that don't belong in pre-compiled header (typically due to low frequency) ***/



/******************* defines *****************************/

#define SWOOSH_DEFAULT_POINT_LIFETIME	0.25f	// in seconds
#define SWOOSH_DEFAULT_LOD					12.0f	// in meters

/******************* structures **************************/

/******************* local variables *********************/

/******************* externs *****************************/

extern "C" void WriteLogFile(const char *pcFmt, ...);

/******************* class functions *********************/

/* this enables easy serialization of the CObArray-- you just call array's Serialize function and all the
swoosh records serialize themselves, in order-- slick */

IMPLEMENT_SERIAL(CSwoosh, CObject, 0)

/////////////////////////////////////////////////////////////////////////////
// CSwoosh construction

CSwoosh::CSwoosh()
{
	m_ulVersion = SWOOSH_VERSION;
	m_u32Flags = 0;

	m_Name = "New";
	m_nUsedCount = -1;	// unknown status until analysis is done
	m_pEngineData = NULL;
	m_nExportDataSize = 0;

	/* display-related */

	m_pGraphic = NULL;
	m_nGraphicIndex = -1;
	m_u32RandomFlags = 0;
	m_u32Bitfields = 0;
	m_fFPS = DEFAULT_FPS;
	m_fFPSRandomPct = 0;

	/* envelopes */

	m_nCurrentEnvelope = 0;
	m_u32Flags |= ENVELOPE_HIDE_ALL;
	m_Envelope[X_SIZE_ENVELOPE].Straight100To0();	// this is typical for a swoosh
	m_Envelope[ALPHA_ENVELOPE].Straight100To0();		// this is typical for a swoosh

	/* colors for color pair color control */

	m_Color[0] = 0x00ffffff;	// white-- color at 100%
	m_Color[1] = 0x00000000;	// black-- color at 0%

	/* lifetime, LOD */

	m_fPointLifetime = SWOOSH_DEFAULT_POINT_LIFETIME;
	m_fLOD = SWOOSH_DEFAULT_LOD;

	/* edge 2 multipliers */

	m_fEdge2ColorMult = 1.0f;
	m_fEdge2AlphaMult = 1.0f;
}
CSwoosh::~CSwoosh()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSwoosh serialization

void CSwoosh::Serialize(CArchive& ar)
{
	int i;

	if (ar.IsStoring())
	{
		ASSERT(m_ulVersion == SWOOSH_VERSION);
		ar << m_ulVersion;

		/* version 0 */

		ar << m_u32Flags;
		ar << m_Name;
		ar << m_nGraphicIndex;
		ar << m_u32RandomFlags;
		ar << m_u32Bitfields;
		ar << m_fFPS;
		ar << m_fFPSRandomPct;
		ar << m_Color[0];
		ar << m_Color[1];
		ar << m_fPointLifetime;
		ar << m_fLOD;
		ar << m_fEdge2ColorMult;
		ar << m_fEdge2AlphaMult;

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
		if (m_ulVersion > SWOOSH_VERSION)
		{
			ASSERT(FALSE);	// shouldn't happen since later version should be caught by doc serialization
			if (PigpenApp.IsCommandLineMode())
				PigpenApp.SetReturnCode(CPigpenApp::RETURN_CODE_INVALID_VERSION);
			else
				AfxMessageBox("Loading swooshes.\nThis executable cannot load this version of Pigpen file.\nIt was saved by a later version of the executable.\nPlease update your Pigpen.exe.", MB_OK | MB_ICONEXCLAMATION);
			AfxThrowArchiveException(CArchiveException::generic);	// abort
		}

		/* as we add new versions, process latest version and then fall through to earlier versions */

		switch(m_ulVersion)
		{
		case 0:
			/* version 0 */

			ar >> m_u32Flags;
			ar >> m_Name;
			ar >> m_nGraphicIndex;
			ar >> m_u32RandomFlags;
			ar >> m_u32Bitfields;
			ar >> m_fFPS;
			ar >> m_fFPSRandomPct;
			ar >> m_Color[0];
			ar >> m_Color[1];
			ar >> m_fPointLifetime;
			ar >> m_fLOD;
			ar >> m_fEdge2ColorMult;
			ar >> m_fEdge2AlphaMult;

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

		m_u32Flags &= ~SWOO_ASSOCIATED;

		/* now using latest version */

		m_ulLoadVersion = m_ulVersion;	// save version from load, for possible fix-ups
		m_ulVersion = SWOOSH_VERSION;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSwoosh implementation

CString& CSwoosh::GetGraphicName(void)
{
	static CString NameNone("White");	// default texture for swooshes is a plain white texture

	if (m_pGraphic == NULL)
		return(NameNone);
	else
		return(m_pGraphic->GetName());
}

/* fill in data for export. return size of data, 0 if error or overflow */

int CSwoosh::FillExportBuffer(char *pBuffer, int nMaxSize, bool bPreview)
{
	/* should have valid data at this point */

	ASSERT(GetExportStatus() == EXPORT_STATUS_VALID);

	/* point to buffer to fill, clear data to make sure spaces between packed fields is 0 */

	ts_SwooshSource *pSwooshSource = (ts_SwooshSource *) pBuffer;
	memset(pSwooshSource, 0, nMaxSize);

	/* animation speed */

	pSwooshSource->u8FPS = (u8) m_fFPS;
	pSwooshSource->u8FPSRandomPct = (u8) m_fFPSRandomPct;

	/* lifetime, LOD */

	pSwooshSource->u8PointLifetime = CONVERT_FLOAT_TO_U8(m_fPointLifetime, MAX_S_LIFETIME);
	pSwooshSource->u8LOD = CONVERT_FLOAT_TO_U8(m_fLOD, MAX_S_LOD);

	/* edge multipliers */

	pSwooshSource->u8Edge2ColorMult = CONVERT_FLOAT_TO_U8(m_fEdge2ColorMult, 1.0f);
	pSwooshSource->u8Edge2AlphaMult = CONVERT_FLOAT_TO_U8(m_fEdge2AlphaMult, 1.0f);

	/* starting and ending image indices. if m__pGraphic is NULL, swoosh uses a small white texture that is */
	/* exported as first texture in texture chunk */

	if (m_pGraphic)
	{
//WriteLogFile("swoosh %s, graphic %s, first index = %d", GetName().GetBuffer(10), m_pGraphic->GetName().GetBuffer(10), m_pGraphic->GetImage(0)->GetTempIndex());
		if (m_pGraphic->GetExportStatus() == EXPORT_STATUS_UNKNOWN)
		{
			ASSERT(bPreview);	// this should only happen in preview mode when changing a particle's graphic
			return(0);
		}
		pSwooshSource->u16FirstIndex = m_pGraphic->GetImage(0)->GetTempIndex();
		pSwooshSource->u16LastIndex = m_pGraphic->GetImage(m_pGraphic->GetImageCount() - 1)->GetTempIndex();
	}
	else
	{
//WriteLogFile("swoosh %s, white texture", GetName().GetBuffer(10));
		pSwooshSource->u16FirstIndex = BIND_WHITE_TEXTURE;
		pSwooshSource->u16LastIndex = BIND_WHITE_TEXTURE;
	}
	ASSERT(pSwooshSource->u16LastIndex >= pSwooshSource->u16FirstIndex);

	/* envelopes */

	for (int i = 0; i < MAX_ENVELOPES; i++)
	{
		pSwooshSource->Envelope[i].fSplinePoint0Y = m_Envelope[i].m_ControlPoints[SPLINE_POINT0].y();
		pSwooshSource->Envelope[i].SplinePoint1 = m_Envelope[i].m_ControlPoints[SPLINE_POINT1];
		pSwooshSource->Envelope[i].SplinePoint2 = m_Envelope[i].m_ControlPoints[SPLINE_POINT2];
		pSwooshSource->Envelope[i].fSplinePoint3Y = m_Envelope[i].m_ControlPoints[SPLINE_POINT3].y();
		pSwooshSource->Envelope[i].fFloorY = m_Envelope[i].m_ControlPoints[FLOOR_POINT].y();
		pSwooshSource->Envelope[i].fCeilingY = m_Envelope[i].m_ControlPoints[CEILING_POINT].y();
		pSwooshSource->Envelope[i].u8Type = m_Envelope[i].GetType();

		/* if clip lines are at same position, treat as a flat line */

//yuch-- check converted 8 bit values for ==, not float values
		if (pSwooshSource->Envelope[i].fFloorY == pSwooshSource->Envelope[i].fCeilingY)
			pSwooshSource->Envelope[i].u8Type = FLAT_ENVELOPE;

		/* if endpoints for straight line are the same, treat as flat */

//yuch-- check converted 16 bit values for ==, not float values
		if ((pSwooshSource->Envelope[i].u8Type == STRAIGHT_ENVELOPE) &&
			 (pSwooshSource->Envelope[i].fSplinePoint0Y == pSwooshSource->Envelope[i].fSplinePoint3Y))
			pSwooshSource->Envelope[i].u8Type = FLAT_ENVELOPE;

		/* for flat line, envelope evaluator just returns fSplinePoint0Y, so make sure it's clipped */
		/* correctly */

		if (pSwooshSource->Envelope[i].u8Type == FLAT_ENVELOPE)
		{
			if (pSwooshSource->Envelope[i].fSplinePoint0Y > pSwooshSource->Envelope[i].fCeilingY)
				pSwooshSource->Envelope[i].fSplinePoint0Y = pSwooshSource->Envelope[i].fCeilingY;
			else if (pSwooshSource->Envelope[i].fSplinePoint0Y < pSwooshSource->Envelope[i].fFloorY)
				pSwooshSource->Envelope[i].fSplinePoint0Y = pSwooshSource->Envelope[i].fFloorY;
		}

		/* for spline, "bias handle" control points y-position is adjusted to give greater range */

		if (pSwooshSource->Envelope[i].u8Type == SPLINE_ENVELOPE)
		{
			pSwooshSource->Envelope[i].SplinePoint1.y(m_Envelope[i].AdjustSplinePoint(pSwooshSource->Envelope[i].SplinePoint1.y()));
			pSwooshSource->Envelope[i].SplinePoint2.y(m_Envelope[i].AdjustSplinePoint(pSwooshSource->Envelope[i].SplinePoint2.y()));
		}
	}

	/* colors for color pair color control */

	if (GetColorControl() == SWOO_COLOR_RGB)
		memset(pSwooshSource->Color, 0, 2 * sizeof(ts_bRGBA));	// colors not used in rgb mode
	else
	{
		for (int j = 0; j < 2; j++)
		{
			pSwooshSource->Color[j].R = (u8) m_Color[j] & 0xff;
			pSwooshSource->Color[j].G = (u8) (m_Color[j] >> 8) & 0xff;
			pSwooshSource->Color[j].B = (u8) (m_Color[j] >> 16) & 0xff;
			pSwooshSource->Color[j].A = 0;

			/* don't need to cut in half for ps2 use, swooshes use vertex buffers which take care of it */
		}
	}

	/* consolidate flags */

	m_u32TestFlags = 0;	// for error checking
	AddFlag(m_u32Flags, pSwooshSource, SWOO_ADDITIVE_RENDER, SWOOSH_ADDITIVE_RENDER);

	/* random flags */

	if (pSwooshSource->u8FPSRandomPct != 0)	// don't bother adding flag if no randomness
		AddFlag(m_u32RandomFlags, pSwooshSource, SWOO_FPS_CONT_RANDOM, SWOOSH_FPS_CONT_RANDOM);
	AddFlag(m_u32RandomFlags, pSwooshSource, SWOO_RANDOM_START_FRAME, SWOOSH_RANDOM_START_FRAME);

	/* bitfields */

	AddBitfield(pSwooshSource, SWOO_ANIM_MASK, SWOO_ANIM_SHIFT, SWOOSH_ANIM_MASK, SWOOSH_ANIM_SHIFT);
	AddBitfield(pSwooshSource, SWOO_COLOR_MASK, SWOO_COLOR_SHIFT, SWOOSH_COLOR_MASK, SWOOSH_COLOR_SHIFT);

	/* maybe endian swap it */

	ts_SwooshSource *pSwooshSourceSwapped = DBLUtil__SwapEndian(pSwooshSource);
	if (pSwooshSourceSwapped != pSwooshSource)
		memcpy(pSwooshSource, pSwooshSourceSwapped, sizeof(ts_SwooshSource));	// if swapped, copy swapped version back into export buffer

	/*** can't refer to contents of pSwooshSource after this point since it may be endian-swapped ***/

	/* record and return size of data that got filled in */

	SetExportDataSize(sizeof(ts_SwooshSource));
	ASSERT(GetExportDataSize() <= nMaxSize);	// if this hits, we've overrun our budget-- probably a preview thing?
	return(GetExportDataSize());
}

/* add a flag bit to the export flags field, with some error checking */

void CSwoosh::AddFlag(u32 u32Flags, ts_SwooshSource *pSwooshSource, u32 u32InternalFlag, u32 u32ExportFlag)
{
	/* this bit should be unique, of course, so make sure nothing else sets it */

	ASSERT((m_u32TestFlags & u32ExportFlag) == 0);	// if this hits, there is overlap in export flag bits defined in ParticleDB.h
	m_u32TestFlags |= u32ExportFlag;
	if (u32Flags & u32InternalFlag)
		pSwooshSource->u32Flags |= u32ExportFlag;
}

/* add a bitfield to the export flags field, with some error checking */

void CSwoosh::AddBitfield(ts_SwooshSource *pSwooshSource, u32 u32InternalMask, u32 u32InternalShift, u32 u32ExportMask, u32 u32ExportShift)
{
	/* none of bits used by this bitfield should have been set yet */

	ASSERT((m_u32TestFlags & u32ExportMask) == 0);	// if this hits, there is overlap in export flag bits defined in ParticleDB.h
	m_u32TestFlags |= u32ExportMask;

	/* masks should be the same */

	ASSERT((u32InternalMask >> u32InternalShift) == (u32ExportMask >> u32ExportShift));

	/* add the bitfield */

	pSwooshSource->u32Flags |= ((m_u32Bitfields & u32InternalMask) >> u32InternalShift) << u32ExportShift;
}

/* determine if swoosh is valid for export. an invalid swoosh points to a NULL or an empty graphic */

int CSwoosh::AnalyzeForExport(bool bPreview)
{
	if (GetExportStatus() == EXPORT_STATUS_UNKNOWN)
	{
		/* analyze data-- swooshes with no graphic selected are valid for export-- they will use a */
		/* tiny white texture */

		if ((m_pGraphic == NULL) || (m_pGraphic->AnalyzeForExport() == EXPORT_STATUS_VALID))
		{
			SetExportStatus(EXPORT_STATUS_VALID);
			IncUsedCount();
		}
		else
		{
			char acMessage[100];
			sprintf(acMessage, "Swoosh: %s", GetName().GetBuffer(10));
			AddConditionalInfoToErrorLog(acMessage);
			HANDLE_ERROR(RETURN_EMPTY_SWOOSH);
			SetExportStatus(EXPORT_STATUS_INVALID);
		}
	}
	return(GetExportStatus());
}

/* check if envelope is marked as hidden for envelope dialog */

bool CSwoosh::EnvelopeHidden(int nIndex)
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

void CSwoosh::PrepareForMemcpy(void)
{
	/* save contents of name */

	strncpy(m_NameString, m_Name.GetBuffer(10), SWOOSH_MAX_NAME_LENGTH);
	m_NameString[SWOOSH_MAX_NAME_LENGTH - 1] = 0;

	/* free up CString */

	SetName("");
	m_Name.ReleaseBuffer();
	m_Name.FreeExtra();
}
void CSwoosh::RestoreAfterMemcpy(void)
{
	/* set CString from temporary string */

	SetName(m_NameString);
}

/* return swoosh's image, if any */

CImage *CSwoosh::GetImage(bool *pbShowAlphaMask)
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

void CSwoosh::LoadFixup(void)
{
	/* allow envelopes to fix themselves up */

	for (int i = 0; i < MAX_ENVELOPES; i++)
		m_Envelope[i].LoadFixup();
}

/* test if data object is associated with passed data object, for association testing. returns true if a new association is found */

int CSwoosh::TestAssociate(CObject *pTestObject, int nType)
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
		/* does this swoosh use passed graphic? */

		if (GetGraphic() == pTestObject)
		{
			SetAssociateFlags();	// sets both associated and new-associate flags
			return(ASSOCIATE_GRAPHIC_USED_BY_SWOOSH);
		}
		break;
	case EMITTERS:
		break;
	case SWOOSHES:
		break;
	case SWOOSH_EMITTERS:
		/* does passed swoosh emitter use this swoosh? */

		if (((CPigpenSwooshEmitter *) pTestObject)->GetSwoosh() == this)
		{
			SetAssociateFlags();	// sets both associated and new-associate flags
			return(ASSOCIATE_SWOOSH_EMITTER_USES_SWOOSH);
		}
		break;
	default:
		ASSERT(false);
		break;
	}

	/* no association */

	return(ASSOCIATE_NONE);
}

/////////////////////////////////////////////////////////////////////////////
// CSwoosh operations

CSwoosh& CSwoosh::operator=( CSwoosh &Source )
{
	/* prepare records for memcpy-- this basically tidies up CStrings */

	PrepareForMemcpy();
	Source.PrepareForMemcpy();

	/* do the memcpy */

	memcpy(this, &Source, sizeof(CSwoosh));

	/* clean up after memcpy */

	RestoreAfterMemcpy();
	Source.RestoreAfterMemcpy();

	/* done */

	return *this;
}

