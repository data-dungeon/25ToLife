/***********************************/
/*                                 */
/* Emitter.h                       */
/* Pigpen particle tool  01/22/01  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle emitter class          */
/*                                 */
/***********************************/

#ifndef __EMITTER_H
#define __EMITTER_H

/******************* includes ****************************/

/* ImportExport includes */

/* game engine includes */

#include "Game/Managers/ParticleDB.h"	// data export structures

/******************* defines *****************************/

/* file version. must be an integer. increment by 1 each time there is a version change in effect serialization */

#define EMITTER_VERSION		8

/* max size of effect name. name is treated as a CString for the most part, but there are times when it */
/* it's useful to be able to save off the string somewhere else for copying purposes */

#define EMITTER_MAX_NAME_LENGTH	100 

/* flag bits for ulFlags */

#define EMITTER_ASSOCIATED				0x00000001
#define EMITTER_SELECTED				0x00000002	// for serialization
#define EMITTER_MIN_DIST_ENABLED		0x00000004
#define EMITTER_MOVE_ONLY				0x00000008
#define EMITTER_SCALE_CONT_RANDOM	0x00000010
#define EMITTER_DELAY_CONT_RANDOM	0x00000020
#define EMITTER_NEW_ASSOCIATE			0x00000040
/*** always add to end of list-- don't reorder-- flags are serialized ***/

/******************* forward declarations ****************/

class CEffect;
class CImage;

/******************* global variables ********************/

extern char *pEmitterTypeString[];

/******************* structures **************************/

/******************* class *******************************/

class CEmitter : public CObject
{
	/* give edit windows access to data members */

	friend class CEmitterDialog;
	friend class CPigpenDoc;

public:

	/* this enables easy serialization of the CObArray-- you just call the array's Serialize function and all the */
	/* emitter records serialize themselves, in order-- slick */

	DECLARE_SERIAL(CEmitter)

// Constructor/destructor

	CEmitter();
	~CEmitter();

// Implementation

protected:

	/* keep serialization up-to-date! */

	CString m_Name;
	unsigned long m_ulVersion;
	unsigned long m_ulFlags;
	int m_nEffectIndex;					// used for serialization pointer-to-index conversion
	float m_fDelay;
	float m_fDelayRandomPct;
	float m_fScale;
	float m_fScaleRandomPct;
	float m_fVelPct;
	float m_fAdditiveVelPct;
	float m_fMinDistance;
	float m_fGoRadius;
	float m_fRadius;
	float m_fAngularVel;
	float m_fLifetime;					// lifetime (in seconds), 0 means forever
	float m_fLifetimeRandomPct;		// lifetime randomness
	float m_fLOD1Radius;					// emitter will emit at 100% of emitter rate nearer than lod radius 1
	float m_fLOD2Radius;					// emitter will emit at 0% of emitter rate farther than lod radius 2
	float m_fFluxSize;					// size of flux volume (it is a cube with each side <flux size> meters)
	int m_nType;
	float m_fInvisDistance;				// invisible up until this distance
	float m_fNearDistance;				// alpha in between invis distance and this distance, use min scale at min distance
	float m_fNearScale;					// scale to use at or before min distance
	float m_fFarDistance;				// interpolate scale between min distanc and max distance, use max scale at max distance
	float m_fFarScale;					// scale to use at or after max distance

	/* not serialized */

	CEffect *m_pEffect;					// effect that emitter uses, set at construction or serialization loading
	int m_nTempIndex;						// used for export
	int m_nExportStatus;					// used for export
	int m_nExportDataSize;				// used for preview
	char m_NameString[EMITTER_MAX_NAME_LENGTH];	// for record copying purposes
	int m_nUsedCount;						// for use analysis
	void *m_pEngineData;					// for real-time update of particle data during particle preview
	unsigned long m_ulLoadVersion;	// version number from serialization load, for possible fix-ups

	/* keep serialization up-to-date! */

public:

	/* simple routines */

	void SetEffect(CEffect *pEffect)					{ m_pEffect = pEffect; }
	CEffect *GetEffect(void)							{ return(m_pEffect); }
	void SetEffectIndex(int nIndex)					{ m_nEffectIndex = nIndex; }
	int GetEffectIndex(void)							{ return(m_nEffectIndex); }
	void SetName(char *pName)							{ m_Name = pName; }
	CString& GetName()									{ return(m_Name); }
	bool IsMinDistEnabled()								{ return((m_ulFlags & EMITTER_MIN_DIST_ENABLED) != 0); }
	bool IsMoveOnly()										{ return((m_ulFlags & EMITTER_MOVE_ONLY) != 0); }
	bool IsScaleContRandom()							{ return((m_ulFlags & EMITTER_SCALE_CONT_RANDOM) != 0); }
	bool IsDelayContRandom()							{ return((m_ulFlags & EMITTER_DELAY_CONT_RANDOM) != 0); }
	void SetIsMoveOnly(void)							{ m_ulFlags |= EMITTER_MOVE_ONLY; }
	void SetTempIndex(int nIndex)						{ m_nTempIndex = nIndex; }
	int GetTempIndex(void)								{ return(m_nTempIndex); }
	void SetExportStatus(int nStatus)				{ m_nExportStatus = nStatus; }
	int GetExportStatus(void)							{ return(m_nExportStatus); }
	void SetExportDataSize(int nExportDataSize)	{ m_nExportDataSize = nExportDataSize; }
	int GetExportDataSize(void)						{ return(m_nExportDataSize); }
	void SetSelected(bool bSelected)					{ if (bSelected) m_ulFlags |= EMITTER_SELECTED; else m_ulFlags &= ~EMITTER_SELECTED; }
	bool GetSelected(void)								{ return((m_ulFlags & EMITTER_SELECTED) != 0); }
	void SetUsedCount(int nCount)						{ m_nUsedCount = nCount; }
	int GetUsedCount(void)								{ return(m_nUsedCount); }
	void IncUsedCount(void)								{ m_nUsedCount++; }
	void SetType(int nType)								{ m_nType = nType; }
	int GetType(void)										{ return(m_nType); }
	void SetEngineData(void *pData)					{ m_pEngineData = pData; }
	void *GetEngineData(void)							{ return(m_pEngineData); }
	void SetDelay(float fDelay)						{ m_fDelay = fDelay; }
	float GetDelay(void)									{ return(m_fDelay); }
	unsigned long GetLoadVersion(void)				{ return(m_ulLoadVersion); }

