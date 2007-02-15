/***************************************************************************/
// GCN hardware soundbank implementation
/***************************************************************************/
#ifndef SBANKGCN_H
#define SBANKGCN_H

// We subclass the sound bank
#include "Audio/soundbank.h"

// Our owner
class SoundDriverGCN;

class SoundBankGCN : public SoundBank
{
public:
	~SoundBankGCN();

	// Was is our base address
	unsigned int BaseAddress()
		{ return base; }

private:
	// The sound bank must have a sound driver
	SoundBankGCN(SoundDriverGCN &soundDriver);

	// Our heap parameters
	u32			 	base;

	friend class SoundDriverGCN;
};

#endif
















