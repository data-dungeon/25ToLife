///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifndef SBANKXB_H
#define SBANKXB_H

#include "Audio/soundbank.h"

class SoundBankXbox : public SoundBank
{
public:
	// The sound bank must have a sound driver
	SoundBankXbox(SoundDriver &soundDriver);

 	// Destroying the bank will let go of our sounds
	// NEVER CALL THIS DIRECTLY!
	~SoundBankXbox();

	// Get at the relevant info
	bool GetSoundCreateInfo(void *forSound, WAVEFORMATEX *&wfx,
									void *&bankData, unsigned int &bankSize,
									unsigned int &playOffset, unsigned int &playLength);

private:
	// We assume inorder driver creation...this could mess up if someone
	// starts messing with the formula
	int	m_onCreateSound;

	// This lookup table gives the offset/lengths for each sound
	struct PlayInfo
	{
		unsigned int offset;
		unsigned int length;
		unsigned int dataLength;
	};
	PlayInfo		*m_playInfo;

	// We have a hunk of data here that is the whole bank
	// with everything aligned
	void	*m_bankData;
	int	m_bankSize;
	int	m_baseOffset;
	bool BuildBankData();

	// Take this number and return the number of bytes needed to round it up to
	// the nearest acceptable boundry
	static const int m_offsetAlign;
	static const int m_lengthAlign;
	static const int m_lengthPad;
	static unsigned int AlignOffset(unsigned int i)
		{ return Align(i, m_offsetAlign); }
	static unsigned int AlignLength(unsigned int i)
		{ return Align(i, m_lengthAlign); }
	static unsigned int Align(unsigned int i, unsigned int align);
};

#endif
