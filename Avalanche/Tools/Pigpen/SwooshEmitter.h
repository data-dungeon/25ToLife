/***********************************/
/*                                 */
/* SwooshEmitter.h                 */
/* Pigpen particle tool  11/24/02  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* swoosh emitter class            */
/*                                 */
/***********************************/

#ifndef __SWOOSHEMITTER_H
#define __SWOOSHEMITTER_H

/******************* includes ****************************/

#include "Swoosh.h"

/* ImportExport includes */

/* game engine includes */

#include <Game/Managers/ParticleDB.h>

/******************* defines *****************************/

/* file version. must be an integer. increment by 1 each time there is a version change in swoosh emitter serialization */

#define SWOOSH_EMITTER_VERSION		2

/* max size of swoosh emitter name. name is treated as a CString for the most part, but there are times when it */
/* it's useful to be able to save off the string somewhere else for copying purposes */

#define SWOOSH_EMITTER_MAX_NAME_LENGTH		100 

/* flag bits for m_u32Flags. have to avoid conflict with engine swoosh emitter flags enumerated in ParticleDB.h, so use */
/* "SWOO_" instead of "SWOOSH_" */

#define SWOO_EMITTER_ASSOCIATED			0x00000001
#define SWOO_EMITTER_SELECTED				0x00000002	// for serialization
#define SWOO_EMITTER_SPARSE_CYLINDER	0x00000004	// swooshes start randomly in cylinder and disappear after a while, they don't start at bottom and go all the way up
#define SWOO_EMITTER_NEW_ASSOCIATE		0x08000008
/*** always add to end of list-- don't reorder-- flags are serialized ***/

#if 0
/* flag bits for m_u32RandomFlags */

#define SWOO_FPS_CONT_RANDOM				0x00000001
#define SWOO_RANDOM_START_FRAME			0x00000002

/* values and masks for m_u32Bitfields (radio button bit fields) */

#define SWOO_ANIM_SHIFT						0
#define SWOO_ANIM_LOOP						(0x00000000 << SWOO_ANIM_SHIFT)
#define SWOO_ANIM_PING_PONG				(0x00000001 << SWOO_ANIM_SHIFT)
#define SWOO_ANIM_ONE_SHOT					(0x00000002 << SWOO_ANIM_SHIFT)
#define SWOO_ANIM_MASK						(0x00000003 << SWOO_ANIM_SHIFT)

#define SWOO_COLOR_SHIFT					(SWOO_ANIM_SHIFT + 2)	// skip over anim mask
#define SWOO_COLOR_PAIR						(0x00000000 << SWOO_COLOR_SHIFT)
#define SWOO_COLOR_RGB						(0x00000001 << SWOO_COLOR_SHIFT)
#define SWOO_COLOR_MASK						(0x00000001 << SWOO_COLOR_SHIFT)
#endif //0

/******************* structures **************************/

/******************* forward declarations ****************/

/******************* global variables ********************/

extern char *pSwooshEmitterTypeString[];

/******************* class *******************************/

class CPigpenSwooshEmitter : public CObject
{
public:

	/* this enables easy serialization of the CObArray-- you just call array's Serialize function and all the */
	/* swoosh records serialize themselves, in order-- slick */

	DECLARE_SERIAL(CPigpenSwooshEmitter)

// Constructor/destructor

	CPigpenSwooshEmitter();
	~CPigpenSwooshEmitter();

// Implementation

protected:

	/* keep serialization up-to-date! */

	CString m_Name;
	unsigned long m_ulVersion;
	unsigned long m_u32Flags;
	int m_nSwooshIndex;					// used for serialization pointer-to-index conversion
	int m_nType;
	float m_fLifetime;
	float m_fLifetimeRandomPct;

	/* cylinder type values */

	float m_fCylinderRadius;
	float m_fCylinderRadiusRandomPct;
	float m_fCylinderHeight;
	float m_fCylinderSwooshYVel;
	float m_fCylinderSwooshYVelRandomPct;
	float m_fCylinderSwooshRotVel;
	float m_fCylinderDeltaTime;
	float m_fCylinderDeltaAngle;
	float m_fCylinderSwooshWidth;
	float m_fCylinderSwooshWidthRandomPct;
	float m_fCylinderPointsOnCircle;
	float m_fCylinderSwooshLife;
	float m_fCylinderSwooshLifeRandomPct;
	float m_fCylinderPointsAtOnce;	// obsolete
	
