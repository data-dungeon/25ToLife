///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "Audio/AudioPCH.h"

#include "layers/mstream/mstream.h"
#include "Audio/PS2/sbankps2.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SoundBankPS2::~SoundBankPS2()
{
	// Release what we have in SPU ram
	MultiStream::FreeSRAM(m_spuBaseAddr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SoundBankPS2::SoundBankPS2(
SoundDriver &soundDriver) :
SoundBank(soundDriver)
{
	m_onCreateSound = 0;
	m_spuBaseAddr = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SoundBankPS2::LoadSPUData(
bool allowSPUAlloc)
{
	// Do we have a block yet?
	uint length = GetSPUSize();
	if (!m_spuBaseAddr && allowSPUAlloc)
		m_spuBaseAddr = MultiStream::AllocateSRAM(length);
	if (!m_spuBaseAddr)
		return false;

	// Stream it in!
	char spuFile[32];
	sprintf(spuFile, "%s.spu", soundBankData->externalFile);

	// Find the bank info
	SoundDriverPS2 &ps2Driver = (SoundDriverPS2 &)driver;
	const BankLibraryEntry *bankEntry = ps2Driver.bankLibrary.Find(spuFile);
	ASSERT(bankEntry);
	if (!bankEntry)
		return false;

	// Get it from the bank!
	PS2FileStream *file = MultiStream::CreateFile(bankEntry->fileID, ps2Driver.bankLibrary.Size(), false);
	if (!file)
		return false;

	// Do the real work
	file->Seek(bankEntry->offset);
	bool ret = file->Read((void *)m_spuBaseAddr, (int)length, PS2FileStream::FILEDEST_SPU);
	MultiStream::CloseFile(file);

	// Only the read knows if it worked
	ASSERT(ret);
	if (!ret)
	{
		MultiStream::FreeSRAM(m_spuBaseAddr);
		m_spuBaseAddr = 0;
	}

	// Get out
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
uint SoundBankPS2::GetSPUSize()
{
	// We have to be loaded to know our size
	ASSERT(soundBankData);
	if (!soundBankData)
		return 0;

	return (uint)soundBankData->externalFileSize;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
uint SoundBankPS2::GetSoundSPUAddress(
void *data,
int size)
{
	// We have to be loaded to know our size
	ASSERT(soundBankData);
	if (!soundBankData)
		return 0;

	ASSERT(soundBankData->sourceTable);
	if (!soundBankData->sourceTable)
		return 0;

	// First create sound?
	if (m_onCreateSound++ == 0)
		LoadSPUData();
	if (m_spuBaseAddr == 0)
		return 0;

	// Compute our offset into the SPU block
	uint offset = 0;
	const SourceDef *source = soundBankData->sourceTable;
	for (int s = 0; s < m_onCreateSound - 1; s++)
	{
		offset += (uint)source[s].length;
	}

	// We are are loaded into SPU give back the SPU address
	return m_spuBaseAddr + offset;
}

