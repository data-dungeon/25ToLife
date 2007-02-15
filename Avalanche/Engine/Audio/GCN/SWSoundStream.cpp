/***************************************************************************/
// SWSoundStream, streams audio via software
/***************************************************************************/

/******************* includes ****************************/

#include "Audio/AudioPCH.h"

#include "Layers/medialow.h"

static void _MakeGCNFileName(char *baseFileName, char *realFileName);

/***************************************************************************/
// The driver must create the sound, but it can be deleted directly
/***************************************************************************/
SWSoundStream::~SWSoundStream( void)
{
	m_pDriver->ClearSWSoundStream( );

	for( int i = 0; i < 2; i++)
	{
		if( m_sb[i].bReadPending)
		{
			DVDCancel( (DVDCommandBlock *)&m_sb[i].dvdfi);
		}

		if( m_sb[i].bFileOpened)
		{
			DVDClose( &m_sb[i].dvdfi);
		}

		if( m_sb[i].pBuffer != NULL)
		{
			memFree( m_sb[i].pBuffer);
		}
	}

	if( m_pAXVPB != NULL)
	{
		AXFreeVoice( m_pAXVPB);
	}
}

/***************************************************************************/
/***************************************************************************/
bool SWSoundStream::Play( void)
{
	if( m_pAXVPB == NULL)
	{
		return true;
	}

	if( m_bInitialized)
	{
		m_bPlaying = true;
		AXSetVoiceState( m_pAXVPB, AX_PB_STATE_RUN);
		m_bPlayQueued = false;
	}
	else
	{
		m_bPlayQueued = true;
	}

	return true;
}

/***************************************************************************/
// Return the play position, returns -1 if still preloading
/***************************************************************************/
int SWSoundStream::CurrentPlayPos()
{
	return( m_SamplesPlayed);
}

/***************************************************************************/
/***************************************************************************/
bool SWSoundStream::Stop( bool rewind)
{
	if( m_pAXVPB == NULL)
	{
		return true;
	}
	m_bPlaying = false;
	m_bPlayQueued = false;

	AXSetVoiceState( m_pAXVPB, AX_PB_STATE_STOP);
	return true;
}

void SWSoundStream::StopLeavePlaying( void)
{
	if( m_pAXVPB == NULL)
	{
		return;
	}

	AXSetVoiceState( m_pAXVPB, AX_PB_STATE_STOP);
}
/***************************************************************************/
/***************************************************************************/
bool SWSoundStream::IsPlaying( void)
{

	s32			status = DVDGetDriveStatus( );

	t_Error		error = MediaHAL::TranslateError( status, MED_ERR_FATAL);

	if( error != MED_ERR_NONE && m_medError == MED_ERR_NONE)
	{
		if( m_bPlaying)
		{
			StopLeavePlaying( );
			if( m_sb[0].bReadPending)
			{
				m_BuffersNeeded++;
			}
			if( m_sb[1].bReadPending)
			{
				m_BuffersNeeded++;
			}
		}
	}
	else
	if( error == MED_ERR_NONE && m_medError != MED_ERR_NONE && m_BuffersNeeded == 0)
	{
		if( m_bPlaying)
		{
			Play( );
		}
	}

	m_medError = error;

	return (m_bPlayQueued || m_bPlaying);
}

/***************************************************************************/
// Rewind the sound
/***************************************************************************/
bool SWSoundStream::Rewind( void)
{
	return true;
}

/***************************************************************************/
// Set the volume [0..1], 0.0 is silent 1.0 is full volume
/***************************************************************************/
bool SWSoundStream::SetVolume( float volume)
{
	if( m_pAXVPB == NULL)
	{
		return true;
	}

	MIXSetInput( m_pAXVPB, SoundInstance::HectodBToGCNdB( SoundVolume::UnitToHectodB( volume)));
	return true;
}