	/* not serialized */

	CSwoosh *m_pSwoosh;					// swoosh to emit, set at construction or serialization loading
	int m_nTempIndex;						// used for export
	int m_nExportStatus;					// used for export
	int m_nExportDataSize;				// used for preview
	u32 m_u32TestFlags;					// used for export error-checking
	void *m_pEngineData;					// for real-time update of swoosh data during swoosh preview
	char m_NameString[SWOOSH_EMITTER_MAX_NAME_LENGTH];	// for record copying purposes
	int m_nUsedCount;						// for use analysis
	unsigned long m_ulLoadVersion;	// version number from serialization load, for possible fix-ups

	/* keep serialization up-to-date! */

public:

	/* simple routines */

	CString& GetName(void)								{ return(m_Name); }
	void SetName(char *pName)							{ m_Name = pName; }
	bool IsSparseCylinder(void)						{ return((m_u32Flags & SWOO_EMITTER_SPARSE_CYLINDER) != 0); }
	void SetSwoosh(CSwoosh *pSwoosh)					{ m_pSwoosh = pSwoosh; }
	CSwoosh *GetSwoosh(void)							{ return(m_pSwoosh); }
	void SetSwooshIndex(int nIndex)					{ m_nSwooshIndex = nIndex; }
	int GetSwooshIndex(void)							{ return(m_nSwooshIndex); }
	void SetType(int nType)								{ m_nType = nType; }
	int GetType(void)										{ return(m_nType); }
	void SetTempIndex(int nIndex)						{ m_nTempIndex = nIndex; }
	int GetTempIndex(void)								{ return(m_nTempIndex); }
	void SetExportStatus(int nStatus)				{ m_nExportStatus = nStatus; }
	int GetExportStatus(void)							{ return(m_nExportStatus); }
	void SetExportDataSize(int nSize)				{ m_nExportDataSize = nSize; }
	int GetExportDataSize(void)						{ return(m_nExportDataSize); }
	void SetSelected(bool bSelected)					{ if (bSelected) m_u32Flags |= SWOO_EMITTER_SELECTED; else m_u32Flags &= ~SWOO_EMITTER_SELECTED; }
	bool GetSelected(void)								{ return((m_u32Flags & SWOO_EMITTER_SELECTED) != 0); }
	void SetEngineData(void *pData)					{ m_pEngineData = pData; }
	void *GetEngineData(void)							{ return(m_pEngineData); }
	void SetUsedCount(int nCount)						{ m_nUsedCount = nCount; }
	int GetUsedCount(void)								{ return(m_nUsedCount); }
	void IncUsedCount(void)								{ m_nUsedCount++; }
	void SetFlag(u32 u32FlagBit)						{ m_u32Flags |= u32FlagBit; }
	void ClearFlag(u32 u32FlagBit)					{ m_u32Flags &= ~u32FlagBit; }
	bool IsFlagSet(u32 u32FlagBit)					{ return((m_u32Flags & u32FlagBit) == u32FlagBit); }	// can be more than one bit passed, will check for all bits set
	unsigned long GetLoadVersion(void)				{ return(m_ulLoadVersion); }
	bool CheckPointsAtOnce()							{ if (m_fCylinderPointsAtOnce > m_fCylinderPointsOnCircle) { m_fCylinderPointsAtOnce = m_fCylinderPointsOnCircle; return(true); } else return(false); }
	bool CheckPointsOnCircle()							{ if (m_fCylinderPointsOnCircle < m_fCylinderPointsAtOnce) { m_fCylinderPointsOnCircle = m_fCylinderPointsAtOnce; return(true); } else return(false);	}
	
	/* routines defined in .cpp */

