/////////////////////////////////////////////////////////////////////////////
// Multistream wrapper for SPU and file managment
/////////////////////////////////////////////////////////////////////////////
#include "LayersPCH.h"

#include <sifrpc.h>
#include <sifdev.h>
#include <sdmacro.h>	// HAS defines for SD_P_MVOL etc..etc..

#include "Layers/mediahal.h"
#include "Layers/mstream/streamirx.h"
#include "Layers/mstream/mstream.h"
#include "platform/gameapp.h"

// Define this for file info
#ifndef CONSUMER_BUILD
//#define FILE_DEBUG
#endif

// Main constants
int MultiStream::m_maxOpenFiles = 0;
int MultiStream::m_maxSFX = 0;
int MultiStream::m_maxMIBStreams = 0;
int MultiStream::m_maxSpuHandles = 0;

// Are we suspended?
int MultiStream::m_suspended = 0;

// Register a callback for blocking loads
MultiStream::BlockingCallback	MultiStream::m_blockingCallback = NULL;
void *MultiStream::m_blockingCallbackContext = NULL;

bool *MultiStream::m_fileIDPool = NULL;
Pool<PS2FileStream> *MultiStream::m_fileStreamPool = NULL;

bool *MultiStream::m_streamPool = NULL;
int MultiStream::m_streamPoolSize = 0;

// SRAM tracking
int MultiStream::m_spuHandlesUsed = 0;
int MultiStream::m_maxSpuHandlesUsed = 0;

// SFX ID support
MultiStream::SFX *MultiStream::m_sfx = NULL;
int MultiStream::m_sfxUsed = 0;
int MultiStream::m_maxSfxUsed = 0;

// SPU Channel support
SPUChannel MultiStream::m_channel[MAX_SOUNDCHANNELS];
int MultiStream::m_channelsUsed = 0;
int MultiStream::m_maxChannelsUsed = 0;

// Stream support
Pool<MIBStream> 	*MultiStream::m_mibStreamPool = NULL;

// Effects support
uint MultiStream::m_effectAddr[2] = { 0, 0 };
int MultiStream::m_effectSpuSize = 0;

// Fastload support
//#define USEFASTLOADCALLBACK
void *MultiStream::m_fastloadStack = NULL;

