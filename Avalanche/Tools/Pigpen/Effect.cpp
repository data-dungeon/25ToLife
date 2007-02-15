/***********************************/
/*                                 */
/* Effect.cpp                      */
/* Pigpen particle tool  12/07/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle effect class           */
/*                                 */
/***********************************/

/* a particle effect is a set of one or more particles, with timing/randomness info */

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

/* this enables easy serialization of the CObArray-- you just call the array's Serialize function and all the
effect records serialize themselves, in order-- slick */

IMPLEMENT_SERIAL(CEffect, CObject, 0)

/////////////////////////////////////////////////////////////////////////////
// CEffect construction

CEffect::CEffect()
{
	m_ulVersion = EFFECT_VERSION;
	m_ulFlags = 0;
	m_ulObsoleteRandomFlags = 0;	// now obsolete, but retained for serialization compatibility
	m_Name = "New";
	memset(m_Channels, 0, MAX_CHANNELS_PER_EFFECT * sizeof(ts_EffectChannel));
	for (int i = 0; i < MAX_CHANNELS_PER_EFFECT; i++)
	{
		m_Channels[i].nParticleIndex = -1;
		strcpy(m_Channels[i].SoundName, NO_SOUND_NAME);
	}
	m_nUsedCount = -1;	// unknown status until analysis is done
	m_pEngineData = NULL;
	m_nExportDataSize = 0;
	m_nChannelIndex = 0;
}
CEffect::~CEffect()
{
}

/////////////////////////////////////////////////////////////////////////////
// CEffect serialization

