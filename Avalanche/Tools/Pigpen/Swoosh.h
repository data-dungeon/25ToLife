/***********************************/
/*                                 */
/* Swoosh.h                        */
/* Pigpen particle tool  10/29/02  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* swoosh class                    */
/*                                 */
/***********************************/

#ifndef __SWOOSH_H
#define __SWOOSH_H

/******************* includes ****************************/

#include "Graphic.h"
#include "Envelope.h"

/* ImportExport includes */

/* game engine includes */

#include <Game/Managers/ParticleDB.h>

/******************* defines *****************************/

/* file version. must be an integer. increment by 1 each time there is a version change in swoosh serialization */

#define SWOOSH_VERSION		0

/* max size of swoosh name. name is treated as a CString for the most part, but there are times when it */
/* it's useful to be able to save off the string somewhere else for copying purposes */

#define SWOOSH_MAX_NAME_LENGTH		100 

/* flag bits for m_u32Flags. have to avoid conflict with engine swoosh flags enumerated in ParticleDB.h, so use */
/* "SWOO_" instead of "SWOOSH_" */

#define SWOO_ASSOCIATED					0x00000001
#define SWOO_SELECTED					0x00000002	// for serialization
#define SWOO_ADDITIVE_RENDER			0x00000004	// use additive blend function when rendering
#define SWOO_NEW_ASSOCIATE				0x00000008
/*** must be shared with Particle.h ***/
//#define ENVELOPE_HIDE_X_SIZE		0x00000020
//#define ENVELOPE_HIDE_Y_SIZE		0x00000040
//#define ENVELOPE_HIDE_RED			0x00000080
//#define ENVELOPE_HIDE_GREEN			0x00000100
//#define ENVELOPE_HIDE_BLUE			0x00000200
//#define ENVELOPE_HIDE_ALPHA			0x00000400
//#define ENVELOPE_HIDE_ALL			(ENVELOPE_HIDE_X_SIZE | ENVELOPE_HIDE_Y_SIZE | ENVELOPE_HIDE_RED | ENVELOPE_HIDE_GREEN | ENVELOPE_HIDE_BLUE | ENVELOPE_HIDE_ALPHA)
/*** must be shared with Particle.h ***/
/*** always add to end of list-- don't reorder-- flags are serialized ***/

/* flag bits for m_u32RandomFlags */

#define SWOO_FPS_CONT_RANDOM			0x00000001
#define SWOO_RANDOM_START_FRAME		0x00000002

/* values and masks for m_u32Bitfields (radio button bit fields) */

#define SWOO_ANIM_SHIFT					0
#define SWOO_ANIM_LOOP					(0x00000000 << SWOO_ANIM_SHIFT)
#define SWOO_ANIM_PING_PONG			(0x00000001 << SWOO_ANIM_SHIFT)
#define SWOO_ANIM_ONE_SHOT				(0x00000002 << SWOO_ANIM_SHIFT)
#define SWOO_ANIM_MASK					(0x00000003 << SWOO_ANIM_SHIFT)

#define SWOO_COLOR_SHIFT				(SWOO_ANIM_SHIFT + 2)	// skip over anim mask
#define SWOO_COLOR_PAIR					(0x00000000 << SWOO_COLOR_SHIFT)
#define SWOO_COLOR_RGB					(0x00000001 << SWOO_COLOR_SHIFT)
#define SWOO_COLOR_MASK					(0x00000001 << SWOO_COLOR_SHIFT)

/******************* structures **************************/

/******************* forward declarations ****************/

/******************* class *******************************/

class CSwoosh : public CObject
{
public:

	/* this enables easy serialization of the CObArray-- you just call array's Serialize function and all the */
	/* swoosh records serialize themselves, in order-- slick */

	DECLARE_SERIAL(CSwoosh)

// Constructor/destructor

	CSwoosh();
	~CSwoosh();

// Implementation

protected:

	/* keep serialization up-to-date! */

	CString m_Name;
	unsigned long m_ulVersion;
	unsigned long m_u32Flags;
	unsigned long m_u32RandomFlags;	// one-shot vs continuous randomness for application of randomness value
	unsigned long m_u32Bitfields;		// for radio button groups-- can be more than one bit wide
	int m_nGraphicIndex;					// used for serialization pointer-to-index conversion
	
