//
// File			-> replayMgr.cpp
// Created		-> June 30, 2003
// Author		-> Dwain Skinner
// Description	-> Instant replay manager
//

#include "Game/GamePCH.h"
#include "platform/systemi.h"
#include "Display/Common/RenderMisc.h"
#include "hud/sprite.h"
#include "GameHelper/compression/compress2.h"
CReplayMgr g_replayMgr;

//static u8 compBuf[ 0x100000 ];
// ---------------------------------------------------------------------------
CReplayMgr::CReplayMgr( ) :
	m_replayBufferSize( 0 ),
	m_pReplayBuffer( NULL ),
	m_curHandle( 0 ),
	m_numHandles( 0 ),
	m_curRecordSize( 0 ),
	m_curReplaySize( 0x20000 ),
	m_frameCount( 0 ),
	m_lastFrameSize( 0 ),
	m_totalRecordedSize( 0 ),
	m_currentReplayTime( 0 ),
	m_replayMode( CReplayMgr::RECORD ),
	m_pCurRecordPosition( NULL ),
	m_pCurRecordStartPosition( NULL ),
	m_pCurPlayPosition( NULL ),
	m_pCallBackList( NULL ),
	m_pLastRecordedFrame( NULL ),
	m_pLastFrameInBuffer( NULL ),
	m_enabled( false ),
	m_wrapMode( NOT_WRAPPED ),
	m_pHighestMemoryUsed( NULL ),
	m_allocateEnable( false ),
	m_textureSet( INVALID_HANDLE )
{
	for( int i = 0; i < NUM_LEADIN_FRAMES; i++ )
	{
		m_pFirstFrames[ i ] = NULL;
	}
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
CReplayMgr::~CReplayMgr( )
{
	if( m_pReplayBuffer )
	{
		delete[ ] m_pReplayBuffer;
	}

	if( m_pCallBackList )
	{
		delete m_pCallBackList;
	}

	m_pReplayBuffer				= NULL;
	m_pCallBackList				= NULL;
	m_pCurRecordPosition			= NULL;
	m_pCurRecordStartPosition	= NULL;
	m_pCurPlayPosition			= NULL;
	m_pLastFrameInBuffer			= NULL;
	m_pLastRecordedFrame			= NULL;
	m_replayBufferSize			= 0;
	m_numHandles					= 0;
	m_frameCount					= 0;
	m_lastFrameSize				= 0;
	m_enabled						= false;
	m_allocateEnable				= false;
	for( int i = 0; i < NUM_LEADIN_FRAMES; i++ )
	{
		m_pFirstFrames[ i ] = NULL;
	}
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::Reset( )
{
	m_replayMode					= STOP;
	m_totalRecordedSize			= 0;
	m_frameCount					= 0;
	m_lastFrameSize				= 0;
	m_pCurRecordPosition			= m_pReplayBuffer;
	m_pCurRecordStartPosition	= NULL;
	m_pCurPlayPosition			= m_pReplayBuffer;
	m_pLastRecordedFrame			= NULL;
	m_pLastFrameInBuffer			= NULL;
	m_wrapMode						= NOT_WRAPPED;


	u32 i;
	for( i = 0; i < NUM_LEADIN_FRAMES; i++ )
	{
		m_pFirstFrames[ i ] = NULL;
	}

	if( m_pCallBackList )
	{
		u32 originalHandles = m_numHandles;
		for( i = 0; i < originalHandles; i++ )
		{
			if( ( ( *m_pCallBackList )[ i ].m_handle & PERMANENT_BIT ) == 0 )
			{
				m_numHandles--;
			}
		}
	}
	else
	{
		m_numHandles = 0;
	}
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::CreateCallBackList( )
{
	if( m_pCallBackList == NULL )
	{
		m_pCallBackList = new DynamicArray< callBackFunctionData >(16, 16);
		ASSERTS( m_pCallBackList, "Could not create call back list" );

		// --- controler recording support
		m_controlerHandle = CreateReplayHandle( DESTROYABLE );
		RegisterReplayFunctions( m_controlerHandle, this, RecordReplayInterface, PlayReplayInterface );
	}
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::DestroyCallBackList( )
{
	ASSERTS( m_pCallBackList != NULL, "Callback buffer not created" );
	delete m_pCallBackList;
	m_pCallBackList = NULL;
}
// ---------------------------------------------------------------------------



// ---------------------------------------------------------------------------
bool CReplayMgr::CreateReplayBuffer( u32 size )
{
	if( m_allocateEnable )
	{
		ASSERTS( m_pReplayBuffer == NULL, "Replay buffer already created" );
		m_pReplayBuffer = new u8[ size ];
		ASSERTS( m_pReplayBuffer != NULL, "Replay buffer creation failed" );

		// --- set our initial record mark
		m_pCurRecordPosition			= m_pReplayBuffer;
		m_pCurRecordStartPosition	= NULL;
		m_curReplaySize				= size;

		ReplayModeRecord( );
	}

	return( m_pReplayBuffer != NULL );
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
bool CReplayMgr::DestroyReplayBuffer( )
{
	if( m_allocateEnable )
	{
		ASSERTS( m_pReplayBuffer != NULL, "Replay buffer not created" );
		delete[ ] m_pReplayBuffer;
		m_pReplayBuffer = NULL;
	}

	Reset( );
	Enable( false );
	return( true );
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::AllocateEnable( bool val )
{
	val = m_allocateEnable ^ true;

	if( val == true )
	{
		if( m_allocateEnable == false )
		{
			m_allocateEnable = true;
			CreateReplayBuffer( m_curReplaySize );
		}
	}
	else
	{
		if( m_allocateEnable == true )
		{
			ASSERTS( m_pReplayBuffer != NULL, "Replay buffer not created" );
			delete[ ] m_pReplayBuffer;
			m_pReplayBuffer = NULL;
			ReplayModeReset( );
		}
	}
	m_allocateEnable = val;
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::SetReplaySize( u32 size )
{
	if( size != m_curReplaySize )
	{
		if( m_allocateEnable == true )
		{
			ASSERTS( m_pReplayBuffer != NULL, "Replay buffer not created" );
			delete[ ] m_pReplayBuffer;
			m_pReplayBuffer = NULL;
			CreateReplayBuffer( size );
			ReplayModeReset( );
			ReplayModeRecord( );
		}
	}
	m_curReplaySize = size;
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
u32 CReplayMgr::CreateReplayHandle( replayFunctionType type )
{
	if( m_pCallBackList == NULL )
	{
		CreateCallBackList( );
	}
	m_numHandles++;
	if( type == PERMANENT ) return( PERMANENT_BIT | m_numHandles );
	else return( m_numHandles );
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
bool CReplayMgr::RegisterReplayFunctions( u32 handle, void *object, void ( *pRecordFunc )( void *object ), void ( *pPlayFunc )( void *object ) ) 
{
	CReplayHandle localHandle = ( ~PERMANENT_BIT ) & handle;

	ASSERTS( localHandle <= m_numHandles, "Invalid Replay handle" );
	ASSERTS( object, "Invalid replay object" );
	ASSERTS( pRecordFunc, "Invalid record function" );
	ASSERTS( pPlayFunc, "Invalid play function" );

	callBackFunctionData data;

	data.m_handle			= handle;
	data.m_pObject			= object;
	data.m_pRecordFunc	= pRecordFunc;
	data.m_pPlayFunc		= pPlayFunc;

	m_pCallBackList->Add( data, localHandle - 1 );

	return( true );
}
// ---------------------------------------------------------------------------


static	s32	frameIncrement = 1;
// ---------------------------------------------------------------------------
void CReplayMgr::ReplayMgrUpdate( )
{

	// --- make sure replay is on
	if( m_enabled )
	{
		m_currentReplayTime++;

		// --- RECORD or PLAY
		switch( m_replayMode )
		{
			// --- record
			case RECORD:
				{
					// --- make sure we have something to record
					if( m_numHandles )
					{
						u8 *pStartFramePosition	= m_pCurRecordPosition;

						//m_pCurPlayPosition		= m_pCurRecordPosition;
						m_curPlayFrame				= m_frameCount;
						m_pLastRecordedFrame		= m_pCurRecordPosition;
						
						// --- save location of our leadin frames
						if( m_pFirstFrames[ 0 ] == NULL )
						{
							for( int i = 1; i < NUM_LEADIN_FRAMES; i++ )
							{
								m_pFirstFrames[ i - 1 ] = m_pFirstFrames[ i ];
							}
							m_pFirstFrames[ NUM_LEADIN_FRAMES - 1 ] = m_pCurRecordPosition;
						}

						// --- retain the start of the frame data
						m_pCurReplayFrameHeader = m_pCurRecordPosition;
						m_curFrameSize				= sizeof( CReplayFrameHeader );
						m_curFrameChunks			= 0;
						m_pCurRecordPosition += sizeof( CReplayFrameHeader );

						// --- look to mark or change our starting record position for playback
						if( m_pCurRecordStartPosition == NULL )
						{
							m_pCurRecordStartPosition = m_pReplayBuffer;
						}

						u32 i;
						for( i = 0; i < m_numHandles; i++ )
						{
							// --- the size of this record
							m_curRecordSize		= sizeof( CReplayRecordHeader );
							m_curFrameSize			+= m_curRecordSize;

							// --- the handle for this record
							m_curHandle				= ( *m_pCallBackList )[ i ].m_handle;
							ASSERTS( m_curHandle, "Invalid replay handle" );
							
							// --- mark the recording positioins
							m_pCurRecordPosition = StartRecording( );

							// --- call the record function
							ASSERTS( ( *m_pCallBackList )[ i ].m_pObject, "Record Object not registered" );
							ASSERTS( ( *m_pCallBackList )[ i ].m_pRecordFunc, "Record function not registered" );
							void *object = ( *m_pCallBackList )[ i ].m_pObject;
							( ( *m_pCallBackList )[ i ].m_pRecordFunc )( object );

							// --- cleanup the record
							m_pCurRecordPosition = StopRecording( );
						}
						
//						u32 size = COMPRESS_CompressBuffer( m_pLastRecordedFrame, m_curFrameSize, compBuf, m_curFrameSize );
//						g_console.Notify( "uncompressed size = %ld, compressed size = %ld\n", m_curFrameSize, size );

						// --- set the frame data
						m_replayFrameHeader.SetFrameMark( );
						m_replayFrameHeader.SetFrameHeaderID( m_frameCount );
						m_replayFrameHeader.SetFrameHeaderSize( m_curFrameSize  );
						m_replayFrameHeader.SetFrameHeaderPrevSize( m_lastFrameSize  );
						m_replayFrameHeader.SetFrameHeaderChunks( m_curFrameChunks );
						m_totalRecordedSize += m_curFrameSize;
						m_lastFrameSize = m_curFrameSize;
						m_frameCount++;
						ASSERTS( m_pCurRecordPosition < ( m_pReplayBuffer + m_curReplaySize ), "Ran past replay buffer" );

						// --- wrap record buffer
						if( ( m_pCurRecordPosition + ( 2 * m_lastFrameSize ) ) > ( m_pReplayBuffer + m_curReplaySize ) )
						{
							// --- mark last frame of the buffer
							m_replayFrameHeader.SetFrameHeaderID( LAST_FRAME_ID );
							m_pCurRecordPosition = m_pReplayBuffer;
							m_pLastFrameInBuffer = m_pLastRecordedFrame;
							m_wrapMode = WRAPPED;
						}

						// --- have we wrapped around our buffer?
						if( m_wrapMode == WRAPPED )
						{
							// --- we just jumped over and back around
							if( m_pCurRecordPosition > m_pCurRecordStartPosition )
							{
								for( i = 0; i < NUM_LEADIN_FRAMES; i++ )
								{
									if( m_pFirstFrames[ i ] < m_pCurRecordPosition )
									{
										ASSERTS( i < ( NUM_LEADIN_FRAMES - 1 ), "Uh Oh something went wrong" );
										
										// --- make sure address is very small but not zero
										m_pSaveFirstFrames[ i ] = m_pFirstFrames[ i ];
										m_pFirstFrames[ i ] = ( u8 * )0x00000001;
										if( m_pSaveFirstFrames[ i ] > m_pFirstFrames[ i + 1 ] ) break;
									}
									else
									{
										break;
									}
								}
							}

							m_pCurRecordStartPosition = GetNextFrameMark( pStartFramePosition );
						}
						m_replayFrameHeader.SaveFrameHeaderData( m_pCurReplayFrameHeader );
					}
				}
				break;


			case REVERSE:
			case PLAY:
			case SLOWFWD:
			case SLOWREV:
			case STEPFWD:
			case STEPREV:
				{
					if( m_currentReplayTime >= m_currentReplayTimeTarget )
					{
						u32	localDirection = PLAY;

						m_currentReplayTime = 0;
						if( m_replayMode == REVERSE || m_replayMode == SLOWREV || m_replayMode == STEPREV )
						{
							localDirection = REVERSE;
							frameIncrement = -1;
						}
						if( m_numHandles && m_pLastRecordedFrame )
						{
							if(	( ( localDirection == PLAY ) && ( m_pCurPlayPosition != m_pLastRecordedFrame ) ) ||
									( ( localDirection == REVERSE ) && ( m_pCurPlayPosition != m_pCurRecordStartPosition ) ) )
							{
								// --- get the frame header
								m_replayFrameHeader.GetFrameHeaderData( m_pCurPlayPosition );
								if( localDirection == REVERSE )
								{
									m_pCurPlayPosition -= m_replayFrameHeader.GetFrameHeaderPrevSize( );
									if( m_pCurPlayPosition < m_pReplayBuffer )
									{
										m_pCurPlayPosition = m_pLastFrameInBuffer;
										ASSERTS( m_pCurPlayPosition, "m_pLastFrameInBuffer did not get set" );
									}
									m_replayFrameHeader.GetFrameHeaderData( m_pCurPlayPosition );
								}
								m_pCurPlayPosition += sizeof( CReplayFrameHeader );
					
								for( u32 j = 0; j < m_replayFrameHeader.GetFrameHeaderChunks( ); j++ )
								{
									m_curHandle				= ( *m_pCallBackList )[ j ].m_handle;
									ASSERTS( m_curHandle, "Invalid replay handle" );
					
									m_pCurPlayPosition = StartPlaying( );
		
									// --- call the play function
									ASSERTS( ( *m_pCallBackList )[ j ].m_pObject, "Record Object not registered" );
									ASSERTS( ( *m_pCallBackList )[ j ].m_pPlayFunc, "Play function not registered" );
									void *object = ( *m_pCallBackList )[ j ].m_pObject;
									( ( *m_pCallBackList )[ j ].m_pPlayFunc )( object );

									m_pCurPlayPosition = StopPlaying( );
								}
								m_curPlayFrame += frameIncrement;
								if( localDirection == REVERSE )
								{
									m_pCurPlayPosition -= m_replayFrameHeader.GetFrameHeaderSize( );
									if( m_pCurPlayPosition < m_pReplayBuffer )
									{
										m_pCurPlayPosition = m_pLastFrameInBuffer;
										ASSERTS( m_pCurPlayPosition, "m_pLastFrameInBuffer did not get set" );
									}
								}
								else
								{
									// --- how last frame of the buffer is marked
									if( m_replayFrameHeader.GetFrameHeaderID( ) == LAST_FRAME_ID )
									{
										m_pCurPlayPosition = m_pReplayBuffer;
									}
								}

								// --- single step?
								if( m_replayMode == STEPFWD || m_replayMode == STEPREV )
								{
									
									ReplayModePause( );
								}
							}
							else
							{
								PlayPausedFrame( );
								ReplayModePause( );
							}
						}
					}
					else
					{
						PlayPausedFrame( );
					}
				}
				break;

			// --- Pause recording or playing
			case PAUSE:
				PlayPausedFrame( );
				break;

			// --- Stop recording or playing
			case STOP:
				break;
				
		}
	}
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
u8 *CReplayMgr::GetNextFrameMark( u8 *pFrameStart )
{
	// --- see if we have overwritten our tape record start position
	if(	( ( pFrameStart <= m_pCurRecordStartPosition ) && ( m_pCurRecordPosition > m_pCurRecordStartPosition ) ) || 
			( ( pFrameStart >= m_pCurRecordStartPosition ) && ( m_pCurRecordPosition < m_pCurRecordStartPosition ) )  )
	{
		bool found = false;

		ASSERTS( m_pFirstFrames[ 0 ], "Did not record enough leadin frames before we wrapped" );
		for( int i = 0; i < NUM_LEADIN_FRAMES; i++ )
		{
			ASSERTS( i < NUM_LEADIN_FRAMES - 1, "Ran out of leadin frames to test against" );

			bool marked = false;
			if( i > 0 )
			{
				marked = ( ( ( u32 )m_pFirstFrames[ i - 1 ] == 1 ) && ( ( u32 )m_pFirstFrames[ i ] > 1 ) );
			}
			// --- got our new frame for the start of playback
			if( ( m_pFirstFrames[ i ] >= m_pCurRecordPosition ) || ( m_pFirstFrames[ i + 1 ] < m_pFirstFrames[ i ] ) || marked )
			{
				CReplayFrameHeader frameHeader;
				u8	*pLastLeadinFrame = NULL;

				frameHeader.GetFrameHeaderData( m_pFirstFrames[ i ] );
				ASSERTS( frameHeader.IsFrameMark( ( const u8 * )FRAMEMARK ), "Not a valid frame" );
				found = true;
				m_pCurRecordStartPosition = m_pFirstFrames[ i ];

				// --- adjust leadin
				if( i )
				{
					int j;
					for( j = 0; j + i < NUM_LEADIN_FRAMES; j++ )
					{
						m_pFirstFrames[ j ] = pLastLeadinFrame = m_pFirstFrames[ j + i ];
					}

					for( ; j < NUM_LEADIN_FRAMES; j++ )
					{
						frameHeader.GetFrameHeaderData( pLastLeadinFrame );
						ASSERTS( frameHeader.IsFrameMark( ( const u8 * )FRAMEMARK ), "Not a valid frame" );
						pLastLeadinFrame += frameHeader.GetFrameHeaderSize( );
						if( ( pLastLeadinFrame > ( m_pReplayBuffer + m_curReplaySize ) ) || ( pLastLeadinFrame >= m_pLastFrameInBuffer ) ) 
						{
							pLastLeadinFrame = m_pReplayBuffer;
						}
						frameHeader.GetFrameHeaderData( pLastLeadinFrame );
						ASSERTS( frameHeader.IsFrameMark( ( const u8 * )FRAMEMARK ), "Not a valid frame" );
						m_pFirstFrames[ j ] = pLastLeadinFrame;
					}
				}
				break;
			}
		}
		ASSERTS( found, "Did not find a new playback starting frame" );
		m_pCurPlayPosition = m_pFirstFrames[ 0 ];
	}

	return( m_pCurRecordStartPosition );
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::ReplayModeRecord( )
{
	if( m_allocateEnable )
	{
		if( m_pReplayBuffer == NULL )
		{
			CreateReplayBuffer( m_curReplaySize );
		}
		ReplayModeReset( );
		m_replayMode					= RECORD;
		m_enabled						= true;
		m_totalRecordedSize			= 0;
		m_frameCount					= 0;
		m_lastFrameSize				= 0;
		m_pCurRecordStartPosition	= NULL;
		m_pCurPlayPosition			= m_pCurRecordPosition;
	}
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::ReplayModePlay( )
{
	if( m_pCurPlayPosition == m_pLastRecordedFrame )
	{
		m_pCurPlayPosition = m_pCurRecordStartPosition;
	}
	m_enabled						= true;
	m_replayMode					= PLAY;
	m_currentReplayTimeTarget	= 1;
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::ReplayModePause( )
{
	m_enabled						= true;
	m_replayMode					= PAUSE;
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::ReplayModeReverse( )
{
	if( m_pCurPlayPosition == m_pCurRecordStartPosition )
	{
		m_pCurPlayPosition = m_pLastRecordedFrame;
	}

	m_enabled						= true;
	m_replayMode					= REVERSE;
	m_currentReplayTimeTarget	= 1;
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::ReplayModeSlowFwd( )
{
	if( m_pCurPlayPosition == m_pLastRecordedFrame )
	{
		m_pCurPlayPosition = m_pCurRecordStartPosition;
	}
	m_enabled						= true;
	m_replayMode					= SLOWFWD;
	m_currentReplayTimeTarget	= 5;
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::ReplayModeSlowRev( )
{
	if( m_pCurPlayPosition == m_pCurRecordStartPosition )
	{
		m_pCurPlayPosition = m_pLastRecordedFrame;
	}
	m_enabled						= true;
	m_replayMode					= SLOWREV;
	m_currentReplayTimeTarget	= 5;
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::ReplayModeStepFwd( )
{
	if( m_pCurPlayPosition == m_pLastRecordedFrame )
	{
		m_pCurPlayPosition = m_pCurRecordStartPosition;
	}
	m_enabled						= true;
	m_replayMode					= STEPFWD;
	m_currentReplayTimeTarget	= 1;
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::ReplayModeStepRev( )
{
	if( m_pCurPlayPosition == m_pCurRecordStartPosition )
	{
		m_pCurPlayPosition = m_pLastRecordedFrame;
	}
	m_enabled						= true;
	m_replayMode					= STEPREV;
	m_currentReplayTimeTarget	= 1;
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::ReplayModeStop( )
{
	m_enabled						= false;
	m_replayMode					= STOP;
	ClearSprites( );	
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::ReplayModeReset( )
{
	u32		saveHandles			= m_numHandles;
	
	Reset( );
	m_numHandles					= saveHandles;
	m_enabled						= false;
	m_replayMode					= RESET;
	ClearSprites( );
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
u8 *CReplayMgr::StartRecording( )
{
	m_pCurReplayRecordHeader	= m_pCurRecordPosition;
	m_pCurRecordPosition			+= sizeof( CReplayRecordHeader );
	return( m_pCurRecordPosition );
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
u8 *CReplayMgr::StopRecording( )
{
	m_replayRecordHeader.SetRecordHeaderHandle( m_curHandle );
	m_replayRecordHeader.SetRecordChunkSize( m_curRecordSize );
	m_replayRecordHeader.SaveRecordHeaderData( m_pCurReplayRecordHeader );
	m_curFrameChunks++;
	return( m_pCurRecordPosition );
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
u8 *CReplayMgr::StartPlaying( )
{
	m_pCurReplayPlayHeader	= m_pCurPlayPosition;
	m_pCurPlayPosition		+= sizeof( CReplayRecordHeader );
	m_replayPlayHeader.GetRecordHeaderData( m_pCurReplayPlayHeader );
	ASSERTS( m_curHandle == m_replayPlayHeader.GetRecordHeaderHandle( ), "Record handle mismatch" );
	return( m_pCurPlayPosition );
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
u8 *CReplayMgr::StopPlaying( )
{
	return( m_pCurPlayPosition );
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::RecordReplayData( const u8 *pData, u32 size )
{
	if( m_enabled && m_allocateEnable )
	{
		m_curFrameSize		+= size;
		m_curRecordSize	+= size;
		memcpy( m_pCurRecordPosition, pData, size );
		m_pCurRecordPosition += size;
	}
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::PlayReplayData( u8 *pDstData, u32 size )
{
	if( m_enabled && m_allocateEnable )
	{
		memcpy( pDstData, m_pCurPlayPosition, size );
		m_pCurPlayPosition += size;
	}
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::PlayPausedFrame( )
{
	u8 *pSaveCurPlayPosition = m_pCurPlayPosition;

	if( m_numHandles && m_pLastRecordedFrame )
	{
		// --- get the frame header
		m_replayFrameHeader.GetFrameHeaderData( m_pCurPlayPosition );
		m_pCurPlayPosition += sizeof( CReplayFrameHeader );
			
		for( u32 j = 0; j < m_replayFrameHeader.GetFrameHeaderChunks( ); j++ )
		{
			m_curHandle				= ( m_pCallBackList->GetPtr( ) )[ j ].m_handle;
			ASSERTS( m_curHandle, "Invalid replay handle" );
			m_pCurPlayPosition = StartPlaying( );
		
			// --- call the play function
			ASSERTS( m_pCallBackList->GetPtr( )[ j ].m_pObject, "Record Object not registered" );
			ASSERTS( m_pCallBackList->GetPtr( )[ j ].m_pPlayFunc, "Play function not registered" );
			void *object = m_pCallBackList->GetPtr( )[ j ].m_pObject;
			( m_pCallBackList->GetPtr( )[ j ].m_pPlayFunc )( object );

			m_pCurPlayPosition = StopPlaying( );
		}
	}

	m_pCurPlayPosition = pSaveCurPlayPosition;
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::RecordReplayInterface( void *object )
{
	( ( CReplayMgr * )object )->RecordControllerData( );
}

void CReplayMgr::PlayReplayInterface( void *object )
{
	( ( CReplayMgr * )object )->PlayControllerData( );
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::RecordControllerData( )
{
	ControllerInputDriver* controller = g_input.GetController( 0 );

	float		rStickX[ 2 ];
	float 	rStickY[ 2 ];
	u32		buttons = 0;

	// --- left analog
	rStickX[ 0 ] = controller->StickPosition(AA_X1);
	rStickY[ 0 ] = controller->StickPosition(AA_Y1);

	// --- right analog
	rStickX[ 1 ] = controller->StickPosition(AA_X2);
	rStickY[ 1 ] = controller->StickPosition(AA_Y2);

	buttons |= ( controller->ButtonDown( CB_STICK1 )	? 1 : 0 );
	buttons |= ( controller->ButtonDown( CB_STICK2 )	? 2 : 0 );
	buttons |= ( controller->ButtonDown( CB_TRI )		? 4 : 0 );
	buttons |= ( controller->ButtonDown( CB_O )			? 8 : 0 );
	buttons |= ( controller->ButtonDown( CB_X )			? 16 : 0 );
	buttons |= ( controller->ButtonDown( CB_SQUARE )	? 32 : 0 );

	RecordReplayData( ( u8 * )rStickX, sizeof( rStickX ) );
	RecordReplayData( ( u8 * )rStickY, sizeof( rStickY ) );
	RecordReplayData( ( u8 * )&buttons, sizeof( buttons ) );
}
// ---------------------------------------------------------------------------



// ---------------------------------------------------------------------------
struct spriteCoords
{
	float centerX;
	float centerY;
	float radius;
	float width;
	float height;
	u32	textureIndex;
	u32	useStick;
	u16	priority;
};

static spriteCoords s[ NUM_DEBUG_SPRITES ] =
{
	{ 350.0f, 400.0f, 28.0f, 10.0f, 10.0f, 0, 1, 1 },
	{ 413.0f, 400.0f, 28.0f, 10.0f, 10.0f, 0, 2, 1 },
	
	{ 470.0f, 380.0f, 1.0f, 16.0f, 16.0f, 0, 0, 2 },
	{ 485.0f, 390.0f, 1.0f, 16.0f, 16.0f, 0, 0, 2 },
	{ 470.0f, 400.0f, 1.0f, 16.0f, 16.0f, 0, 0, 2 },
	{ 455.0f, 390.0f, 1.0f, 16.0f, 16.0f, 0, 0, 2 },

	{ 350.0f, 400.0f, 28.0f, 6.0f, 6.0f, 0, 0, 3 },
	{ 413.0f, 400.0f, 28.0f, 6.0f, 6.0f, 0, 0, 3 },
	
	{ 350.0f, 400.0f, 1.0f, 4.0f, 4.0f, 2, 0, 4 },
	{ 413.0f, 400.0f, 1.0f, 4.0f, 4.0f, 2, 0, 4 },

	{ 350.0f, 400.0f, 1.0f, 64.0f, 64.0f, 3, 0, 0 },
	{ 413.0f, 400.0f, 1.0f, 64.0f, 64.0f, 3, 0, 0 },
	};

Vector3	v[4];
Vector2	uv[4];

void CReplayMgr::PlayControllerData( )
{
	float		rStickX[ 2 ];
	float		rStickY[ 2 ];
	
	u32		buttons;
	int		i;
	u32		bit;
	
	PlayReplayData( ( u8 * )rStickX, sizeof( rStickX ) );
	PlayReplayData( ( u8 * )rStickY, sizeof( rStickY ) );
	PlayReplayData( ( u8 * )&buttons, sizeof( buttons ) );


	// --- draw the controller inputs
	if( m_textureSet == INVALID_HANDLE )
	{
		m_textureSet = dblLoad("fluff\\replay.dbl" );
		ASSERT( m_textureSet != INVALID_HANDLE );

		SpriteMgr__SetTextureDbl( m_textureSet );

		uv[0].Set( 0.0f, 0.0f );
		uv[1].Set( 1.0f, 0.0f );
		uv[2].Set( 1.0f, 1.0f );
		uv[3].Set( 0.0f, 1.0f );
		for( i = 0; i < NUM_DEBUG_SPRITES; i++ )
		{
			m_spriteHandles[ i ] = SpriteMgr__RegisterSprite( s[ i ].textureIndex, 0x8000, 0xf000+s[ i ].priority , SHADE_SPRITE | WHITE_SPRITE, v, uv, NULL );
		}
	}

	// --- update the controller display interface
	for( bit = 1, i = 0; i < NUM_DEBUG_SPRITES; i++ )
	{	
		float l, r, t, b;
		float sX, sY;

		// --- is this a stick sprite?
		if( s[ i ].useStick )
		{
			sX = rStickX[ s[ i ].useStick - 1 ];
			sY = rStickY[ s[ i ].useStick - 1 ];
		}
		else
		{
			sX = 0.0f;
			sY = 0.0f;
		}
		
		// --- need to transform the vertical bars to point in the direction of the stick
		if( i == 8 || i == 9 )
		{
			Graphics4x4 matrix;
			float angle;
			float magnitude;
			if( i == 8 )
			{
				magnitude	= -Math::Sqrt( rStickX[ 0 ] * rStickX[ 0 ] + rStickY[ 0 ] * rStickY[ 0 ] );
				angle			= Math::ArcTan2( rStickY[ 0 ], rStickX[ 0 ] );
			}
			else
			{
				magnitude	= -Math::Sqrt( rStickX[ 1 ] * rStickX[ 1 ] + rStickY[ 1 ] * rStickY[ 1 ] );
				angle			= Math::ArcTan2( rStickY[ 1 ], rStickX[ 1 ] );
			}

			// --- normal unmoved orientation points up 90 degrees, so subtract 90 degrees
			angle -= Math::Deg2Rad( 90.0f );
			matrix.Identity( );
			matrix.CatXTranslation( -( s[ i ].centerX ) );
			matrix.CatYTranslation( -( s[ i ].centerY ) );
			matrix.CatZRotation( -angle );
			matrix.CatXTranslation( s[ i ].centerX );
			matrix.CatYTranslation( s[ i ].centerY );
			l = ( s[ i ].centerX - ( s[ i ].width / 2 ) );
			r = ( s[ i ].centerX + ( s[ i ].width / 2 ) );
			t = ( s[ i ].centerY + ( s[ i - 8 ].radius * magnitude ) - ( s[ i ].height / 2 ) );
			v[ 0 ].Set( matrix.Transform( l, t, 0.0f ) );
			v[ 1 ].Set( matrix.Transform( r, t, 0.0f ) );
			l = ( s[ i ].centerX + ( s[ i ].radius * sX ) - ( s[ i ].width / 2 ) );
			r = ( s[ i ].centerX + ( s[ i ].radius * sX ) + ( s[ i ].width / 2 ) );
			b = ( s[ i ].centerY + ( s[ i ].radius * -sY )+ ( s[ i ].height / 2 ) );
			v[ 2 ].Set( matrix.Transform( r, b, 0.0f ) );
			v[ 3 ].Set( matrix.Transform( l, b, 0.0f ) );
			v[ 0 ].X( v[ 0 ].X( ) / 512.0f ); v[ 0 ].Y( v[ 0 ].Y( ) / 448.0f );
			v[ 1 ].X( v[ 1 ].X( ) / 512.0f ); v[ 1 ].Y( v[ 1 ].Y( ) / 448.0f );
			v[ 2 ].X( v[ 2 ].X( ) / 512.0f ); v[ 2 ].Y( v[ 2 ].Y( ) / 448.0f );
			v[ 3 ].X( v[ 3 ].X( ) / 512.0f ); v[ 3 ].Y( v[ 3 ].Y( ) / 448.0f );
		}
		else
		{
			l = ( s[ i ].centerX + ( s[ i ].radius * sX ) - ( s[ i ].width / 2 ) ) / 512.0f;
			r = ( s[ i ].centerX + ( s[ i ].radius * sX ) + ( s[ i ].width / 2 ) ) / 512.0f;
			t = ( s[ i ].centerY + ( s[ i ].radius * -sY )- ( s[ i ].height / 2 ) ) / 448.0f;
			b = ( s[ i ].centerY + ( s[ i ].radius * -sY )+ ( s[ i ].height / 2 ) ) / 448.0f;
			v[0].Set( l, t, 0 );  v[1].Set( r, t, 0 );  v[2].Set( r, b, 0 );  v[3].Set( l, b, 0 );
		}

		// --- make sure we're using the right texture set
		SpriteMgr__SetTextureDbl( m_textureSet );

		// --- sprites that draw differently if a button is pressed
		if( i < 8 )
		{
			if( buttons & bit )
				SpriteMgr__SetTextureByIndex( m_spriteHandles[ i ], 1 );
			else
				SpriteMgr__SetTextureByIndex( m_spriteHandles[ i ], 0 );
		}
		SpriteMgr__UpdateSprite( m_spriteHandles[ i ], 0x8000, 0xf000+s[ i ].priority, SHADE_SPRITE | WHITE_SPRITE, v, uv, NULL );
		bit <<= 1;
	}
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CReplayMgr::ClearSprites( )
{
	if( m_textureSet != INVALID_HANDLE )
	{
		for( int i = 0; i < NUM_DEBUG_SPRITES; i++ )
		{
			if( m_spriteHandles[ i ] ) SpriteMgr__KillSprite( m_spriteHandles[ i ] );
		}

		m_textureSet = INVALID_HANDLE;
	}
}
// ---------------------------------------------------------------------------
