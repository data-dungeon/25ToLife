/***********************************/
/*                                 */
/* Effect.h                        */
/* Pigpen particle tool  12/07/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle effect class           */
/*                                 */
/***********************************/

#ifndef __EFFECT_H
#define __EFFECT_H

/******************* includes ****************************/

#include "Sound.h"

/* ImportExport includes */

/* game engine includes */

#include "Game/Managers/ParticleDB.h"	// data export structures

/******************* defines *****************************/

/* file version. must be an integer. increment by 1 each time there is a version change in effect serialization */

#define EFFECT_VERSION		3

/* max size of effect name. name is treated as a CString for the most part, but there are times when it */
/* it's useful to be able to save off the string somewhere else for copying purposes */

#define EFFECT_MAX_NAME_LENGTH	100 

/* max particle channels per effect */

#define MAX_CHANNELS_PER_EFFECT	10

/* flag bits for ulFlags */

#define EFFECT_ASSOCIATED		0x00000001
#define EFFECT_SELECTED			0x00000002	// for serialization
#define EFFECT_NEW_ASSOCIATE	0x00000004
/*** always add to end of list-- don't reorder-- flags are serialized ***/

/******************* forward declarations ****************/

class CParticle;
class CImage;

/******************* structures **************************/

/* an "effect" can reference multiple particles. each particle has count and timing controls */

typedef struct
{
	CParticle *pParticle;	// not serialized-- converted to an particle index for storing, converted back to a pointer at load
	int nParticleIndex;		// used only for serialization-- doc sets this before storing effects CObArray or uses it after loading
	float fCount;				// number of times to repeat this particle. use float for convenience, but it's an int
	float fCountRandom;		// plus or minus. use float for convenience, but it's an int
	float fEmitDelay;			// delay before first particle fires, in seconds
	float fEmitDelayRandom;	// plus or minus
	float fEmitTime;			// if count > 1, total time to emit all particles (not including initial delay)
	float fEmitTimeRandom;	// plus or minus
	unsigned long ulFlags;	// per-channel flags
	char SoundName[MAX_SOUND_NAME_LENGTH];	// name of sound (from sound list, sounds are names only)
} ts_EffectChannel;

/******************* class *******************************/

class CEffect : public CObject
{
	/* give edit windows access to data members */

	friend class CEffectDialog;
	friend class CPigpenDoc;

public:

	/* this enables easy serialization of the CObArray-- you just call the array's Serialize function and all the */
	/* effect records serialize themselves, in order-- slick */

	DECLARE_SERIAL(CEffect)

// Constructor/destructor

	CEffect();
	~CEffect();

// Implementation

protected:

	/* keep serialization up-to-date! */

	CString m_Name;
	unsigned long m_ulVersion;
	unsigned long m_ulFlags;
	ts_EffectChannel m_Channels[MAX_CHANNELS_PER_EFFECT];	// make this public so dialog box can easily edit it

	/* obsolete, but kept in for serialization compatibility */

	unsigned long m_ulObsoleteRandomFlags;

	/* not serialized */

	int m_nTempIndex;						// used for export
	int m_nExportStatus;					// used for export
	int m_nExportDataSize;				// used for preview
	char m_NameString[EFFECT_MAX_NAME_LENGTH];	// for record copying purposes
	int m_nUsedCount;						// for use analysis
	void *m_pEngineData;					// for real-time update of particle data during particle preview
	int m_nChannelIndex;					// which channel is shown in effect dialog
	unsigned long m_ulLoadVersion;	// version number from serialization load, for possible fix-ups

	/* keep serialization up-to-date! */

public:

	/* simple routines */

	void SetName(char *pName)							{ m_Name = pName; }
	CString& GetName()									{ return(m_Name); }
	CParticle *GetChannelParticle(int nIndex)		{ if (ChannelIsSound(nIndex)) return(NULL); else return(m_Channels[nIndex].pParticle); }
	void SetTempIndex(int nIndex)						{ m_nTempIndex = nIndex; }
	int GetTempIndex(void)								{ return(m_nTempIndex); }
	void SetExportStatus(int nStatus)				{ m_nExportStatus = nStatus; }
	int GetExportStatus(void)							{ return(m_nExportStatus); }
	void SetExportDataSize(int nExportDataSize)	{ m_nExportDataSize = nExportDataSize; }
	int GetExportDataSize(void)						{ return(m_nExportDataSize); }
	void SetSelected(bool bSelected)					{ if (bSelected) m_ulFlags |= EFFECT_SELECTED; else m_ulFlags &= ~EFFECT_SELECTED; }
	bool GetSelected(void)								{ return((m_ulFlags & EFFECT_SELECTED) != 0); }
	bool ChannelIsSound(int nIndex)					{ return((m_Channels[nIndex].ulFlags & CHANNEL_IS_SOUND) != 0); }
	bool ChannelIsEmpty(int nIndex)					{ if (ChannelIsSound(nIndex)) return(strcmp(m_Channels[nIndex].SoundName, NO_SOUND_NAME) == 0); else return(m_Channels[nIndex].pParticle == NULL); }
	char *GetChannelSoundName(int nIndex)			{ return(m_Channels[nIndex].SoundName); }
	void SetChannelSoundName(int nIndex, char *pSoundName) { strcpy(m_Channels[nIndex].SoundName, pSoundName); }
	void SetUsedCount(int nCount)						{ m_nUsedCount = nCount; }
	int GetUsedCount(void)								{ return(m_nUsedCount); }
	void IncUsedCount(void)								{ m_nUsedCount++; }
	void SetEngineData(void *pData)					{ m_pEngineData = pData; }
	void *GetEngineData(void)							{ return(m_pEngineData); }
	int GetChannelIndex(void)							{ return(m_nChannelIndex); }
	unsigned long GetLoadVersion(void)				{ return(m_ulLoadVersion); }