	/* exported as integers */

	float m_fFPS;							// animation speed
	float m_fFPSRandomPct;				// animation speed randomness

	/* envelopes */

	CEnvelope m_Envelope[MAX_ENVELOPES];	// envelopes for swoosh behavior control
	int m_nCurrentEnvelope;						// current envelope being edited in envelope dialog

	/* colors for color pair color control */

	COLORREF	m_Color[2];

	/* point lifetime, swoosh LOD distance */

	float m_fPointLifetime;				// history point lifetime (in seconds)
	float m_fLOD;							// LOD (in meters, past LOD distance swoosh is invisible)

	/* edge2 multipliers */

	float m_fEdge2ColorMult;
	float m_fEdge2AlphaMult;

	/* not serialized */

	CGraphic *m_pGraphic;				// graphic that swoosh uses, set at construction or serialization loading
	int m_nTempIndex;						// used for export
	int m_nExportStatus;					// used for export
	int m_nExportDataSize;				// used for preview
	u32 m_u32TestFlags;					// used for export error-checking
	void *m_pEngineData;					// for real-time update of swoosh data during swoosh preview
	char m_NameString[SWOOSH_MAX_NAME_LENGTH];	// for record copying purposes
	int m_nUsedCount;						// for use analysis
	unsigned long m_ulLoadVersion;	// version number from serialization load, for possible fix-ups

	/* keep serialization up-to-date! */

public:

	/* simple routines */

	CString& GetName(void)								{ return(m_Name); }
	void SetName(char *pName)							{ m_Name = pName; }
	void SetGraphic(CGraphic *pGraphic)				{ m_pGraphic = pGraphic; }
	CGraphic *GetGraphic(void)							{ return(m_pGraphic); }
	void SetGraphicIndex(int nIndex)					{ m_nGraphicIndex = nIndex; }
	int GetGraphicIndex(void)							{ return(m_nGraphicIndex); }
	int GetHeight(void)									{ if (m_pGraphic) return(m_pGraphic->GetHeight()); else return(1); }
	int GetWidth(void)									{ if (m_pGraphic) return(m_pGraphic->GetWidth()); else return(1); }
	void SetFPS(float fFPS)								{ m_fFPS = fFPS; }
	void SetTempIndex(int nIndex)						{ m_nTempIndex = nIndex; }
	int GetTempIndex(void)								{ return(m_nTempIndex); }
	void SetExportStatus(int nStatus)				{ m_nExportStatus = nStatus; }
	int GetExportStatus(void)							{ return(m_nExportStatus); }
	void SetExportDataSize(int nSize)				{ m_nExportDataSize = nSize; }
	int GetExportDataSize(void)						{ return(m_nExportDataSize); }
	void SetSelected(bool bSelected)					{ if (bSelected) m_u32Flags |= SWOO_SELECTED; else m_u32Flags &= ~SWOO_SELECTED; }
	bool GetSelected(void)								{ return((m_u32Flags & SWOO_SELECTED) != 0); }
	CEnvelope *GetCurrentEnvelope(void)				{ return(&m_Envelope[m_nCurrentEnvelope]); }
	void SetCurrentEnvelopeIndex(int nIndex)		{ m_nCurrentEnvelope = nIndex; }
	int GetCurrentEnvelopeIndex(void)				{ return(m_nCurrentEnvelope); }
	CEnvelope *GetEnvelope(int nIndex)				{ return(&m_Envelope[nIndex]); }
	int GetColorControl(void)							{ return(m_u32Bitfields & SWOO_COLOR_MASK); }
	void SetEngineData(void *pData)					{ m_pEngineData = pData; }
	void *GetEngineData(void)							{ return(m_pEngineData); }
	void SetColor(int nIndex, COLORREF Color)		{ m_Color[nIndex] = Color; }
	COLORREF GetColor(int nIndex)						{ return(m_Color[nIndex]); }
	void SetUsedCount(int nCount)						{ m_nUsedCount = nCount; }
	int GetUsedCount(void)								{ return(m_nUsedCount); }
	void IncUsedCount(void)								{ m_nUsedCount++; }
	void SetFlag(u32 u32FlagBit)						{ m_u32Flags |= u32FlagBit; }
	void ClearFlag(u32 u32FlagBit)					{ m_u32Flags &= ~u32FlagBit; }
	bool IsFlagSet(u32 u32FlagBit)					{ return((m_u32Flags & u32FlagBit) == u32FlagBit); }	// can be more than one bit passed, will check for all bits set
	unsigned long GetLoadVersion(void)				{ return(m_ulLoadVersion); }
	float GetLOD()											{ return(m_fLOD); }
	
