/////////////////////////////////////////////////////////////////////////////
// Multistream wrapper for SPU and file managment
//
// This is PS2 specific, there is no abstraction for other platforms, this
// should only be included/called in PS2 specific code...what I'm trying to
// say is: if I see an #ifdef, I'll haunt you til your grave.
//
// Currently based off the 7.2 release of multistream
/////////////////////////////////////////////////////////////////////////////
#ifndef MSTREAM_H
#define MSTREAM_H

#include "Layers/mstream/streamirx.h"
#include "container/pool.h"
#include "Layers/mstream/spuchan.h"
#include "Layers/mstream/mibstream.h"
#include "Layers/mstream/fstream.h"

class MultiStream
{
public:
	// Boot!
	static bool Boot(bool isDVD, bool warm);

	// Initialize the file system
	static bool InitializeFileSystem(int maxOpenFiles);

	// Initialize the sound system
	static bool InitializeSoundSystem(int maxSFX, int maxMIBStreams, int maxSRAMHandles);

	// Destroy everything
	static void TerminateSoundSystem();
	static void TerminateFileSystem();

	// Call this frequently.  Threads are nice, but messy
	static void Daemon();

	// Totally pause and resume multistream (MS will not access CD)
	// This is useful for running DNAS
	static void Suspend();
	static void Resume();
	static bool Suspended();

	// Register a callback for blocking loads or long ops
	typedef void (*BlockingCallback)(void *context);
	static void RegisterBlockingCallback(BlockingCallback callback, void *context = NULL);
	static void ClearBlockingCallback();
	static void CallBlockingCallback();

	// open a file
	static PS2FileStream *OpenFile(const char *filename, bool async);
	static PS2FileStream *CreateFile(int fileID, int size, bool async);
	static void CloseFile(PS2FileStream *stream);

	// Wait for so many microseconds (this blocks)
	static void Wait(uint microseconds);

	// Talk to the IOP in a friendly way
	static bool SyncIOP(bool wait = true);

	// Use iop/ee synchronization
	static void BeginReadyCheck();
	static bool IsReady();

	// Allocate/free SRAM
	static uint AllocateSRAM(int size);
	static void FreeSRAM(uint &addr);

	// Allocate some stream SPU RAM, size MUST BE 64 byte aligned!!
	static bool TransferEEToSRAM(void *data, uint size, uint spuAddr);

	// Effects support
	static int GetEffectSize(int effect);
	static bool PrepareEffect(int effect, bool useBestFit = false);
	static bool EnableEffect(int effect, int depthL, int depthR, int delay, int feedback);
	static bool SetEffectVolume(int depthL, int depthR);
	static void ClearEffects();

	// SFXID support
	static SFXID AllocateSFXID(uint spuAddr);
	static void FreeSFXID(SFXID id);

	// Hardware channel support
	static SPUChannel *AllocateChannel(const char *name);
	static void FreeChannel(SPUChannel *channel);

	// Stream support (-1 is invalid)
	static int AllocateStreamID();
	static void FreeStreamID(int &streamID);
	static bool StopStream(int streamID);
	static bool OpenStreamBuffer(int streamID, uint spuAddr, int size);
	static bool CloseStreamBuffer(int streamID);
	static bool StreamLoadEE(int streamID, int fileID, uint eeAddr);
	static bool StreamSetEEAddress(int streamID, uint eeAddr);

	// Create a fileID for a file (return -1 if not there)
	static int OpenFileID(const char *pathFile, int *size = NULL);
	static void CloseFileID(int &fileID);
	static bool FileExists(const char *pathFile);

	// Stream manipulation!
	static MIBStream *CreateStream(const char *streamName);
	static void DestroyStream(MIBStream *stream);

	// Auto DMA and PCM support
	static void AutoDMAVolume(int channel, int volumeL, int volumeR, bool force);
	static void AutoDMAStartWrite(int channel, bool loop, int iopAddr, int size, int offset);
	static void AutoDMAStop(int channel);
	static int GetPCMPlaybackPosition();

	// Some direct control over fastload stuff
	static void OverrideFastLoadMode(int newMode);
	static void RestoreFastLoadMode();
	static bool GetFastLoadInfo(SOUND_FASTLOAD_INFO &info);
	static bool SetFastLoadNext(int streamID);
	static int SuspendFastLoad();
	static void ResumeFastLoad(int state);

	// Is ATWinMon running?
	static bool UsingATWinMon();

	// Dump stuff out
	static void DebugDump();

	// Big fat error handler
	static void BigFatError(const char *description, const char *fileName = NULL);

	// Utiltity
	static inline uint SizeBy64(uint s)
		{ return (s + (((s ^ 0x3f) + 1) & 0x3f)); }

protected:
	// Main constants
	static int	m_maxOpenFiles;
	static int	m_maxSFX;
	static int 	m_maxMIBStreams;
	static int	m_maxSpuHandles;

	// Are we suspended?
	static int	m_suspended;

	// Synchrounous load callback
	static BlockingCallback	m_blockingCallback;
	static void				  	*m_blockingCallbackContext;

	// Files
	static Pool<PS2FileStream>	*m_fileStreamPool;
	static bool	*m_fileIDPool;
	static int LockFileID();
	static void UnlockFileID(int fileID);

	// Stream management
	static bool	*m_streamPool;
	static int m_streamPoolSize;
	static void AllocateStreamPool(int size);
	static void FreeStreamPool();

	// SRAM tracking
	static int 	m_spuHandlesUsed;
	static int 	m_maxSpuHandlesUsed;

	// SFX ID support
	struct SFX
	{
		uint spuAddr;
	};
	static SFX			*m_sfx;
	static int 			m_sfxUsed;
	static int 			m_maxSfxUsed;

	// SPU Channel support
	static SPUChannel m_channel[MAX_SOUNDCHANNELS];
	static int 			m_channelsUsed;
	static int 			m_maxChannelsUsed;

	// MIBStream support
	static Pool<MIBStream>	*m_mibStreamPool;

	// Effects support
	static uint	m_effectAddr[2];
	static int  m_effectSpuSize;

	// Load our moudule
	static bool LoadIOPModule(const char *module);

	// Allocate the ever-so-special effects ram
	static uint AllocateEffectsSRAM(int effect);

	// Memory overrides for MS
	static void *EEAlloc(int alignment, int size);
	static void EEFree(void *block);

	// Are we blocking IO?
	static bool BlockIO(void *context);

	// Fastload support
	static void *m_fastloadStack;
	static int FastLoadCallback();
};

#endif