/***************************************************************************/
// our constructor
/***************************************************************************/
SWSoundStream::SWSoundStream( const char *streamName, char *fileName, bool wantRepeat) :
SoundStream(streamName)
{

	char		acNewName[256];

	m_bStreamQueued = false;
	m_bHeaderRead = false;
	m_bInitialized = false;
	m_bPlayQueued = false;
	m_bPlaying = false;
	m_pAXVPB = NULL;
	m_medError = MED_ERR_NONE;

	m_BuffersNeeded = 0;

	memset( &m_sb[0], 0, sizeof m_sb[0]);
	memset( &m_sb[1], 0, sizeof m_sb[1]);

	_MakeGCNFileName( fileName, acNewName);

#ifdef DVDETH
	return;
#endif

	m_EntryNum = DVDConvertPathToEntrynum( acNewName);
	if( m_EntryNum == -1)
	{
		return;
	}

	if( !InitStreamBuffer( m_EntryNum, 0, SDRIVER_SWSTREAM_ARAMADDR))
	{
		return;
	}
	if( !InitStreamBuffer( m_EntryNum, 1, SDRIVER_SWSTREAM_ARAMADDR + DISK_BUFFER_LENGTH))
	{
		return;
	}

	m_pAXVPB = AXAcquireVoice( AX_PRIORITY_NODROP, SWSoundStream::AXCallback, (u32)this);
	if( m_pAXVPB == NULL)
	{
		return;
	}
	
	m_FileLength = DVDGetLength( &m_sb[0].dvdfi);

	m_DiskReadPos = 0;

	PostDVDRead( m_sb + 0, sizeof(ADPCM_Hdr));

	AXPBADDR        addr;

   MIXInitChannel(	m_pAXVPB,
   						0,
							SoundInstance::HectodBToGCNdB( SoundVolume::UnitToHectodB( SoundInstance::GetMasterVolume( ))),
   						SoundInstance::MINVOLUME,
   						SoundInstance::MINVOLUME,
   						SoundInstance::PANCENTER,
   						SoundInstance::SPANFRONT,
   						0);

	addr.loopFlag = AXPBADDR_LOOP_ON;
	addr.format = AX_PB_FORMAT_ADPCM;

	setHiLo( addr.loopAddress, SDRIVER_SWSTREAM_ARAMADDRADPCM + 2);
	setHiLo( addr.endAddress, SDRIVER_SWSTREAM_ARAMADDRADPCM + ( AUDIO_BUFFER_LENGTH * 2) - 1);
	setHiLo( addr.currentAddress, SDRIVER_SWSTREAM_ARAMADDRADPCM + 2);

	AXSetVoiceType( m_pAXVPB, AX_PB_TYPE_STREAM);
	AXSetVoiceAddr( m_pAXVPB, &addr);
	AXSetVoiceSrcType( m_pAXVPB, AX_SRC_TYPE_LINEAR);
	AXSetVoiceState( m_pAXVPB, AX_PB_STATE_STOP);

	m_StreamLastPos = 0;
	m_SamplesPlayed = -1;

	m_bStreamQueued = true;
}

void SWSoundStream::PostDVDRead( StreamBuffer *pSB, u32 RequestedLength)
{

	SWSoundStream		*pThis = pSB->pThis;

	u32					fileBytesRemaining = pThis->m_FileLength - pThis->m_DiskReadPos;

	if( fileBytesRemaining <= 0)
	{
		memset( pSB->pBuffer, 0, DISK_BUFFER_LENGTH);
		DCFlushRange( pSB->pBuffer, DISK_BUFFER_LENGTH);
		DCInvalidateRange( pSB->pBuffer, DISK_BUFFER_LENGTH);
		pSB->bDataRead = true;
		pSB->bFileComplete = true;
		if( !pSB->bInitialized)
		{
			StartDMA( pSB);
		}
		return;
	}

	pSB->requestedSize = RequestedLength;

	pSB->actualSize = min( RequestedLength, fileBytesRemaining);

	pSB->bDataRead = false;
	pSB->bReadPending = true;
	DVDReadAsync( &pSB->dvdfi, pSB->pBuffer, OSRoundUp32B( pSB->actualSize), pThis->m_DiskReadPos, SWSoundStream::DVDCallback);

	pThis->m_DiskReadPos += pSB->actualSize;
	if( pSB->actualSize == fileBytesRemaining)
	{
		pSB->bLastBufferRead = true;
	}
}

void SWSoundStream::StartDMA( StreamBuffer *pSB)
{

	SWSoundStream		*pThis = pSB->pThis;

	if( pSB == pThis->m_sb + 0)
	{

		AXPBADPCMLOOP	loop;

		loop.loop_pred_scale = (u16)pSB->pBuffer[0];
		loop.loop_yn1           = 0;
		loop.loop_yn2           = 0;

		AXSetVoiceAdpcmLoop( pThis->m_pAXVPB, &loop);
	}

	ASSERT( pSB->bDataRead);
	if( !pSB->bDataRead)
	{
		pThis->Stop( );
		return;
	}

	ARQPostRequest(	&pSB->arq,
							(u32)pSB,
							ARQ_TYPE_MRAM_TO_ARAM,
							ARQ_PRIORITY_HIGH,
							(u32)pSB->pBuffer,
							pSB->aramAddr,
							DISK_BUFFER_LENGTH,
							SWSoundStream::ARQCallback);
}