// ATWinMon
extern const char *g_RemoteIPAddress;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::Boot(
bool isDVD,
bool warm)
{
	// Does everyone call these or what?
	sceSifInitRpc(0);
	sceSifInitIopHeap();

	// Is this a warm boot?
	if (!warm)
	{
		sceCdInit(SCECdINIT);
		sceCdMmode(isDVD ? SCECdDVD : SCECdCD);
		sceCdDiskReady(0);
		sceFsReset();
		sceSifLoadFileReset();

		// Don't need these on a warm boot
		if (!LoadIOPModule("sio2man.irx"))
			return false;
	}

	// Load the module(s)
	if (!LoadIOPModule("libsd.irx"))
		return false;

#if 0
	if (!LoadIOPModule("sdrdrv.irx"))
		return false;
#endif

	if (!LoadIOPModule("stream.irx"))
		return false;

	// Start talking to the IOP module
	if (SOUND_InitIOP() < 0)
	{
		BigFatError("Can't init the IOP");
		return false;
	}

	// Make sure our callback is in place
	SOUND_InitCD(SOUND_SET_CALLBACK);

	// Make CD go at streaming speed
	SOUND_InitCD(SOUND_CD_SPIN_STREAM);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::InitializeFileSystem(
int maxOpenFiles)
{
	// Setup the allocation stuff
	SOUND_SetMemHooks((SOUND_ALLOC_FUNC)EEAlloc, (SOUND_FREE_FUNC)EEFree);

	// Use MultiStream CD/Host loading routines.
	m_maxOpenFiles = maxOpenFiles;
	SOUND_InitStreamData(SND_LOAD_INTERNAL, maxOpenFiles, 0);

	// Set this
	SOUND_SetMaxStreamLimit(maxOpenFiles);
	AllocateStreamPool(maxOpenFiles);

	// let's be super friendly with our SIF hammering
	SOUND_InitWaitUpdate();

	// Send the three SOUND_ commands above to the IOP and wait for completion
	SOUND_FlushIOPCommand(0);

	// Lets go into fastload
	const int stackSize = 32 * 1024;
	m_fastloadStack = memAlloc(stackSize, 64);
	ASSERT(m_fastloadStack);

	ChangeThreadPriority(GetThreadId(), APPLICATION_THREAD_PRIORITY);
	SOUND_InitFastLoad(APPLICATION_THREAD_PRIORITY - 1, (int)m_fastloadStack, stackSize);

	SOUND_SetFastLoadMode(SOUND_FASTLOAD_ON);
#ifdef USEFASTLOADCALLBACK
	SOUND_SetFastLoadCallback(FastLoadCallback);
#endif
	while (!SyncIOP());

	SOUND_SetFastLoadMode(SOUND_FASTLOAD_CONTINUE);
	while (!SyncIOP());

	SOUND_SetFastLoadMode(SOUND_FASTLOAD_INVALID_DCACHE_ON);
	while (!SyncIOP());

	// Make the file ID array
	m_fileIDPool = new bool[maxOpenFiles];
	ASSERT(m_fileIDPool);
	for (int i = 0; i < maxOpenFiles; i++)
		m_fileIDPool[i] = false;

	// Create the pool
	m_fileStreamPool = new Pool<PS2FileStream>(maxOpenFiles, 0);
	ASSERT(m_fileStreamPool);

	// Install our IO blocker
	MediaHAL::InstallIOBlocker(BlockIO, NULL);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::InitializeSoundSystem(
int maxSFX,
int maxMIBStreams,
int maxSRAMHandles)
{
	// Init SPU
	SOUND_InitSPU();

	// Set the maximum number of streams
	m_maxMIBStreams = maxMIBStreams;
	SOUND_SetMaxStreamLimit(maxMIBStreams);
	m_mibStreamPool = new Pool<MIBStream>(maxMIBStreams, 0);
	ASSERT(m_mibStreamPool);

	// Real streams required
	int totalStreams = m_maxMIBStreams + m_maxOpenFiles;
	SOUND_SetMaxStreamLimit(totalStreams);
	AllocateStreamPool(totalStreams);

	// Set the number of SFX
	m_maxSFX = maxSFX;
	SOUND_AllocateSpotFXMemory(maxSFX);
	m_sfx = new SFX[maxSFX];
	ASSERT(m_sfx);
	for (int s = 0; s < maxSFX; s++)
		m_sfx[s].spuAddr = 0;
	m_sfxUsed = 0;
	m_maxSfxUsed = 0;

	// Setup the SPU RAM allocator
	m_maxSpuHandles = maxSRAMHandles;
	SOUND_MemAllocInit(maxSRAMHandles, SOUND_ALLOCATE_ALL_RAM);
	m_spuHandlesUsed = 0;
	m_maxSpuHandlesUsed = 0;

	// Someing is hitting 0x5010 plus some amount, what gives....
	uint leakForProtection = SOUND_MemAllocate(16 * 1024);

	// Effects initialization
	m_effectAddr[0] = m_effectAddr[1] = 0;
	m_effectSpuSize = 0;

	// Setup the volumes
	SOUND_AllChannelOutputsOn(0);
	SOUND_AllChannelOutputsOn(1);

	// Setup effects
	SOUND_AllChannelEffectsOn(0);
	SOUND_AllChannelEffectsOn(1);

	// Send the three SOUND_ commands above to the IOP and wait for completion
	while (!SyncIOP());

	// Setup the SPU channels
	for (int c = 0; c < MAX_SOUNDCHANNELS; c++)
		m_channel[c].m_channel = c;
	m_channelsUsed = 0;
	m_maxChannelsUsed = 0;

#ifndef CONSUMER_BUILD
	SOUND_DisplayStats();
#endif

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Destroy everything sound related
/////////////////////////////////////////////////////////////////////////////
void MultiStream::TerminateSoundSystem()
{
	// Setup effects
	SOUND_AllChannelEffectsOff(0);
	SOUND_AllChannelEffectsOff(1);

	// Setup the volumes
	SOUND_AllChannelOutputsOff(0);
	SOUND_AllChannelOutputsOff(1);

	delete [] m_sfx;
	m_sfx = NULL;

	delete m_mibStreamPool;
	m_mibStreamPool = NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::TerminateFileSystem()
{
	// Be explicit about these...
	SOUND_CloseFastLoad();
	SOUND_CloseWaitUpdate();

	// destroy everything file related
	SOUND_CloseEverything();
	SOUND_FlushIOPCommand(0);

	// Wee!
	FreeStreamPool();

	delete m_fileStreamPool;
	m_fileStreamPool = NULL;

	delete [] m_fileIDPool;
	m_fileIDPool = NULL;

	memFree(m_fastloadStack);
	m_fastloadStack = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The Daemon!
/////////////////////////////////////////////////////////////////////////////
void MultiStream::Daemon()
{
	// Don't do anything if we are suspended
	if (m_suspended > 0)
		return;

#if 1
	SOUND_HandleCDErrors();	// No..Its safe to check for CD errors
#else
	if (SOUND_ReadyToLoad() == SND_LOAD_INTERNAL)
	{
		SOUND_EXTERNAL_INFO externalInfo;
		if (SOUND_LoadComplete(&externalInfo) != -1)
			SOUND_GetStatus((int)externalInfo.StreamRequest, (int)externalInfo.RequestCount);
	}

	// Check for errors
	SOUND_HandleCDErrors();
#endif

	SyncIOP(false);

	// Look for problematic channels
	for (int channel = 0; channel < MAX_SOUNDCHANNELS; channel++)
	{
		int status = SOUND_GetKeyStatus(channel);
		if (status != 4)
			m_channel[channel].m_at4 = 0;
		else
		{
			++m_channel[channel].m_at4;
			if (!m_channel[channel].m_locked && m_channel[channel].m_at4 > 8)
			{
				TRACE("Whoa channel %d (%s) is waiting on IOP but is not locked!!!\n", channel, m_channel[channel].m_name);
				ASSERT(false);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Totally pause and resume multistream (MS will not access CD)
// This is useful for running DNAS
/////////////////////////////////////////////////////////////////////////////
void MultiStream::Suspend()
{
	if (m_suspended == 0)
	{
		SOUND_SetFastLoadMode(SOUND_FASTLOAD_OFF);
		SOUND_Pause();
		SOUND_FlushIOPCommand(0);

		SOUND_WaitForCDFree(SOUND_WAIT_HOT);
		SOUND_FlushIOPCommand(0);

		SOUND_CloseWaitUpdate();
		SOUND_FlushIOPCommand(0);

		SOUND_DisableSPUCallback();
		SOUND_DisableCDCallback();
		SOUND_FlushIOPCommand(0);
	}

	++m_suspended;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::Resume()
{
	ASSERT(m_suspended > 0);
	if (m_suspended == 1)
	{
		SOUND_InitCD(SOUND_SET_CALLBACK);
		SOUND_EnableSPUCallback();
		SOUND_FlushIOPCommand(0);

		SOUND_Resume();
		SOUND_SetFastLoadMode(SOUND_FASTLOAD_ON);
		SOUND_SetFastLoadMode(SOUND_FASTLOAD_CONTINUE);
   	SOUND_FlushIOPCommand(0);

		SOUND_InitWaitUpdate();
   	SOUND_FlushIOPCommand(0);
	}

	--m_suspended;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::Suspended()
{
	return (m_suspended > 0);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::RegisterBlockingCallback(
BlockingCallback callback,
void *context)
{
	m_blockingCallback = callback;
 	m_blockingCallbackContext = context;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::ClearBlockingCallback()
{
	RegisterBlockingCallback(NULL, NULL);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::CallBlockingCallback()
{
	if (m_blockingCallback)
		m_blockingCallback(m_blockingCallbackContext);
}

/////////////////////////////////////////////////////////////////////////////
// open a file
/////////////////////////////////////////////////////////////////////////////
PS2FileStream *MultiStream::OpenFile(
const char *filename,
bool async)
{
	// Weed out stupidity
	ASSERT(filename && *filename);
	if (!filename || filename[0] == '\0')
		return NULL;

	// Something is really wrong
	if (!m_fileStreamPool)
		return NULL;

	// Create the fileID
	int size;
	int fileID = OpenFileID(filename, &size);
	if (fileID < 0)
		return NULL;

	// Now make the object
	void *buffer = m_fileStreamPool->NewPtr();
	ASSERT(buffer);
	if (!buffer)
	{
		CloseFileID(fileID);
		BigFatError("Cannot allocate file stream");
		return NULL;
	}

	// This can't fail
	return new (buffer) PS2FileStream(fileID, size, async, true);
}

/////////////////////////////////////////////////////////////////////////////
// open a file
/////////////////////////////////////////////////////////////////////////////
PS2FileStream *MultiStream::CreateFile(
int fileID,
int size,
bool async)
{
	// Something is really wrong
	if (!m_fileStreamPool)
		return NULL;

	// Is this okay
	if (fileID < 0 || size <= 0)
		return NULL;

	// Now make the object
	void *buffer = m_fileStreamPool->NewPtr();
	ASSERT(buffer);
	if (!buffer)
	{
		BigFatError("Cannot allocate file stream");
		return NULL;
	}

	// This can't fail
	return new (buffer) PS2FileStream(fileID, size, async, false);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::CloseFile(PS2FileStream *stream)
{
	if (!stream)
		return;

	ASSERT(m_fileStreamPool);
	m_fileStreamPool->Delete(stream);
}

/////////////////////////////////////////////////////////////////////////////
// Sleep for so many microseconds
/////////////////////////////////////////////////////////////////////////////
void MultiStream::Wait(
uint microseconds)
{
	// What time is it now?
	uint start;
	asm __volatile__ ("mfc0 %0, $9" : "=r" (start));

	// When are we going to sleep until?
	uint sleepCycles = microseconds * 299;
	uint sleepTo = start + sleepCycles;

	// Did we wrap?
	uint adjust;
	if (sleepTo > start)
		adjust = 0;
	else
	{
		adjust = 0 - start;
		sleepTo += adjust;
	}

	// Wait!
	uint now;
	do {
		asm __volatile__ ("mfc0 %0, $9" : "=r" (now));
	} while (now + adjust < sleepTo);
}

/////////////////////////////////////////////////////////////////////////////
// Talk to the IOP in a friendly way
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::SyncIOP(
bool wait)
{
	// How many MS should we wait?
	uint waitMS = wait ? 1000 : 0;

	// How long before we ask the SIF again?
	const uint usWait = 33;

	// Figure out maxTries
	int maxTries = waitMS * 1000 / usWait;

	// Wait until we flush
	int tries = 0;
	while (tries < maxTries && SOUND_CheckRPCStatus() != 0)
	{
		// Do something here, other than looping violently
		Wait(usWait); // Wait for n microseconds

		// Make sure we don't totally die
		++tries;
	}

	// Did we time out?
	ASSERTS(!wait || maxTries == 0 || tries < maxTries, "SIF is hung, go get some coffee!");

#ifdef _DEBUG
	float waited = (float)tries * (float)usWait * 0.001f;
	if (tries > 0)
		TRACE("!!!! Waited %f ms for SIF to wake the hell up !!!!\n", waited);
#endif

	// Shit, get out maybe the next Sync will really work
	// but this is really bad!
	if (SOUND_CheckRPCStatus() != 0)
		return false;

	// Do the flush
	int flush = SOUND_FlushIOPCommand(wait ? 0 : SIF_RPCM_NOWAIT);

	// See if we are moving along
	ASSERTS(flush >= 0, "IOP is choking!");

	// Did the flush work?
	return (flush >= 0);
}

/////////////////////////////////////////////////////////////////////////////
// Use iop/ee synchronization
/////////////////////////////////////////////////////////////////////////////
void MultiStream::BeginReadyCheck()
{
	SOUND_SetWaitUpdate();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::IsReady()
{
	return (SOUND_CheckForUpdate() != 0);
}

/////////////////////////////////////////////////////////////////////////////
// Allocate some stream SPU RAM, size MUST BE 16 byte aligned!!
/////////////////////////////////////////////////////////////////////////////
uint MultiStream::AllocateSRAM(
int size)
{
	// Must be 16 byte aligned
	ASSERT((size % 16) == 0);

	// Give it it back
	int ptr = SOUND_MemAllocate(size);

	// um, check for out of memory situations
	if (ptr == -1)
	{
		ASSERT(false && "Out of SPU RAM Handles!");
	 	return 0;
	}
	else if (ptr == -2)
	{
		ASSERT(false && "Out of SPU RAM!");
	 	return 0;
	}

	// track
	if (++m_spuHandlesUsed > m_maxSpuHandlesUsed)
		m_maxSpuHandlesUsed = m_spuHandlesUsed;

	// Give them pointer
	return (uint)ptr;
}

/////////////////////////////////////////////////////////////////////////////
// Free some SPU RAM
/////////////////////////////////////////////////////////////////////////////
void MultiStream::FreeSRAM(
uint &addr)
{
	// It's okay to free a bad alloc
	if (!addr)
		return;

	// Free it
	SOUND_MemFree(addr);

	// Null it for them
	addr = 0;

	// track
	--m_spuHandlesUsed;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::TransferEEToSRAM(
void *data,
uint size,
uint spuAddr)
{
	uint  realSize  = size;
	char* eeBuffer  = (char*) data;
	bool  ownBuffer = false;

	if ((realSize & 63) != 0 || ((int)eeBuffer & 63) != 0)
	{
		BigFatError("EE->SPU transfer not 64-byte aligned" );

		// Must be 64 byte aligned
		realSize = SizeBy64(realSize);

		// Have to use a 64-byte aligned buffer
		ownBuffer = true;
		eeBuffer = (char *)memAlloc(realSize, 64);
		if (!eeBuffer)
		{
			BigFatError("Out of EE RAM transferring EE->SPU.");
			return false;
		}

		// Copy in the data
		memcpy(eeBuffer, data, size);

		// Zero out the end because we are paranoid
		if (realSize != size)
			memset(&eeBuffer[size], '\0', realSize - size);
	}

	// Allocate heap n upload from EE to IOP
	void *iopBuffer = sceSifAllocIopHeap(realSize);
	if (!iopBuffer)
	{
		if (ownBuffer)
			memFree(eeBuffer);
		BigFatError("Out of IOP RAM transferring EE->SPU.");
		return false;
	}

	// Transfer the data!
	int ret = SOUND_TransferDataToIOP((u32)eeBuffer, (u32)iopBuffer, realSize, SOUND_DMA_WAIT);
	if (ret != 0)
	{
		sceSifFreeIopHeap(iopBuffer);
		if (ownBuffer)
			memFree(eeBuffer);
		BigFatError("IOP failed transferring EE->SPU.");
		return false;
	}

	// Transfer it to the spu
	SOUND_TransIOPToSPU((u32)iopBuffer, spuAddr, realSize);
	while (!SyncIOP());

	// Free it from the IOP
	sceSifFreeIopHeap(iopBuffer);
	if (ownBuffer)
		memFree(eeBuffer);

	// Somehow this worked.
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Effects support
/////////////////////////////////////////////////////////////////////////////
int MultiStream::GetEffectSize(
int effect)
{
	return SOUND_GetHardwareEffectSize(effect);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::PrepareEffect(
int effect,
bool useBestFit)
{
	// Early bailout
	if (!useBestFit && GetEffectSize(effect) <= m_effectSpuSize &&
			m_effectAddr[0] != 0 && m_effectAddr[1] != 0)
		return true;

	// Always flush to make smaller or if we need bigger
	if (useBestFit || GetEffectSize(effect) > m_effectSpuSize)
	{
		FreeSRAM(m_effectAddr[0]);
		FreeSRAM(m_effectAddr[1]);
	}

	// Allocate the new stuff
	if (m_effectAddr[0] == 0 || m_effectAddr[1] == 0)
	{
		ASSERT(m_effectAddr[0] == 0 && m_effectAddr[1] == 0);
		m_effectSpuSize = GetEffectSize(effect);
		m_effectAddr[0] = AllocateEffectsSRAM(effect);
		m_effectAddr[1] = AllocateEffectsSRAM(effect);
		ASSERTS(m_effectAddr[0] && m_effectAddr[1], "Could not allocate SRAM effects buffers!");
	}

	// Set the buffers
	if (m_effectAddr[0] == 0 || m_effectAddr[1] == 0)
	{
		FreeSRAM(m_effectAddr[0]);
		FreeSRAM(m_effectAddr[1]);
 		m_effectSpuSize = 0;
		return false;
	}

	// Set the buffers!
	uint orWith;
	if (effect == SOUND_FX_ECHO || effect == SOUND_FX_DELAY)
		orWith = 0x1ffff;
	else
		orWith = 0xffff;
	int ret = SOUND_SetEffectBufferAddress(m_effectAddr[0] | orWith, m_effectAddr[1] | orWith);
	ASSERT(ret >= 0);
	SyncIOP(true);

	// We got it!
	return (ret >= 0);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::EnableEffect(
int effect,
int depthL,
int depthR,
int delay,
int feedback)
{
	int core0 = SOUND_EnableEffects(0, effect, depthL, depthR, delay, feedback);
	int core1 = SOUND_EnableEffects(1, effect, depthL, depthR, delay, feedback);

	return (core0 == 0 && core1 == 0);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::SetEffectVolume(
int depthL,
int depthR)
{
	// Wee!
	SOUND_SetEffectMasterVolume(0, depthL, depthR);
	SOUND_SetEffectMasterVolume(1, depthL, depthR);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::ClearEffects()
{
	PrepareEffect(SOUND_FX_OFF, true);
	EnableEffect(SOUND_FX_OFF, 0, 0, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SFXID MultiStream::AllocateSFXID(
uint spuAddr)
{
	// Find an open ID
	for (int id = 0; id < m_maxSFX; id++)
	{
		// Free?
		if (!m_sfx[id].spuAddr)
		{
			// Reserve it!
			m_sfx[id].spuAddr = spuAddr;

			// Bind the id to the ram address
			SOUND_PatchSFX(id, spuAddr);
			SyncIOP();

			// track
			if (++m_sfxUsed > m_maxSfxUsed)
				m_maxSfxUsed = m_sfxUsed;

			// Return the ID
			return id;
		}
	}

	// Shit, we failed
	DebugDump();
	ASSERTS(false, "Out of SFX slots!");

	return SFXID_INVALID;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::FreeSFXID(
SFXID id)
{
	// This is legal
	if (id == SFXID_INVALID)
		return;

	// Make sure this isn't bogus
	ASSERT(id >= 0 && id < m_maxSFX);

	// Free the id
	SOUND_FreeSpotFXID(id);
	m_sfx[id].spuAddr = 0;

	// We may not need this
	SyncIOP();

	// track
	--m_sfxUsed;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
SPUChannel *MultiStream::AllocateChannel(
const char *name)
{
	// Find and open channel
	for (int channel = 0; channel < MAX_SOUNDCHANNELS; channel++)
	{
		int status = SOUND_IsFreeSPUChannel(channel);
		if (status == 1 && !m_channel[channel].m_locked)
		{
			// Give them the channel
			m_channel[channel].m_locked = true;
			strncpy(m_channel[channel].m_name, name, sizeof(m_channel[channel].m_name) - 1);

			// track
			if (++m_channelsUsed > m_maxChannelsUsed)
				m_maxChannelsUsed = m_channelsUsed;

			return &m_channel[channel];
		}
	}

	// Damn we are out of channels!
	DebugDump();
	ASSERTF(false, ("Out of SPU channels (%s)!!", name));
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::FreeChannel(
SPUChannel *channel)
{
	// Sanity check
	if (!channel)
		return;

	// Mark it as free
	ASSERT(m_channel[channel->m_channel].m_locked);
	m_channel[channel->m_channel].m_locked = false;

	// Track
	--m_channelsUsed;
}

/////////////////////////////////////////////////////////////////////////////
// Stream support (-1 is invalid)
/////////////////////////////////////////////////////////////////////////////
int MultiStream::AllocateStreamID()
{
	for (int i = 0; i < m_streamPoolSize; i++)
	{
		if (!m_streamPool[i])
		{
			m_streamPool[i] = true;
			return i;
		}
	}

	// Doh!
	BigFatError("Out of streams!");
	return -1;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::FreeStreamID(int &streamID)
{
	ASSERT(streamID < m_streamPoolSize);
	if (streamID < 0 || streamID >= m_streamPoolSize)
		return;

	ASSERT(m_streamPool[streamID]);
 	m_streamPool[streamID] = false;;
	streamID = -1;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::StopStream(int streamID)
{
	if (streamID < 0)
		return false;

	return (SOUND_StopStream(streamID) == 0);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::OpenStreamBuffer(
int streamID,
uint spuAddr,
int size)
{
	if (streamID < 0)
		return false;

	return (SOUND_AllocateStreamBuffer(streamID, spuAddr, size) == 0);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::CloseStreamBuffer(int streamID)
{
	if (streamID < 0)
		return false;

	return (SOUND_CloseStreamBuffer(streamID) == 0);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::StreamLoadEE(
int streamID,
int fileID,
uint eeAddr)
{
	if (streamID < 0 || fileID < 0)
		return false;

	ASSERT((eeAddr % 64) == 0);
	int ret = SOUND_LoadFile(STREAM_LOAD_EE, streamID, fileID, eeAddr);
	SyncIOP();

	return (ret >= 0);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::StreamSetEEAddress(
int streamID,
uint eeAddr)
{
	if (streamID < 0)
		return false;

	SOUND_ModifyEETransferAddress(streamID, eeAddr);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::SetFastLoadNext(int streamID)
{
	return (SOUND_SetFastLoadNext(streamID) == 0);
}

/////////////////////////////////////////////////////////////////////////////
// It we hit this call, it seems to wake gameclient up.
/////////////////////////////////////////////////////////////////////////////
extern void SendToRemoteTerminal(const char *);

/////////////////////////////////////////////////////////////////////////////
// Create a fileID for a file (return -1 if not there)
/////////////////////////////////////////////////////////////////////////////
int MultiStream::OpenFileID(
const char *pathFile,
int *size)
{
	// Do we have an open fileD?
	int fileID = LockFileID();
	if (fileID < 0)
	{
		BigFatError("Out of file ID's");
		return -1;
	}

	// What a hack (but this seems to work out the info
	if (UsingATWinMon())
		SendToRemoteTerminal("Multistream says Wake up!.");

	// Show some info
#ifdef FILE_DEBUG
	TRACE("Multistream: Opening \"%s\": ", pathFile);
#endif

	// Make sure we are kosher to hit the CD
	int state = SuspendFastLoad();
	SOUND_WaitForCDFree(SOUND_WAIT_HOT);

	// Get the file info
	int tagID = SOUND_GetFileInfo(pathFile);
	SyncIOP();

	// Restore our fastload mode
	ResumeFastLoad(state);

	// Does it exist?
	SOUND_FILE_INFO fileInfo;
	memset(&fileInfo, '\0', sizeof(fileInfo));
	int returnTagID = SOUND_ReturnFileInfo(&fileInfo);

	ASSERT(tagID == returnTagID);
	if (tagID != returnTagID)
	{
#ifdef FILE_DEBUG
		TRACE("failed, could not GetFileInfo.\n");
#endif
		UnlockFileID(fileID);
		BigFatError("Error getting file information!", pathFile);
		return -1;
	}

	// File not found
	if (fileInfo.Size == 0)
	{
#ifdef FILE_DEBUG
		TRACE("failed, file size is zero.\n");
#endif
		UnlockFileID(fileID);
		return -1;
	}

	// Give them back the size!
	if (size)
		*size = fileInfo.Size;

#ifdef FILE_DEBUG
	TRACE("success, file size is %d.\n", fileInfo.Size);
#endif

#ifdef CDROM
	const bool onCD = true;
#else
	const bool onCD = false;
#endif

	// Create it
	SOUND_SetFileInfo(fileID, fileInfo.LSN, fileInfo.Size, onCD ? ADPCM_DATA_ON_CD : ADPCM_DATA_ON_HOST, 0);
	if (!onCD)
		SOUND_StoreHOST0FileName(fileID, pathFile);

	// Tell atwinmon we are going nutz
	if (UsingATWinMon())
		SOUND_ATWinMonOpenFile(fileID);

	// Get everything together
	SyncIOP();

#ifdef FILE_DEBUG
	// Show the info for this freaking file
	TRACE("File info for %s\n", pathFile);
	TRACE("FileID: %d LSN: %d Size: %d Source %d\n", fileID, fileInfo.LSN, fileInfo.Size, fileInfo.Source);
#endif

	// Give it back
	return fileID;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::CloseFileID(
int &fileID)
{
	// Valid to call with invalid ID
	if (fileID < 0)
		return;

	// ATMon crap
	if (UsingATWinMon())
		SOUND_ATWinMonCloseFile(fileID);

	// Make sure we free the fileID
	SOUND_FreeFileID(fileID);
	SyncIOP();

	// Make sure we unown this
	UnlockFileID(fileID);
	fileID = -1;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::FileExists(const char *pathFile)
{
#ifdef FILE_DEBUG
	TRACE("Doing a file exists !!!!WARNING!!!! \"%s\": ", pathFile);
#endif

	// Make sure we are kosher to hit the CD
	int state = SuspendFastLoad();
	SOUND_WaitForCDFree(SOUND_WAIT_HOT);

	// Get the file info
	int tagID = SOUND_GetFileInfo(pathFile);
	SyncIOP();

	// Restore our fastload mode
	ResumeFastLoad(state);

	// Does it exist?
	SOUND_FILE_INFO fileInfo;
	memset(&fileInfo, '\0', sizeof(fileInfo));
	int returnTagID = SOUND_ReturnFileInfo(&fileInfo);

	ASSERT(tagID == returnTagID);
	if (tagID != returnTagID)
	{
		BigFatError("Error getting file information!", pathFile);
		return -1;
	}

#ifdef FILE_DEBUG
	TRACE("%s\n", (fileInfo.Size != 0) ? "exists!" : "no luck!");
#endif

	// File not found
	return (fileInfo.Size != 0);
}

/////////////////////////////////////////////////////////////////////////////
// Stream manipulation!
/////////////////////////////////////////////////////////////////////////////
MIBStream *MultiStream::CreateStream(
const char *streamName)
{
	// You are hosed!
	if (m_suspended > 0)
		return NULL;

	// wee!
	ASSERT(streamName && streamName[0]);
	if (!streamName || streamName[0] == '\0')
		return NULL;

	// Is something major wrong?
	if (!m_mibStreamPool)
		return NULL;

	// Create it
	void *buffer = m_mibStreamPool->NewPtr();
	ASSERT(buffer);
	if (!buffer)
		return NULL;

	// This can't fail
	MIBStream *stream = new (buffer) MIBStream(streamName);
	ASSERT(stream);

	return stream;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::DestroyStream(
MIBStream *stream)
{
	ASSERT(m_suspended == 0);

	if (!stream)
		return;

	// They aren't here unless we have a streamPool
	ASSERT(m_mibStreamPool);
	if (m_mibStreamPool)
		m_mibStreamPool->Delete(stream);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::AutoDMAVolume(
int channel,
int volumeL,
int volumeR,
bool force)
{
	SOUND_SetParam(channel | SD_P_BVOLL, volumeL);
	SOUND_SetParam(channel | SD_P_BVOLR, volumeR);
	if (force)
		SyncIOP();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::AutoDMAStartWrite(
int channel,
bool loop,
int iopAddr,
int size,
int offset)
{
	int flags = SD_TRANS_MODE_WRITE_FROM;
	if (loop)
		flags |= SD_BLOCK_LOOP;

	SOUND_BlockTrans(channel, flags, iopAddr, size, offset);
	SyncIOP();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::AutoDMAStop(int channel)
{
	// Stop the autodma
	SOUND_BlockTrans(channel, SD_TRANS_MODE_STOP, 0, 0, 0);
	SyncIOP();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int MultiStream::GetPCMPlaybackPosition()
{
	SOUND_FlushIOPCommand(0);

	SOUND_PCM_INFO pcmInfo;
	SOUND_GetPCMInfo(&pcmInfo);

	return pcmInfo.IOPCurrent;
}

/////////////////////////////////////////////////////////////////////////////
// Some direct control over fastload stuff
/////////////////////////////////////////////////////////////////////////////
void MultiStream::OverrideFastLoadMode(int newMode)
{
	SOUND_SetFastLoadMode(newMode);
	while (!SyncIOP());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::RestoreFastLoadMode()
{
	SOUND_SetFastLoadMode(SOUND_FASTLOAD_INVALID_DCACHE_ON);
	while (!SyncIOP());

	SOUND_SetFastLoadMode(SOUND_FASTLOAD_CONTINUE);
	while (!SyncIOP());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::GetFastLoadInfo(
SOUND_FASTLOAD_INFO &info)
{
	int ret = SOUND_GetFastLoadInfo(&info);
	SOUND_HandleCDErrors();
	return (ret != 0);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int MultiStream::SuspendFastLoad()
{
	// Make sure we are kosher to hit the CD
	SOUND_FASTLOAD_INFO fastLoadInfo;
	SOUND_GetFastLoadInfo(&fastLoadInfo);
	int eeState = fastLoadInfo.EEStatus;
	ASSERT((eeState & 0xffff0000) == 0);
	int loadStatus = fastLoadInfo.LoadStatus;
	ASSERT((loadStatus & 0xffff0000) == 0);
	SOUND_SetFastLoadMode(SOUND_FASTLOAD_OFF);
	SyncIOP();

	return (eeState | (loadStatus << 16));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::ResumeFastLoad(
int state)
{
	// Restore our fastload mode
	int eeState = (state & 0xffff);
	int loadStatus = ((state >> 16) & 0xffff);
	SOUND_SetFastLoadMode(eeState);
	SOUND_SetFastLoadMode(loadStatus);
	SyncIOP();
}

/////////////////////////////////////////////////////////////////////////////
// Is ATWinMon running?
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::UsingATWinMon()
{
	return (g_RemoteIPAddress != 0);
}

/////////////////////////////////////////////////////////////////////////////
// Dump some useful sound information
/////////////////////////////////////////////////////////////////////////////
void MultiStream::DebugDump()
{
	// Shove one of these in here
	SyncIOP(false);

	// Show off some stats
	TRACE("Largest chunk of SPU RAM: %d bytes\n", SOUND_MemFindLargest());
	TRACE("SPU RAM Handles used %d (%d hwm) of %d\n", m_spuHandlesUsed, m_maxSpuHandlesUsed, m_maxSpuHandles);
	TRACE("SFX Handles used %d (%d hwm) of %d\n", m_sfxUsed, m_maxSfxUsed, m_maxSFX);
	TRACE("Channels used %d (%d hwm) of %d\n", m_channelsUsed, m_maxChannelsUsed, MAX_SOUNDCHANNELS);

	// Show channel status
	TRACE("Channel lock: \n\t");
	int c;
	for (c = 0; c < MAX_SOUNDCHANNELS; c++)
	{
		if (m_channel[c].m_locked)
			TRACE("1");
		else
			TRACE("0");
	}
	TRACE("\n");

	TRACE("Channel status (Free, Active, Stream, EE, IOP wait, Xerror):\n\t");
	for (c = 0; c < MAX_SOUNDCHANNELS; c++)
	{
		int status = SOUND_GetKeyStatus(c);
		switch (status)
		{
			case 0:
				TRACE("F");
				break;
			case 1:
				TRACE("A");
				break;
			case 2:
				TRACE("S");
				break;
			case 3:
				TRACE("E");
				break;
			case 4:
				TRACE("I");
				break;
			default:
				TRACE("X");
				break;
		}
	}
	TRACE("\n");

	// Look for problematic channels
	for (c = 0; c < MAX_SOUNDCHANNELS; c++)
	{
		int status = SOUND_GetKeyStatus(c);
		if (status == 4 && !m_channel[c].m_locked)
			TRACE("Whoa channel %d (%s) is waiting on IOP but is not locked!!!\n", c, m_channel[c].m_name);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Load the module
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::LoadIOPModule(
const char *module)
{
	// Get the platform specific name
	char path[256];
	strcpy(path, "irx\\");
	strcat(path, module);
	char *irxName = Media::GetFileName(path);

	// Load it!
	int triesLeft = 32;
	while (sceSifLoadModule(irxName, 0, NULL) < 0)
	{
		TRACE("Loading stream.irx failed\n");
		if (--triesLeft == 0)
		{
			BigFatError("Can't load IRX ", irxName);
			return false;
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Memory overrides for MS
/////////////////////////////////////////////////////////////////////////////
void *MultiStream::EEAlloc(
int alignment,
int size)
{
	return memAlloc(size, alignment);
}

/////////////////////////////////////////////////////////////////////////////
// Memory overrides for MS
/////////////////////////////////////////////////////////////////////////////
void MultiStream::EEFree(
void *block)
{
	if (block)
		memFree(block);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
uint MultiStream::AllocateEffectsSRAM(int effect)
{
	// Make the new ones
	int effectAddr = SOUND_MemAllocateEffectBuffer(effect);
	if (effectAddr < 0)
		return 0;

	// One more handle
	if (++m_spuHandlesUsed > m_maxSpuHandlesUsed)
		m_maxSpuHandlesUsed = m_spuHandlesUsed;

	return effectAddr;
}

/////////////////////////////////////////////////////////////////////////////
// Big fat error handler
/////////////////////////////////////////////////////////////////////////////
void MultiStream::BigFatError(
const char *description,
const char *fileName)
{
	TRACE("*******************************************************************\n");
	TRACE("*******************************************************************\n");
	TRACE("*******************************************************************\n");
	TRACE("Sound Driver Error::");
	if (fileName)
	{
		TRACE(" file \"");
		TRACE(fileName);
		TRACE("\" --> \"");
	}
	TRACE(description);
	TRACE("!!!\"\n");
	TRACE("*******************************************************************\n");
	TRACE("*******************************************************************\n");
	TRACE("*******************************************************************\n");
	ASSERT(false);
}

/////////////////////////////////////////////////////////////////////////////
// Are we blocking IO?
/////////////////////////////////////////////////////////////////////////////
bool MultiStream::BlockIO(
void *context)
{
	// this is more friendly
	SOUND_WaitForCDFree(SOUND_WAIT_HOT);
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// Allocate a file ID
/////////////////////////////////////////////////////////////////////////////
int MultiStream::LockFileID()
{
	// Do we have an open fileD?
	for (int i = 0; i < m_maxOpenFiles; i++)
	{
		if (!m_fileIDPool[i])
		{
			m_fileIDPool[i] = true;
			return i;
		}
	}

	BigFatError("No free FileID's");
	return -1;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::UnlockFileID(
int fileID)
{
	// Valid to call with invalid ID
	if (fileID < 0)
		return;

	// Make sure we own this
	ASSERT(m_fileIDPool[fileID] == true);
	m_fileIDPool[fileID] = false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::AllocateStreamPool(int size)
{
	ASSERT(size != 0);

	if (size != m_streamPoolSize)
	{
		// Create the newone
		bool *newStreamPool = new bool[size];
		ASSERT(newStreamPool);

		// Do we have to transfer data over
		if (!m_streamPool)
		{
			// Mark all as open
			for (int i = 0; i < size; i++)
				newStreamPool[i] = false;
		}
		else
		{
			// Inherit
			for (int i = 0; i < size; i++)
			{
				if (i < m_streamPoolSize)
					newStreamPool[i] = m_streamPool[i];
				else
					newStreamPool[i] = false;
			}

			// Make sure we aren't losing too many streams
			if (m_streamPoolSize > size)
			{
				for (int test = size; test < m_streamPoolSize; test++)
				{
					// If we are shrinking the pool, and a stream is in use
					// bark
					if (m_streamPool[test])
					{
						BigFatError("Resized stream pool smaller than used streams!!!!\n");
						ASSERT(false);
						break;
					}
				}
			}
		}

		// Establish the new one
		delete [] m_streamPool;
		m_streamPool = newStreamPool;
		m_streamPoolSize = size;
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MultiStream::FreeStreamPool()
{
	delete [] m_streamPool;
	m_streamPool = NULL;
	m_streamPoolSize = 0;
}

/*****************************************************************************
FastloadCallback
	This is the callback which is set to be called when a IOP->EE transfer
	RPC is called.

	Requires:	Nothing
	Returns:	SOUND_FASTLOAD_CONTINUE (allow more data to be loaded/transferred)
				SOUND_FASTLOAD_STOP	(stop any more data being transfered)
				0 (do nothing..carry on with the last mode set)

	Note:		If returning SOUND_FASTLOAD_STOP, you can continue loading
				more data via SOUND_GetStatus(StreamChannel,0);

*****************************************************************************/
int MultiStream::FastLoadCallback()
{
	SOUND_FASTLOAD_INFO fastLoadInfo;
	SOUND_GetFastLoadInfo(&fastLoadInfo);

	int eeStatus = (int)fastLoadInfo.EEStatus;
	int iopStatus = (int)fastLoadInfo.IOPStatus;
	int stream = (int)fastLoadInfo.StreamChannel;
	int totalSize = (int)fastLoadInfo.TotalSize;
	int transSize = (int)fastLoadInfo.LoadSize;
	unsigned int eeAddress = fastLoadInfo.EEAddress;

#if 0
	TRACE("Fastload status: ee - %d iop - %d stream - %d total - %d transfer - %d addr - %x\n",
				eeStatus, iopStatus, stream, totalSize, transSize, eeAddress);
#endif

	// Final transfer completed?
	if (fastLoadInfo.Status == 1)
	{
#if 0
		TRACE("Fastload transfer done!\n");
#endif
	}

	return 0;	// Use last return mode (set with SOUND_SetFastLoadMode())
}

