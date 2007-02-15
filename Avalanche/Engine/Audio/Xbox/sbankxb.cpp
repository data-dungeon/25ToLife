///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "Audio/AudioPCH.h"
#include "Audio/Xbox/sbankxb.h"

// Special (pain in the ass) alignments for Xbox
const int SoundBankXbox::m_offsetAlign = 36;
const int SoundBankXbox::m_lengthAlign = 36;
const int SoundBankXbox::m_lengthPad = 0;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SoundBankXbox::~SoundBankXbox()
{
	delete [] m_bankData;
	delete [] m_playInfo;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SoundBankXbox::SoundBankXbox(
SoundDriver &soundDriver) :
SoundBank(soundDriver)
{
	m_onCreateSound = 0;

	m_playInfo = NULL;

	m_bankData = NULL;
	m_bankSize = 0;
	m_baseOffset = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SoundBankXbox::GetSoundCreateInfo(
void *forSound,
WAVEFORMATEX *&wfx,
void *&bankData,
unsigned int &bankSize,
unsigned int &playOffset,
unsigned int &playLength)
{
	// This should happen once
	if (!m_bankData && !BuildBankData())
		return false;

	// Whoa!
	ASSERT(m_playInfo);
	ASSERT(m_onCreateSound < soundBankData->sourceCount);

	// Give them the info
	wfx = (WAVEFORMATEX *)forSound;
	bankData = (char *)m_bankData + m_baseOffset;
	bankSize = m_bankSize;
	playOffset = m_playInfo[m_onCreateSound].offset;
	playLength = m_playInfo[m_onCreateSound].length;

	// copy in the data!
	void *copyTo = (char *)bankData + playOffset;
	void *copyFrom = (char *)forSound + sizeof(WAVEFORMATEX);
	unsigned int copyLength = m_playInfo[m_onCreateSound].dataLength;
	ASSERT((unsigned int)copyTo + copyLength < (unsigned int)m_bankData + m_bankSize + m_offsetAlign);
	memcpy(copyTo, copyFrom, copyLength);

	// next!
	++m_onCreateSound;
	if (m_onCreateSound == soundBankData->sourceCount)
	{
		delete [] m_playInfo;
		m_playInfo = NULL;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SoundBankXbox::BuildBankData()
{
	// We should only do this on entry
	ASSERT(m_onCreateSound == 0);
	ASSERT(m_bankData == NULL);
	ASSERT(m_bankSize == 0);

	// Create the playinfo array
	m_playInfo = new PlayInfo[soundBankData->sourceCount];
	ASSERT(m_playInfo);
	if (!m_playInfo)
		return false;

	// Make the big fat data chunk
	for (int i = 0; i < soundBankData->sourceCount; i++)
	{
		// Setup the playinfo
		unsigned int offsetPad = AlignOffset(m_bankSize);
		m_playInfo[i].offset = m_bankSize;

		// Length
		m_playInfo[i].dataLength = soundBankData->sourceTable[i].length - sizeof(WAVEFORMATEX);
		unsigned int length = m_playInfo[i].dataLength + m_lengthPad;
		m_playInfo[i].length = length + AlignLength(length);

		// Total size
		m_bankSize += m_playInfo[i].length + offsetPad;
	}

	// Now create the chunk
	unsigned int trueSize = m_bankSize + m_offsetAlign;
	m_bankData = (void *)new char[trueSize];
	ASSERT(m_bankData);
	if (!m_bankData)
		return false;

	// Do I really need to do this?
	memset(m_bankData, '\0', trueSize);

	// What is the base offset?
	m_baseOffset = AlignOffset((unsigned int)m_bankData);

	// This worked, phew!
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned int SoundBankXbox::Align(
unsigned int i, 
unsigned int align)
{
	if (align <= 1)
		return 0;

	return (align - (i  % align)) % align;
}

