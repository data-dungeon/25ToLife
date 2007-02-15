///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifndef SBANKXB_H
#define SBANKXB_H

#include "Audio/soundbank.h"

class SoundBankPS2 : public SoundBank
{
public:
	// The sound bank must have a sound driver
	SoundBankPS2(SoundDriver &soundDriver);

 	// Destroying the bank will let go of our sounds
	// NEVER CALL THIS DIRECTLY!
	~SoundBankPS2();

	// Load the bank to the SPU
	bool LoadSPUData(bool allowSPUAlloc = true);

	// What is the base address in spu RAM (0 means ERROR)
	uint GetBaseSPUAddress()
		{ return m_spuBaseAddr; }
	uint GetSPUSize();

	// Get the SPU Address for a sound
	uint GetSoundSPUAddress(void *data, int size);

private:
	// We assume inorder driver creation...this could mess up if someone
	// starts messing with the formula
	int	m_onCreateSound;

	// Our address in SPU ram
	uint	m_spuBaseAddr;
};

#endif
