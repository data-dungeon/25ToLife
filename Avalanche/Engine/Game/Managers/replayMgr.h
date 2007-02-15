//
// File			-> replayMgr.h
// Created		-> June 30, 2003
// Author		-> Dwain Skinner
// Description	-> Instant replay manager
//

#ifndef __REPLAYMGR_H__
#define __REPLAYMGR_H__

typedef u32 CReplayHandle;
#define FRAMEMARK "FRAME69\0"
#define NUM_LEADIN_FRAMES 5
#define LAST_FRAME_ID 0xffffffff
#define NUM_DEBUG_SPRITES 12

class CReplayFrameHeader
{

public:
	CReplayFrameHeader												( )	{ };
	~CReplayFrameHeader												( )	{ };

	inline void				GetFrameHeaderData					( u8 *pSrcData )			{ memcpy( this, pSrcData, sizeof( CReplayFrameHeader ) ); }
	inline void				SaveFrameHeaderData					( u8 *pDstData )			{ memcpy( pDstData, this, sizeof( CReplayFrameHeader ) ); }
	inline void				SetFrameHeaderID						( u32 id )					{ m_frameID = id; }
	inline void				SetFrameHeaderSize					( u32 size )				{ m_frameSize = size; }
	inline void				SetFrameHeaderPrevSize				( u32 size )				{ m_prevFrameSize = size; }
	inline void				SetFrameHeaderChunks					( u32 chunks )				{ m_frameChunks = chunks; }
	inline void				SetFrameMark							( )							{ memcpy( m_frameMark, FRAMEMARK, sizeof( m_frameMark ) ); }
	inline u32				GetFrameHeaderID						( )							{ return( m_frameID ); }
	inline u32				GetFrameHeaderSize					( )							{ return( m_frameSize ); }
	inline u32				GetFrameHeaderPrevSize				( )							{ return( m_prevFrameSize ); }
	inline u32				GetFrameHeaderChunks					( )							{ return( m_frameChunks ); }
	inline u8*				GetFrameMark							( )							{ return( m_frameMark ); }
	inline bool				IsFrameMark								( const u8*pSrcData )	{ return( strncmp( ( const char * )pSrcData, ( const char * )m_frameMark, sizeof( m_frameMark ) ) == 0 ); }

private:
	u32						m_frameID;
	u32						m_frameSize;
	u32						m_prevFrameSize;
	u32						m_frameChunks;
	u8							m_frameMark[ 8 ];
};


class CReplayRecordHeader
{

public:
	CReplayRecordHeader												( )	{ };
	~CReplayRecordHeader												( )	{ };

	inline void				GetRecordHeaderData					( u8 *pSrcData )			{ memcpy( this, pSrcData, sizeof( CReplayRecordHeader ) ); }
	inline void				SaveRecordHeaderData					( u8 *pDstData )			{ memcpy( pDstData, this, sizeof( CReplayRecordHeader ) ); }
	inline void				SetRecordHeaderHandle				( u32 handle )				{ m_handle = handle; }
	inline void				SetRecordChunkSize					( u32 chunkSize )			{ m_chunkSize = chunkSize; }
	inline u32				GetRecordHeaderHandle				( )							{ return( m_handle ); }
	inline u32				GetRecordHeaderChunkSize			( )							{ return( m_chunkSize ); }

private:
	u32						m_handle;
	u32						m_chunkSize;
};


class CReplayMgr
{

public:
	enum replayFunctionType
	{
		PERMANENT,
		DESTROYABLE,
	};

	CReplayMgr															( );
	~CReplayMgr															( );

	bool						CreateReplayBuffer					( u32 size );
	bool						DestroyReplayBuffer					( );
	void						CreateCallBackList					( );
	void						DestroyCallBackList					( );
	u32						CreateReplayHandle					( replayFunctionType type );
	bool						RegisterReplayFunctions				( u32 handle, void *object, void ( *RecordFunc )( void *object ), void ( *PlayFunc )( void *object ) );
	void						ReplayMgrUpdate						( );
	void						RecordReplayData						( const u8 *data, u32 size );
	void						PlayReplayData							( u8 *data, u32 size );
	inline void				Enable									( bool value )				{ m_enabled = value; }
	inline bool				IsPlaying								( )							{ return( m_replayMode == PLAY || m_replayMode == PAUSE || m_replayMode == REVERSE || m_replayMode == SLOWFWD || m_replayMode == SLOWREV || m_replayMode == STEPFWD || m_replayMode == STEPREV ); }

