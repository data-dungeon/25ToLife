/////////////////////////////////////////////////////////////////////////////
// This is the driver for FMV video
/////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include <libgraph.h>

#include "platform/BaseType.h"
#include "Layers/Media.h"
#include "Layers/Memory.h"
#include "platform/CompilerControl.h"	// for NAMED_SECTION
#include "Display/Common/DisplayState.h"
#include "Display/Common/DisplayPlatform.h"
#include "Display/PS2/Renderer/Code/MasterList.h"
#include "Display/Common/DisplayState.h"

#include "Layers/Debug.h"

#include "fmv/ps2/playpss.h"
#include "fmv/ps2/fmvdriver.h"

#include "Game/Managers/TextureMgr/PlatformImage.h"

// damn it is just amazing there isn't an API for this
#define SCRATCHPAD_SIZE 16384

// TJC - the space used by the player in VRAM
#define VRAM_USE_DEPTH 4
#define VRAM_USE_NUM_BUFFERS 2
#define VRAM_USE_BYTES (MAX_WIDTH * MAX_HEIGHT * VRAM_USE_NUM_BUFFERS * VRAM_USE_DEPTH)
#define VRAM_USE_BLOCKS ((u32)(VRAM_USE_BYTES / 256))

// TJC - save from vram data...
// TJC - for some reason, u32TextureOrigBaseAddr doesnt seem to be
// before the white texture liek the docs say.. so I moved back a
// little to make sure we get it..(I moved it back even farther)
#define VRAM_SAVE_ADDR (Math::Max(0u,(DisplayState.u32TextureOrigBaseAddr-128)))
// TJC - I should only have to save up to the lowest of these, but
// sometimes things are still getting corrupted.. so I'll use the
// top of texmem and see what happens... (perhaps my calculation above
// for used bytes is wrong somehow.. doesnt count something or ...)
#define VRAM_SAVE_TOP DisplayState.u32TextureAddr//(Math::Max(DisplayState.u32TextureAddr,VRAM_USE_BLOCKS))
#define VRAM_SAVE_BLOCKWIDTH 16
#define VRAM_SAVE_BLOCKHEIGHT 8
#define VRAM_SAVE_TRANSFERWIDTH 64
#define VRAM_SAVE_NUMBLOCKS (VRAM_SAVE_TOP - VRAM_SAVE_ADDR)
#define VRAM_SAVE_NUMBLOCKSWIDE (VRAM_SAVE_TRANSFERWIDTH / VRAM_SAVE_BLOCKWIDTH)
#define VRAM_SAVE_HEIGHT ((VRAM_SAVE_BLOCKHEIGHT * VRAM_SAVE_NUMBLOCKS) / VRAM_SAVE_NUMBLOCKSWIDE)

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FMVDriver::FMVDriver(void)
{
	session = NULL;
	scratchPadSave = NULL;
	vramSave = NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FMVDriver::~FMVDriver(void)
{
	ASSERT(scratchPadSave == NULL);
	ASSERT(vramSave == NULL);
}

/////////////////////////////////////////////////////////////////////////////
// Make a full filename of what an FMV would be on this platform
/////////////////////////////////////////////////////////////////////////////
void FMVDriver::MakeFile(
const char *fmv,
char *fullFile)
{
	strcpy(fullFile, Directory());
	strcat(fullFile, fmv);
	strcat(fullFile, ".");
	strcat(fullFile, Extension());
}

/////////////////////////////////////////////////////////////////////////////
// Is this a valid/existing fmv for this driver?
// Note, pass in fmv name, not the full path
/////////////////////////////////////////////////////////////////////////////
bool FMVDriver::Valid(
const char *fmv)
{
	// Doh, no fmvs on atwinmon
	if (MultiStream::UsingATWinMon())
		return false;

	// Build the file name
	char pathFile[128];
	MakeFile(fmv, pathFile);

	// We probably should validate it a little
	return(Media::StreamExists(pathFile));
}

/////////////////////////////////////////////////////////////////////////////
// Create the device
/////////////////////////////////////////////////////////////////////////////
bool FMVDriver::Create(
float volume,
bool preserveVRAM)
{
	// Doh, no fmvs on atwinmon
	if (MultiStream::UsingATWinMon())
		return false;

	dbgPrint("switching video mode\n");

	// Do something sensible with this sensible call
	MasterList::Cleanup();

	// TJC - save vram
	if (preserveVRAM && !SaveVRAM())
		return false;

	// switch to the correct video mode
	sceGsResetGraph(0, SCE_GS_INTERLACE, DisplayState.bNTSCMode ? SCE_GS_NTSC : SCE_GS_PAL, SCE_GS_FIELD);

	// Save the scratch pad
	if (!SaveScratchPad())
	{
		Destroy();
		return false;
	}

	// Create the session
	session = StartMpegSession(volume);
	if (!session)
	{
		Destroy();
		return false;
	}

	// We are ready to rock
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Destroy the device
/////////////////////////////////////////////////////////////////////////////
void FMVDriver::Destroy(void)
{
	// Doh, no fmvs on atwinmon
	if (MultiStream::UsingATWinMon())
		return;

	dbgPrint("restoring video mode\n");

	// Destroy the session
	if(session)
	{
		EndMpegSession(session);
		session = NULL;
	}

	// Restore the scratch pad
	RestoreScratchPad();

	// wait for a vblank
	Display__ClearGraphics();	//clear frame buffers so no backbuffer is visible as a "glitch"
	sceGsSyncV(0);

	// Restore the scratch pad
	RestoreVRAM();

	// switch video modes back...
	Display__ResetGraphics();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool FMVDriver::Play(
const char *fmv,
FMVSoundTrack soundTrack,
bool &aborted,
AbortFn abort,
void *abortContext)
{
	// Doh, no fmvs on atwinmon
	if (MultiStream::UsingATWinMon())
		return false;

	ASSERT(session);

	// Convert a native filename
	char pathFile[128];
	MakeFile(fmv, pathFile);
	char *nativeFileName = Media::GetFileName(pathFile);

	// Play it
	PlayMpegFile(session, nativeFileName, soundTrack, aborted, abort, abortContext);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// TJC - store off the vram that we touch (except the front/back/z buffers)
/////////////////////////////////////////////////////////////////////////////
bool FMVDriver::SaveVRAM()
{
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// TJC - restore the vram that we saved
/////////////////////////////////////////////////////////////////////////////
void FMVDriver::RestoreVRAM()
{
	PlatformImage::FlushTextureMemory();
}

/////////////////////////////////////////////////////////////////////////////
// The scratch pad we are going to save from certain death
/////////////////////////////////////////////////////////////////////////////
bool FMVDriver::SaveScratchPad()
{
	ASSERTS(scratchPadSave == NULL, "Scratch pad save called out of order.");
	scratchPadSave = memAlloc(SCRATCHPAD_SIZE, 64);
	if (!scratchPadSave)
		return false;

	// Copy it in!
	memcpy(scratchPadSave, RENDERER_SPAD_ADDR, SCRATCHPAD_SIZE);
}

/////////////////////////////////////////////////////////////////////////////
// TJC - restore the dumb thing
/////////////////////////////////////////////////////////////////////////////
void FMVDriver::RestoreScratchPad()
{
	// Duh
	ASSERTS_PTR(scratchPadSave, "Scratch pad restore called out of order.");
	if (!scratchPadSave)
		return;

	// Copy it back
	memcpy(RENDERER_SPAD_ADDR, scratchPadSave, SCRATCHPAD_SIZE);

	// Free the block
	memFree(scratchPadSave);
	scratchPadSave = NULL;
}

