/***************************************************************************/
// HWSoundStream, uses GC hardware audio streaming
/***************************************************************************/

/******************* includes ****************************/

#include "Audio/AudioPCH.h"

DVDFileInfo				HWSoundStream::m_dvdfi;
DVDCommandBlock		HWSoundStream::m_dvdcb;

static void _MakeGCNFileName( const char *baseFileName, char *realFileName);

/***************************************************************************/
// The driver must create the sound, but it can be deleted directly
/***************************************************************************/
HWSoundStream::~HWSoundStream( void)
{
	AISetStreamVolLeft( 0);
	AISetStreamVolRight( 0);
	AISetStreamPlayState( AI_STREAM_STOP);

	if( m_bStatusRequestQueued)
	{
		DVDCancel( &m_dvdcb);
	}

	if( m_bPrepared)
	{
		DVDCancel( (DVDCommandBlock *)&m_dvdfi);
		DVDCancelStreamAsync( (DVDCommandBlock *)&m_dvdfi, NULL);
	}

	if( m_bOpened)
	{
		DVDClose( &m_dvdfi);
	}
}

/***************************************************************************/
/***************************************************************************/
bool HWSoundStream::Play( void)
{
	m_bPlaying = true;
	if( !m_bPrepared)
	{
		return true;
	}

	AIResetStreamSampleCount( );
	AISetStreamVolLeft( m_volume);
	AISetStreamVolRight( m_volume);
	AISetStreamPlayState( AI_STREAM_START);
	return true;
}

/***************************************************************************/
// Return the play position, returns -1 if still preloading
/***************************************************************************/
int HWSoundStream::CurrentPlayPos( void)
{
	// Todd write this!!!
	return( AIGetStreamSampleCount( ));
}

/***************************************************************************/
/***************************************************************************/
bool HWSoundStream::Stop( bool rewind)
{
	AISetStreamVolLeft( 0);
	AISetStreamVolRight( 0);
	AISetStreamPlayState( AI_STREAM_STOP);

	return true;
}

/***************************************************************************/
/***************************************************************************/
bool HWSoundStream::IsPlaying( void)
{

	t_Error	error = MED_ERR_NONE;

	s32		dvdState;

	dvdState = DVDGetDriveStatus( );

	error = MediaHAL::TranslateError( dvdState, MED_ERR_FATAL);

	if( error != MED_ERR_NONE)
	{
//		OSReport( "dvdState == %d\n", dvdState);
		m_bPrepared = false;
	}

	if( m_PrepareResult != DVD_RESULT_GOOD)
	{
//		OSReport( "m_PrepareResult == %d\n", m_PrepareResult);
		error = MED_ERR_FATAL;
		m_bPrepared = false;
	}

	if( m_bStatusRequestValid)
	{
		if( m_StatusRequest == DVD_STATE_FATAL_ERROR)
		{
//			OSReport( "m_StatusRequest == %d\n", m_StatusRequest);
			error = MED_ERR_FATAL;
			m_bPrepared = false;
		}
		else
		if( m_bPrepared)
		{
			switch( m_StatusRequest)
			{
				case 0:
				case 1:
					m_bPlaying = ( m_StatusRequest == 1);
					if( !m_bPlaying)
					{
//						OSReport( "Marking end of stream\n");
					}
					break;

				default:
					break;
			}
		}
	}
	m_bStatusRequestValid = false;

	m_medError = error;

	if( m_bPlaying && m_bPrepared && !m_bStatusRequestQueued && m_medError == MED_ERR_NONE)
	{
		m_bStatusRequestQueued = true;
		m_bStatusRequestValid = false;
//		OSReport( "Queueing Status Request\n");
		DVDGetStreamErrorStatusAsync( &m_dvdcb, HWSoundStream::StatusCallback);
	}

	if( !m_bPrepared && !m_bPrepareQueued && !m_bStatusRequestQueued && m_medError == MED_ERR_NONE)
	{
		m_PrepareResult = DVD_STATE_END;
		m_bPrepareQueued = true;
//		OSReport( "Queueing Prepare Stream\n");
		DVDPrepareStreamAsync( &m_dvdfi, 0, 0, HWSoundStream::PrepareCallback);
	}

	return m_bPlaying;
}

/***************************************************************************/
// Rewind the sound
/***************************************************************************/
bool HWSoundStream::Rewind( void)
{
	return true;
}

/***************************************************************************/
// Set the volume [0..1], 0.0 is silent 1.0 is full volume
/***************************************************************************/
bool HWSoundStream::SetVolume( float volume)
{
	volume *= 0xff;

	m_volume = (u8)volume;

	AISetStreamVolLeft( m_volume);
	AISetStreamVolRight( m_volume);
	return true;
}

/***************************************************************************/
// our constructor
/***************************************************************************/
HWSoundStream::HWSoundStream( const char *streamName, bool wantRepeat) :
SoundStream( streamName)
{

	char		acNewName[256];

	_MakeGCNFileName( streamName, acNewName);

	m_volume = 0x80;

	m_bPlaying = false;
	m_bRepeat = wantRepeat;
	m_bPrepared = false;
	m_bOpened = false;
	m_medError = MED_ERR_NONE;
	m_bStatusRequestQueued = false;
	m_bStatusRequestValid = false;
	m_PrepareResult = DVD_RESULT_GOOD;
	m_StatusRequest = DVD_RESULT_GOOD;

#ifdef DVDETH
	m_bStreamQueued = false;
	return;
#endif

	m_EntryNum = DVDConvertPathToEntrynum( acNewName);
	if( m_EntryNum == -1)
	{
		m_bStreamQueued = false;
		return;
	}

	if( !DVDFastOpen( m_EntryNum, &m_dvdfi))
	{
		m_bStreamQueued = false;
		return;
	}

	m_bOpened = true;

	DVDSetUserData( (DVDCommandBlock *)&m_dvdfi, this);
	DVDSetUserData( &m_dvdcb, this);

	m_bPrepareQueued = true;
//	OSReport( "Preparing stream %s\n", acNewName);

	if( !DVDPrepareStreamAsync( &m_dvdfi, 0, 0, HWSoundStream::PrepareCallback))
	{
		m_bStreamQueued = false;
		return;
	}

	m_bStreamQueued = true;
}

void HWSoundStream::PrepareCallback( s32 result, DVDFileInfo *pdvdfi)
{

	HWSoundStream	*pThis = (HWSoundStream *)DVDGetUserData( (DVDCommandBlock *)pdvdfi);

	pThis->m_PrepareResult = result;
	pThis->m_bPrepareQueued = false;

	if( pThis->m_PrepareResult == DVD_RESULT_GOOD)
	{
	  	pThis->m_bPrepared = true;
		pThis->Play( );
		if( !pThis->m_bRepeat)
		{
			DVDStopStreamAtEndAsync( (DVDCommandBlock *)pdvdfi, NULL);
		}
	}
}

void HWSoundStream::StatusCallback( s32 result, DVDCommandBlock *pblock)
{

	HWSoundStream	*pThis = (HWSoundStream *)DVDGetUserData( pblock);

	pThis->m_bStatusRequestQueued = false;
	if( result != DVD_RESULT_CANCELED)
	{
		pThis->m_bStatusRequestValid = true;
	}

	pThis->m_StatusRequest = result;
}
/* ------------------------------------------------------------------------ */
/* Make a GCN file name */
/* ------------------------------------------------------------------------ */
static void _MakeGCNFileName( const char *baseFileName, char *realFileName)
{

	const char		*pSrc;

	char				*pDest;

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