	void						ReplayModeRecord						( );
	void						ReplayModePlay							( );
	void						ReplayModePause						( );
	void						ReplayModeReverse						( );
	void						ReplayModeStop							( );
	void						ReplayModeReset						( );
	void						ReplayModeSlowFwd						( );
	void						ReplayModeSlowRev						( );
	void						ReplayModeStepFwd						( );
	void						ReplayModeStepRev						( );
	void						Reset										( );
	void						AllocateEnable							( bool val );
	void						SetReplaySize							( u32 size );
	
	#define PERMANENT_BIT 0x80000000

	enum bufferWrapMode
	{
		NOT_WRAPPED = 0,
		WRAPPED,
	};

	enum replayMode
	{
		RECORD = 0,
		STOP,
		PLAY,
		REVERSE,
		SLOWFWD,
		SLOWREV,
		STEPFWD,
		STEPREV,
		PAUSE,
		RESET,
	};

	
private:
	u8							*StartRecording						( );
	u8							*StopRecording							( );
	u8							*StartPlaying							( );
	u8							*StopPlaying							( );
	u8*						GetNextFrameMark						( u8 *pStart );
	void						ClearSprites							( );
	void						PlayPausedFrame						( );
	
	void						RecordControllerData					( );
	void						PlayControllerData					( );
	static void				RecordReplayInterface				( void *object );
	static void				PlayReplayInterface					( void *object );

	u32						m_replayBufferSize;					// size of the created replay buffer
	u32						m_numHandles;							// number of registered record and replay contributors
	CReplayHandle			m_curHandle;							// current handle of record or replay contributor
	u32						m_curRecordSize;						// current record size being recorded
	u32						m_curReplaySize;						// current size of record being played
	u32						m_curFrameSize;						// total size of the current frames worth of record data
	u32						m_curFrameChunks;						// number of recorded chunks for this frame of data
	u32						m_frameCount;							// number of frames saved in the record buffer
	u32						m_totalRecordedSize;					// total size of all recorded frames
	u32						m_curPlayFrame;						// current frame we are playing
	u32						m_lastFrameSize;						// size of the previous frame
	u32						m_currentReplayTime;					// replay counter
	u32						m_currentReplayTimeTarget;			// replay counter target
	replayMode				m_replayMode;							// mode we are in
	u8							*m_pReplayBuffer;						// pointer to the replay buffer
	u8							*m_pCurRecordPosition;				// current position of the record
	u8							*m_pCurPlayPosition;					// current position of the play
	u8							*m_pCurRecordStartPosition;		// pointer to the first frame of the recorded data, will change if we wrap around the buffer while recording
	u8							*m_pLastRecordedFrame;				// pointer to the last frame we recorded
	u8							*m_pLastFrameInBuffer;				// pointer to the last usable frame in the replay buffer
	u8							*m_pHighestMemoryUsed;				// the largest memory location we recorded a frame into
	bool						m_enabled;								// recording or play enabled
	bufferWrapMode			m_wrapMode;
	u8							*m_pFirstFrames[ NUM_LEADIN_FRAMES ];
	u8							*m_pSaveFirstFrames[ NUM_LEADIN_FRAMES ];
	CReplayHandle			m_controlerHandle;
	t_Handle					m_spriteHandles[ NUM_DEBUG_SPRITES ];
	t_Handle					m_textureSet;
	bool						m_allocateEnable;

	struct callBackFunctionData
	{
		u32					m_handle;
		void					*m_pObject;
		void					( *m_pRecordFunc )( void *object );
		void					( *m_pPlayFunc )( void *object );
	};
	
	DynamicArray<callBackFunctionData>	*m_pCallBackList;
	CReplayRecordHeader	m_replayRecordHeader;
	u8							*m_pCurReplayRecordHeader;

	CReplayFrameHeader	m_replayFrameHeader;
	u8							*m_pCurReplayFrameHeader;

	CReplayRecordHeader	m_replayPlayHeader;
	u8							*m_pCurReplayPlayHeader;
};

extern CReplayMgr g_replayMgr;
#endif