	/* routines defined in .cpp */

	int FillExportBuffer(char *pBuffer, int nMaxSize, bool bPreview);
	int AnalyzeForExport(bool bPreview);
	void SetupDummy(CParticle *pParticle);
	void PrepareForMemcpy(void);
	void RestoreAfterMemcpy(void);
	CImage *GetChannelImage(int nIndex, bool *pbShowAlphaMask);
	CImage *GetImage(bool *pbShowAlphaMask);
	void LoadFixup(void);
	bool UsesParticle(CParticle *pParticle);

	/* association testing */

	bool IsAssociated()									{ return((m_ulFlags & EFFECT_ASSOCIATED) != 0); }
	bool IsNewAssociate()								{ return((m_ulFlags & EFFECT_NEW_ASSOCIATE) != 0); }
	void ClearAssociateFlags()							{ m_ulFlags &= ~(EFFECT_ASSOCIATED | EFFECT_NEW_ASSOCIATE); }
	void ClearNewAssociateFlag()						{ m_ulFlags &= ~EFFECT_NEW_ASSOCIATE; }
	void SetAssociateFlags()							{ m_ulFlags |= (EFFECT_ASSOCIATED | EFFECT_NEW_ASSOCIATE); }
	int TestAssociate(CObject *pTestObject, int nType);

	/* override serialization-- let object save itself out */

	virtual void Serialize(CArchive& ar);

	/* for more generic dialog processing, give access to addresses of data fields. data field id's are used */
	/* by dialog control lists */

	enum {	// because the enum is inside the class, it needs to be referenced as CEffect::DATA_XXX
		DATA_COUNT = 0,				// per channel
		DATA_COUNT_RANDOM,			// per channel
		DATA_EMIT_DELAY,				// per channel
		DATA_EMIT_DELAY_RANDOM,		// per channel
		DATA_EMIT_TIME,				// per channel
		DATA_EMIT_TIME_RANDOM,		// per channel
		DATA_FLAGS,						// per channel
		DATA_CHANNEL,
	
		// update GetDataPointer, below as you add more
	};

	void *GetDataPointer(int nDataID, int nDataIndex)
	{
		switch(nDataID)
		{
		case DATA_COUNT:
			ASSERT((nDataIndex >= 0) && (nDataIndex < MAX_CHANNELS_PER_EFFECT));
			return(&m_Channels[nDataIndex].fCount);
		case DATA_COUNT_RANDOM:
			ASSERT((nDataIndex >= 0) && (nDataIndex < MAX_CHANNELS_PER_EFFECT));
			return(&m_Channels[nDataIndex].fCountRandom);
		case DATA_EMIT_DELAY:
			ASSERT((nDataIndex >= 0) && (nDataIndex < MAX_CHANNELS_PER_EFFECT));
			return(&m_Channels[nDataIndex].fEmitDelay);
		case DATA_EMIT_DELAY_RANDOM:
			ASSERT((nDataIndex >= 0) && (nDataIndex < MAX_CHANNELS_PER_EFFECT));
			return(&m_Channels[nDataIndex].fEmitDelayRandom);
		case DATA_EMIT_TIME:
			ASSERT((nDataIndex >= 0) && (nDataIndex < MAX_CHANNELS_PER_EFFECT));
			return(&m_Channels[nDataIndex].fEmitTime);
		case DATA_EMIT_TIME_RANDOM:
			ASSERT((nDataIndex >= 0) && (nDataIndex < MAX_CHANNELS_PER_EFFECT));
			return(&m_Channels[nDataIndex].fEmitTimeRandom);
		case DATA_FLAGS:
			ASSERT((nDataIndex >= 0) && (nDataIndex < MAX_CHANNELS_PER_EFFECT));
			return(&m_Channels[nDataIndex].ulFlags);
		case DATA_CHANNEL:
			return(&m_nChannelIndex);
		}
		ASSERT(FALSE);
		return(NULL);
	}

// operations

public:

	/* overload = operator to make copying effects easy. it is NOT ok to do a memcpy because the class contains CStrings as data */

	CEffect &operator=( CEffect & );

};

#endif // __EFFECT_H