void SWSoundStream::DVDCallback( s32 result, DVDFileInfo *pDVDFI)
{

	StreamBuffer		*pSB = (StreamBuffer *)DVDGetUserData( (DVDCommandBlock *)pDVDFI);

	SWSoundStream		*pThis = pSB->pThis;

	pSB->bReadPending = false;

	if( result < 0)		// an error occurred, stop me
	{
		if( result == DVD_RESULT_FATAL_ERROR)
		{
			pThis->m_medError = MED_ERR_FATAL;
		}
		if( pThis->m_bPlaying)
		{
			pThis->StopLeavePlaying( );
		}
		return;
	}

	if( pSB->actualSize < pSB->requestedSize)
	{
		memset( pSB->pBuffer + pSB->actualSize, 0, pSB->requestedSize - pSB->actualSize);
		DCFlushRange( pSB->pBuffer, DISK_BUFFER_LENGTH);
		DCInvalidateRange( pSB->pBuffer, DISK_BUFFER_LENGTH);
	}

	pSB->bDataRead = true;

	if( !pThis->m_bHeaderRead)
	{

		AXPBADPCM		adpcm;

		AXPBSRC			src;

		memset( &src, 0, sizeof(AXPBSRC));

		pThis->m_bHeaderRead = true;

		memcpy( &pThis->m_ADPCM_Hdr, pSB->pBuffer, sizeof(ADPCM_Hdr));

		memcpy( adpcm.a, pThis->m_ADPCM_Hdr.coef, sizeof pThis->m_ADPCM_Hdr.coef);
		adpcm.gain = pThis->m_ADPCM_Hdr.gain;
		adpcm.pred_scale = pThis->m_ADPCM_Hdr.ps;
		adpcm.yn1 = pThis->m_ADPCM_Hdr.yn1;
		adpcm.yn2 = pThis->m_ADPCM_Hdr.yn2;

		SoundDriverGCN::CompSRC( &src, pThis->m_ADPCM_Hdr.sample_rate);

		AXSetVoiceAdpcm( pThis->m_pAXVPB, &adpcm);
		AXSetVoiceSrc( pThis->m_pAXVPB, &src);

		PostDVDRead( pThis->m_sb + 0, DISK_BUFFER_LENGTH);
		PostDVDRead( pThis->m_sb + 1, DISK_BUFFER_LENGTH);

		return;
	}
	else
	if( !pSB->bInitialized)
	{
		StartDMA( pSB);
	}
	if( pThis->m_BuffersNeeded > 0)
	{
		pThis->m_BuffersNeeded--;
		if( pThis->m_BuffersNeeded == 0)
		{
			if( pThis->m_bPlaying)
			{
				pThis->Play( );
			}
		}
	}
}

void SWSoundStream::AXCallback( void *pVoid)
{

	AXVPB				*pAXVPB;

	SWSoundStream	*pThis;

	pAXVPB = (AXVPB *)pVoid;

	pThis = (SWSoundStream *)pAXVPB->userContext;

	pThis->m_bPlaying = false;

	pThis->m_pAXVPB = NULL;
}

void SWSoundStream::ARQCallback( u32 pointerToARQRequest)
{

	ARQRequest			*pARQ = (ARQRequest *)pointerToARQRequest;

	StreamBuffer		*pSB = (StreamBuffer *)pARQ->owner;

	SWSoundStream		*pThis = pSB->pThis;

	if( !pSB->bInitialized)
	{
		pSB->bInitialized = true;
	}

	if( !pThis->m_bInitialized)
	{
		pThis->m_bInitialized = ( pThis->m_sb[0].bInitialized && pThis->m_sb[1].bInitialized);
		if( pThis->m_bInitialized)
		{
			if( pThis->m_bPlayQueued)
			{
				pThis->Play( );
			}
		}
	}

	if( pSB->bLastBufferRead)
	{
		pSB->bLastBufferDMA = true;
	}

	PostDVDRead( pSB, DISK_BUFFER_LENGTH);
}


/* ------------------------------------------------------------------------ */
/* Make a GCN file name */
/* ------------------------------------------------------------------------ */
static void _MakeGCNFileName(char *baseFileName, char *realFileName)
{

	char		*pSrc,
				*pDest;

	pSrc = baseFileName;
	pDest = realFileName;

	while( *pSrc)
	{
		if( *pSrc == '\\')
		{
			*pDest = '/';
		}
		else
		{
			*pDest = *pSrc;
		}
		pSrc++;
		pDest++;
	}
	*pDest = '\0';
}