	/* routines defined in .cpp */

	CString& GetGraphicName(void);
	int FillExportBuffer(char *pBuffer, int nMaxSize, bool bPreview);
	void AddFlag(u32 u32Flags, ts_SwooshSource *pSwooshSource, u32 u32InternalFlag, u32 u32ExportFlag);
	void AddBitfield(ts_SwooshSource *pSwooshSource, u32 u32InternalMask, u32 u32InternalShift, u32 u32ExportMask, u32 u32ExportShift);
	int AnalyzeForExport(bool bPreview);
	bool EnvelopeHidden(int nIndex);
	void PrepareForMemcpy(void);
	void RestoreAfterMemcpy(void);
	CImage *GetImage(bool *pbShowAlphaMask);
	void LoadFixup(void);

	/* association testing */

	bool IsAssociated()									{ return((m_u32Flags & SWOO_ASSOCIATED) != 0); }
	bool IsNewAssociate()								{ return((m_u32Flags & SWOO_NEW_ASSOCIATE) != 0); }
	void ClearAssociateFlags()							{ m_u32Flags &= ~(SWOO_ASSOCIATED | SWOO_NEW_ASSOCIATE); }
	void ClearNewAssociateFlag()						{ m_u32Flags &= ~SWOO_NEW_ASSOCIATE; }
	void SetAssociateFlags()							{ m_u32Flags |= (SWOO_ASSOCIATED | SWOO_NEW_ASSOCIATE); }
	int TestAssociate(CObject *pTestObject, int nType);

	/* override serialization-- let object save itself out */

	virtual void Serialize(CArchive& ar);

	/* for more generic dialog processing, give access to addresses of data fields. data field id's are used */
	/* by dialog control lists */

	enum {	// because the enum is inside the class, it needs to be referenced as CSwoosh::DATA_XXX
		DATA_FLAGS = 0,
		DATA_RANDOM_FLAGS,
		DATA_BITFIELDS,

		/* display page */

		DATA_FPS,
		DATA_FPS_RANDOM,
		DATA_EDGE2_COLOR_MULT,
		DATA_EDGE2_ALPHA_MULT,
		DATA_POINT_LIFETIME,
		DATA_LOD,

		/* envelope dialog */

		DATA_ENVELOPE_BITFIELDS,
		DATA_CURRENT_ENVELOPE,

		// update GetDataPointer, below, as you add more
	};

	void *GetDataPointer(int nDataID)
	{
		switch(nDataID)
		{
		case DATA_FLAGS:
			return(&m_u32Flags);
		case DATA_RANDOM_FLAGS:
			return(&m_u32RandomFlags);
		case DATA_BITFIELDS:
			return(&m_u32Bitfields);

		/* display page */

		case DATA_FPS:
			return(&m_fFPS);
		case DATA_FPS_RANDOM:
			return(&m_fFPSRandomPct);
		case DATA_EDGE2_COLOR_MULT:
			return(&m_fEdge2ColorMult);
		case DATA_EDGE2_ALPHA_MULT:
			return(&m_fEdge2AlphaMult);
		case DATA_POINT_LIFETIME:
			return(&m_fPointLifetime);
		case DATA_LOD:
			return(&m_fLOD);

		/* envelope page */

		case DATA_ENVELOPE_BITFIELDS:
			return(&m_Envelope[m_nCurrentEnvelope].m_ulBitfields);
		case DATA_CURRENT_ENVELOPE:
			return(&m_nCurrentEnvelope);
		}
		ASSERT(FALSE);
		return(NULL);
	}

// operations

public:
	
	/* overload = operator to make copying swooshes easy. it is NOT ok to do a memcpy because the class contains CStrings as data */

	CSwoosh &operator=( CSwoosh & );

};

#endif // __SWOOSH_H