	/* routines defined in .cpp */

	CString& GetEffectName(void);
	int FillExportBuffer(char *pBuffer, int nMaxSize, bool bPreview);
	int AnalyzeForExport(bool bPreview);
	void SetupDummy(CEffect *pEffect);
	void PrepareForMemcpy(void);
	void RestoreAfterMemcpy(void);
	CImage *GetImage(bool *pbShowAlphaMask);
	void LoadFixup(void);
	bool CheckLOD1Radius(void);
	bool CheckLOD2Radius(void);
	bool CheckInvisDistance(void);
	bool CheckNearDistance(void);
	bool CheckFarDistance(void);
	
	/* association testing */

	bool IsAssociated()									{ return((m_ulFlags & EMITTER_ASSOCIATED) != 0); }
	bool IsNewAssociate()								{ return((m_ulFlags & EMITTER_NEW_ASSOCIATE) != 0); }
	void ClearAssociateFlags()							{ m_ulFlags &= ~(EMITTER_ASSOCIATED | EMITTER_NEW_ASSOCIATE); }
	void ClearNewAssociateFlag()						{ m_ulFlags &= ~EMITTER_NEW_ASSOCIATE; }
	void SetAssociateFlags()							{ m_ulFlags |= (EMITTER_ASSOCIATED | EMITTER_NEW_ASSOCIATE); }
	int TestAssociate(CObject *pTestObject, int nType);

	/* override serialization-- let object save itself out */

	virtual void Serialize(CArchive& ar);

	/* for more generic dialog processing, give access to addresses of data fields. data field id's are used */
	/* by dialog control lists */

	enum {	// because the enum is inside the class, it needs to be referenced as CEmitter::DATA_XXX
		DATA_FLAGS = 0,
		DATA_DELAY,
		DATA_DELAY_RANDOM,
		DATA_SCALE,
		DATA_SCALE_RANDOM,
		DATA_VEL_PCT,
		DATA_ADD_VEL_PCT,
		DATA_MIN_DISTANCE,
		DATA_GO_RADIUS,
		DATA_RADIUS,
		DATA_ANGULAR_VEL,
		DATA_LIFE,
		DATA_LIFE_RANDOM,
		DATA_LOD1_RADIUS,
		DATA_LOD2_RADIUS,
		DATA_FLUX_SIZE,
		DATA_INVIS_DISTANCE,
		DATA_NEAR_DISTANCE,
		DATA_NEAR_SCALE,
		DATA_FAR_DISTANCE,
		DATA_FAR_SCALE,
	
		// update GetDataPointer, below as you add more
	};

	void *GetDataPointer(int nDataID)
	{
		switch(nDataID)
		{
		case DATA_FLAGS:
			return(&m_ulFlags);
		case DATA_DELAY:
			return(&m_fDelay);
		case DATA_DELAY_RANDOM:
			return(&m_fDelayRandomPct);
		case DATA_SCALE:
			return(&m_fScale);
		case DATA_SCALE_RANDOM:
			return(&m_fScaleRandomPct);
		case DATA_VEL_PCT:
			return(&m_fVelPct);
		case DATA_ADD_VEL_PCT:
			return(&m_fAdditiveVelPct);
		case DATA_MIN_DISTANCE:
			return(&m_fMinDistance);
		case DATA_GO_RADIUS:
			return(&m_fGoRadius);
		case DATA_RADIUS:
			return(&m_fRadius);
		case DATA_ANGULAR_VEL:
			return(&m_fAngularVel);
		case DATA_LIFE:
			return(&m_fLifetime);
		case DATA_LIFE_RANDOM:
			return(&m_fLifetimeRandomPct);
		case DATA_LOD1_RADIUS:
			return(&m_fLOD1Radius);
		case DATA_LOD2_RADIUS:
			return(&m_fLOD2Radius);
		case DATA_FLUX_SIZE:
			return(&m_fFluxSize);
		case DATA_INVIS_DISTANCE:
			return(&m_fInvisDistance);
		case DATA_NEAR_DISTANCE:
			return(&m_fNearDistance);
		case DATA_NEAR_SCALE:
			return(&m_fNearScale);
		case DATA_FAR_DISTANCE:
			return(&m_fFarDistance);
		case DATA_FAR_SCALE:
			return(&m_fFarScale);
		}
		ASSERT(FALSE);
		return(NULL);
	}

// operations

public:

	/* overload = operator to make copying effects easy. it is NOT ok to do a memcpy because the class contains CStrings as data */

	CEmitter &operator=( CEmitter & );

};

#endif // __EMITTER_H