void CEffect::Serialize(CArchive& ar)
{
	int i;
	int ChannelCount;

	if (ar.IsStoring())
	{
		ASSERT(m_ulVersion == EFFECT_VERSION); 
		ar << m_ulVersion;

		/* version 3-- changed MAX_CHANNELS_PER_EFFECT, no additional processing necessary */

		/* version 2 */

		/* still more channel data */

		ChannelCount = MAX_CHANNELS_PER_EFFECT;
		ar << ChannelCount;
		for (i = 0; i < MAX_CHANNELS_PER_EFFECT; i++)
			ar.Write(m_Channels[i].SoundName, MAX_SOUND_NAME_LENGTH);
	
		/* version 1 */

		/* more channel data */

		ChannelCount = MAX_CHANNELS_PER_EFFECT;
		ar << ChannelCount;
		for (i = 0; i < MAX_CHANNELS_PER_EFFECT; i++)
			ar << m_Channels[i].ulFlags;
	
		/* version 0 */

		ar << m_ulFlags;
		ar << m_ulObsoleteRandomFlags;	// now obsolete, but retained for serialization compatibility
		ar << m_Name;

		/* channels */

		ChannelCount = MAX_CHANNELS_PER_EFFECT;
		ar << ChannelCount;
		for (i = 0; i < MAX_CHANNELS_PER_EFFECT; i++)
		{
			ar << m_Channels[i].nParticleIndex;	// index into particle list
			ar << m_Channels[i].fCount;
			ar << m_Channels[i].fCountRandom;
			ar << m_Channels[i].fEmitDelay;
			ar << m_Channels[i].fEmitDelayRandom;
			ar << m_Channels[i].fEmitTime;
			ar << m_Channels[i].fEmitTimeRandom;
		}
	}
	else
	{
		ar >> m_ulVersion;
		if (m_ulVersion > EFFECT_VERSION)
		{
			ASSERT(FALSE);	// shouldn't happen since later version should be caught by doc serialization
			if (PigpenApp.IsCommandLineMode())
				PigpenApp.SetReturnCode(CPigpenApp::RETURN_CODE_INVALID_VERSION);
			else
				AfxMessageBox("Loading effects.\nThis executable cannot load this version of Pigpen file.\nIt was saved by a later version of the executable.\nPlease update your Pigpen.exe.", MB_OK | MB_ICONEXCLAMATION);
			AfxThrowArchiveException(CArchiveException::generic);	// abort
		}

		/* as we add new versions, process latest version and then fall through to earlier versions */

		switch(m_ulVersion)
		{
		case 3:
			/* changed MAX_CHANNELS_PER_EFFECT, no extra data */
			
			/* fall through */
		case 2:
			/* still more channel data */

			ar >> ChannelCount;
			for (i = 0; i < ChannelCount; i++)
			{
				if (i < MAX_CHANNELS_PER_EFFECT)
					ar.Read(m_Channels[i].SoundName, MAX_SOUND_NAME_LENGTH);
				else
				{
					/* throw away extra channel data! */

					ASSERT(false);
					char Name[MAX_SOUND_NAME_LENGTH];
					ar.Read(Name, MAX_SOUND_NAME_LENGTH);	// m_Channels[i].SoundName
				}
			}
	
			/* fall through */
		case 1:
			/* more channel data */

			ar >> ChannelCount;
			for (i = 0; i < ChannelCount; i++)
			{
				if (i < MAX_CHANNELS_PER_EFFECT)
					ar >> m_Channels[i].ulFlags;
				else
				{
					/* throw away extra channel data! */

					ASSERT(false);
					unsigned long ulValue;
					ar >> ulValue;	// m_Channels[i].ulFlags;
				}
			}

			/* fall through */
		case 0:
			/* version 0 */

			ar >> m_ulFlags;
			ar >> m_ulObsoleteRandomFlags;	// now obsolete, but retained for serialization compatibility
			ar >> m_Name;

			/* channels */

			ar >> ChannelCount;
			for (i = 0; i < ChannelCount; i++)
			{
				if (i < MAX_CHANNELS_PER_EFFECT)
				{
					ar >> m_Channels[i].nParticleIndex;	// gets translated to particle pointer by CPigpenDoc serialization
					ar >> m_Channels[i].fCount;
					ar >> m_Channels[i].fCountRandom;
					ar >> m_Channels[i].fEmitDelay;
					ar >> m_Channels[i].fEmitDelayRandom;
					ar >> m_Channels[i].fEmitTime;
					ar >> m_Channels[i].fEmitTimeRandom;
				}
				else
				{
					/* throw away extra channel data! */

					ASSERT(false);
					int nValue;
					float fValue;
					ar >> nValue;	// m_Channels[i].nParticleIndex;
					ar >> fValue;	// m_Channels[i].fCount;
					ar >> fValue;	// m_Channels[i].fCountRandom;
					ar >> fValue;	// m_Channels[i].fEmitDelay;
					ar >> fValue;	// m_Channels[i].fEmitDelayRandom;
					ar >> fValue;	// m_Channels[i].fEmitTime;
					ar >> fValue;	// m_Channels[i].fEmitTimeRandom;
				}
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}

		/* clear associated flag on load-- it's temporary */

		m_ulFlags &= ~EFFECT_ASSOCIATED;

		/* now using latest version */

		m_ulLoadVersion = m_ulVersion;	// save version from load, for possible fix-ups
		m_ulVersion = EFFECT_VERSION;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEffect implementation

/* fill in data for export. return size of data, 0 if error or overflow */

int CEffect::FillExportBuffer(char *pBuffer, int nMaxSize, bool bPreview)
{
	/* should have valid data at this point */

	ASSERT(GetExportStatus() == EXPORT_STATUS_VALID);

	/* export non-empty channels */

	ts_Effect *pEffectData = (ts_Effect *) pBuffer;
	u32 u32ChannelCount = 0;
	for (int i = 0; i < MAX_CHANNELS_PER_EFFECT; i++)
	{
		/* point to data destination */

		ts_Channel *pChannelData = &pEffectData->ChannelData[u32ChannelCount];

		/* particle or sound? */

		bool bValidData = FALSE;
		if (ChannelIsSound(i))
		{
			int nListIndex;

			if ((nListIndex = GetSoundListIndex(m_Channels[i].SoundName)) != 0)	// 0 is no sound
			{
				/* valid sound */

				bValidData = TRUE;
				pChannelData->u32SoundIndex = GetSoundIndex(nListIndex);
			}
		}
		else
		{
			CParticle *pParticle = GetChannelParticle(i);

			/* preview check */

			if ((pParticle) && (pParticle->GetExportStatus() == EXPORT_STATUS_UNKNOWN))
			{
				ASSERT(bPreview);	// this should only happen in preview mode when changing a particle's graphic
				return(0);			// need to re-build preview .dbl
			}

			/* add particle if valid */

			if ((pParticle) && (pParticle->GetExportStatus() == EXPORT_STATUS_VALID))
			{
				/* valid particle */

				bValidData = TRUE;
				pChannelData->u32ParticleIndex = pParticle->GetTempIndex();
				ASSERT(pChannelData->u32ParticleIndex != -1);
			}
		}

		/* add emission data if called for */

		if (bValidData)
		{
			/* valid channel. add it to export data */

//yuch-- make floats bytes? shorts?
			pChannelData->fEmitDelay = m_Channels[i].fEmitDelay;	// delay before first particle is emitted
			pChannelData->fEmitTime = m_Channels[i].fEmitTime;		// total time to emit all particles, not including initial delay
			pChannelData->u8EmitCount = (u8) m_Channels[i].fCount;
			ASSERT((m_Channels[i].ulFlags & ~0xff) == 0);	// 8 bits available		
			pChannelData->u8Flags = (u8) m_Channels[i].ulFlags;
			ASSERT(pChannelData->u8EmitCount != 0);
			if (pChannelData->u8EmitCount == 1)
				pChannelData->fEmitTime = 0.0f;	// just to be clean, non-zero emit time is meaningless for a single particle

			/* maybe endian swap it */

			ts_Channel *pChannelDataSwapped = DBLUtil__SwapEndian(pChannelData);
			if (pChannelDataSwapped != pChannelData)
				memcpy(pChannelData, pChannelDataSwapped, sizeof(ts_Channel));	// if swapped, copy swapped version back into export buffer

			/* another channel added */

			u32ChannelCount++;
		}
	}

	/* record channel count-- need a variable outside the record since record may get endian swapped and i need */
	/* the count for calculating the export buffer size, in the SetExportDataSize call below */

	pEffectData->u32ChannelCount = u32ChannelCount;

	/* maybe endian header portion of ts_Effect */

	ts_Effect *pEffectDataSwapped = DBLUtil__SwapEndian(pEffectData);
	if (pEffectDataSwapped != pEffectData)
		memcpy(pEffectData, pEffectDataSwapped, sizeof(ts_Effect) - sizeof(ts_Channel));	// if swapped, copy swapped version back into export buffer-- header only, channels get swapped individually

	/* in preview mode, always return a full-size buffer even if it is not filled so that we can update in realtime */

	if (bPreview)
		u32ChannelCount = MAX_CHANNELS_PER_EFFECT;

	/* record and return size of data that got filled in. "- 1" is because ts_EffectData contains one channel already */

	SetExportDataSize(sizeof(ts_Effect) + (u32ChannelCount - 1) * sizeof(ts_Channel));
	ASSERT(GetExportDataSize() <= nMaxSize);	// if this hits, we've overrun our budget-- probably a preview thing?
	return(GetExportDataSize());
}

/* determine if effect is valid for export. in an invalid effect, each channel is empty or points to an empty
particle. the analysis of the database as a whole works by trickle-down, so check all channels even once we
know that the effect is valid so that the particles have a chance to analyze themselves */

int CEffect::AnalyzeForExport(bool bPreview)
{
	if (GetExportStatus() == EXPORT_STATUS_UNKNOWN)
	{
		/* analyze data */

		SetExportStatus(EXPORT_STATUS_INVALID);	// assume invalid
		for (int i = 0; i < MAX_CHANNELS_PER_EFFECT; i++)
		{
			if (ChannelIsSound(i))
			{
				SetExportStatus(EXPORT_STATUS_VALID);
				IncUsedCount();
			}
			else
			{
				CParticle *pParticle = GetChannelParticle(i);
				if ((pParticle) && (pParticle->AnalyzeForExport(bPreview) == EXPORT_STATUS_VALID))
				{
					SetExportStatus(EXPORT_STATUS_VALID);
					IncUsedCount();
				}
			}
		}
		if (GetExportStatus() == EXPORT_STATUS_INVALID)
		{
			char acMessage[100];
			sprintf(acMessage, "Effect: %s", GetName().GetBuffer(10));
			AddConditionalInfoToErrorLog(acMessage);
			HANDLE_ERROR(RETURN_EMPTY_EFFECT);
		}
	}
	return(GetExportStatus());
}

/* set up a dummy effect for previewing an individual particle. one channel, one particle, no delay.
very simple */

void CEffect::SetupDummy(CParticle *pParticle)
{
	m_Channels[0].pParticle = pParticle;
	m_Channels[0].fCount = 1.0f;
}

/* we can use memcpy to copy records as long as we do some prep work first. for example, memcpying a cstring is not
a happy thing */

void CEffect::PrepareForMemcpy(void)
{
	/* save contents of name */

	strncpy(m_NameString, m_Name.GetBuffer(10), EFFECT_MAX_NAME_LENGTH);
	m_NameString[EFFECT_MAX_NAME_LENGTH - 1] = 0;

	/* free up CString */

	SetName("");
	m_Name.ReleaseBuffer();
	m_Name.FreeExtra();
}
void CEffect::RestoreAfterMemcpy(void)
{
	/* set CString from temporary string */

	SetName(m_NameString);
}

/* return image associated with channel, if any */

CImage *CEffect::GetChannelImage(int nIndex, bool *pbShowAlphaMask)
{
	if (ChannelIsSound(nIndex) == FALSE)	// sound channels don't have an image
	{
		if (m_Channels[nIndex].pParticle)
			return(m_Channels[nIndex].pParticle->GetImage(pbShowAlphaMask));
	}
	return(NULL);
}

/* look for a particle channel that has a valid icon image */

CImage *CEffect::GetImage(bool *pbShowAlphaMask)
{
	for (int i = 0; i < MAX_CHANNELS_PER_EFFECT; i++)
	{
		CImage *pImage = GetChannelImage(i, pbShowAlphaMask);
		if (pImage)
			return(pImage);
	}
	return(NULL);
}

/* do load-time fix-up on old versions of data. all pointers, etc have been resolved */

void CEffect::LoadFixup(void)
{
	/* currently no fix-up required for effect */
}

/* does effect use passed particle? */

bool CEffect::UsesParticle(CParticle *pParticle)
{
	ASSERT(pParticle);	// meaningless otherwise
	for (int i = 0; i < MAX_CHANNELS_PER_EFFECT; i++)
	{
		if (pParticle == GetChannelParticle(i))
			return(true);
	}
	return(false);
}

/* test if data object is associated with passed data object, for association testing. returns true if a new association is found */

int CEffect::TestAssociate(CObject *pTestObject, int nType)
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
		/* does this effect use passed particle? */

		if (UsesParticle((CParticle *) pTestObject))
		{
			SetAssociateFlags();	// sets both associated and new-associate flags
			return(ASSOCIATE_PARTICLE_USED_BY_EFFECT);
		}

		/* is this effect one of the passed particle's spawn effects? */

		int i;
		for (i = 0; i < ((CParticle *) pTestObject)->GetSpawnCount(); i++)
		{
			if (((CParticle *) pTestObject)->GetSpawn(i)->GetEffect() == this)
			{
				SetAssociateFlags();	// sets both associated and new-associate flags
				return(ASSOCIATE_PARTICLE_SPAWNS_EFFECT);
			}
		}
		break;
	case GRAPHICS:
		break;
	case EMITTERS:
		/* does passed emitter use this effect? */

		if (((CEmitter *) pTestObject)->GetEffect() == this)
		{
			SetAssociateFlags();	// sets both associated and new-associate flags
			return(ASSOCIATE_EMITTER_USES_EFFECT);
		}
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
// CEffect operations

/* copy one effect to another */

CEffect &CEffect::operator=( CEffect &Source )
{
	/* prepare records for memcpy-- this basically tidies up CStrings */

	PrepareForMemcpy();
	Source.PrepareForMemcpy();

	/* do the memcpy */

	memcpy(this, &Source, sizeof(CEffect));

	/* clean up after memcpy */

	RestoreAfterMemcpy();
	Source.RestoreAfterMemcpy();
	return *this;
}