	CString& GetSwooshName(void);
	int FillExportBuffer(char *pBuffer, int nMaxSize, bool bPreview);
	int AnalyzeForExport(bool bPreview);
	void SetupDummy(CSwoosh *pSwoosh);
	void PrepareForMemcpy(void);
	void RestoreAfterMemcpy(void);
	CImage *GetImage(bool *pbShowAlphaMask);
	void LoadFixup(void);

	/* association testing */

	bool IsAssociated()									{ return((m_u32Flags & SWOO_EMITTER_ASSOCIATED) != 0); }
	bool IsNewAssociate()								{ return((m_u32Flags & SWOO_EMITTER_NEW_ASSOCIATE) != 0); }
	void ClearAssociateFlags()							{ m_u32Flags &= ~(SWOO_EMITTER_ASSOCIATED | SWOO_EMITTER_NEW_ASSOCIATE); }
	void ClearNewAssociateFlag()						{ m_u32Flags &= ~SWOO_EMITTER_NEW_ASSOCIATE; }
	void SetAssociateFlags()							{ m_u32Flags |= (SWOO_EMITTER_ASSOCIATED | SWOO_EMITTER_NEW_ASSOCIATE); }
	int TestAssociate(CObject *pTestObject, int nType);

	/* override serialization-- let object save itself out */

	virtual void Serialize(CArchive& ar);

	/* for more generic dialog processing, give access to addresses of data fields. data field id's are used */
	/* by dialog control lists */

	enum {	// because the enum is inside the class, it needs to be referenced as CEmitter::DATA_XXX
		DATA_FLAGS = 0,
		DATA_CYL_RADIUS,
		DATA_CYL_RADIUS_RANDOM_PCT,
		DATA_CYL_HEIGHT,
		DATA_CYL_YVEL,
		DATA_CYL_YVEL_RANDOM_PCT,
		DATA_CYL_ROTVEL,
		DATA_CYL_DELTA_TIME,
		DATA_CYL_DELTA_ANGLE,
		DATA_CYL_SWOOSH_WIDTH,
		DATA_CYL_SWOOSH_WIDTH_RANDOM_PCT,
		DATA_CYL_CIRCLE_POINTS,
		DATA_CYL_LIFE,
		DATA_CYL_LIFE_RANDOM_PCT,
	
		// update GetDataPointer, below as you add more
	};

	void *GetDataPointer(int nDataID)
	{
		switch(nDataID)
		{
		case DATA_FLAGS:
			return(&m_u32Flags);
		case DATA_CYL_RADIUS:
			return(&m_fCylinderRadius);
		case DATA_CYL_RADIUS_RANDOM_PCT:
			return(&m_fCylinderRadiusRandomPct);
		case DATA_CYL_HEIGHT:
			return(&m_fCylinderHeight);
		case DATA_CYL_YVEL:
			return(&m_fCylinderSwooshYVel);
		case DATA_CYL_YVEL_RANDOM_PCT:
			return(&m_fCylinderSwooshYVelRandomPct);
		case DATA_CYL_ROTVEL:
			return(&m_fCylinderSwooshRotVel);
		case DATA_CYL_DELTA_TIME:
			return(&m_fCylinderDeltaTime);
		case DATA_CYL_DELTA_ANGLE:
			return(&m_fCylinderDeltaAngle);
		case DATA_CYL_SWOOSH_WIDTH:
			return(&m_fCylinderSwooshWidth);
		case DATA_CYL_SWOOSH_WIDTH_RANDOM_PCT:
			return(&m_fCylinderSwooshWidthRandomPct);
		case DATA_CYL_CIRCLE_POINTS:
			return(&m_fCylinderPointsOnCircle);
		case DATA_CYL_LIFE:
			return(&m_fCylinderSwooshLife);
		case DATA_CYL_LIFE_RANDOM_PCT:
			return(&m_fCylinderSwooshLifeRandomPct);
		}
		ASSERT(FALSE);
		return(NULL);
	}

// operations

public:

	/* overload = operator to make copying effects easy. it is NOT ok to do a memcpy because the class contains CStrings as data */

	CPigpenSwooshEmitter &operator=( CPigpenSwooshEmitter & );

};

#endif // __SWOOSHEMITTER_H
