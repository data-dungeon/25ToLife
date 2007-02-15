/******************* includes ****************************/

#include "Layers/LayersPCH.h"

#include "Layers/Assert/parseelf.h"


#define MAX_SHENTSIZE 300

#define SEC_LOAD 0x002

ElfParser::ElfParser()
{
#ifdef PS2
	mHandle = 0xFFFFFFFF;
#else
	// need platform specific version (please dont use media:: since it might assert on us)
	LOCKUP()
#endif
}

ElfParser::~ElfParser()
{
#ifdef PS2
	if(mHandle != 0xFFFFFFFF)
		sceClose(mHandle);
#else
	// need platform specific version (please dont use media:: since it might assert on us)
	LOCKUP()
#endif
	mHandle = 0xFFFFFFFF;
}

bool ElfParser::Open(const char* filename)
{
	// open the file
	t_Error error;

#ifdef PS2

	// try it literally first...
	mHandle = sceOpen(filename, SCE_RDONLY);

	if(mHandle == 0xFFFFFFFF)
	{
		// ok, now lets try it mangled
		mHandle = sceOpen(Media::GetFileName(filename), SCE_RDONLY);
		if(mHandle == 0xFFFFFFFF) 
			return false;
	}

	// clear the header so we know the read worked...
	memset(&mHeader, 0, sizeof(mHeader));

	// read the header...
	if(sceRead(mHandle, &mHeader, sizeof(mHeader)) < 0)
		return false;

	// verify its an elf...
	if(mHeader.fileID[0] != 0x7f ||
		mHeader.fileID[1] != 0x45 ||
		mHeader.fileID[2] != 0x4c ||
		mHeader.fileID[3] != 0x46 ||
		mHeader.fileID[4] != 0x01 ||
		mHeader.fileID[5] != 0x01 ||
		mHeader.fileID[6] != 0x01 ||
		mHeader.fileID[7] != 0x00 ||
		mHeader.fileID[8] != 0x00 ||
		mHeader.fileID[9] != 0x00 ||
		mHeader.fileID[10] != 0x00 ||
		mHeader.fileID[11] != 0x00 ||
		mHeader.fileID[12] != 0x00 ||
		mHeader.fileID[13] != 0x00 ||
		mHeader.fileID[14] != 0x00 ||
		mHeader.fileID[15] != 0x00)
	{
		return false;
	}

	ASSERT(sizeof(SectionHeader) <= mHeader.shEntSize);

	// go the strings table...
	if(sceLseek(mHandle, mHeader.shOff + mHeader.shEntSize * mHeader.shStrNdx, SCE_SEEK_SET) < 0)
		return false;

	// get it..
	if(sceRead(mHandle,&mStringSectionHeader, Math::Min(mHeader.shEntSize, (u16)(sizeof(SectionHeader)))) < 0)
		return false;
#else
	// need platform specific version (please dont use media:: since it might assert on us)
	LOCKUP();

#endif

	return true;
}

bool ElfParser::LoadSectionHeader(const char* section)
{
	char stringBuffer[200];
	t_Error error;

#ifdef PS2
	if(mHandle == 0xFFFFFFFF) return false;
#else
		// need platform specific version (please dont use media:: since it might assert on us)
		LOCKUP();
#endif

	for(int i = 0; i < mHeader.shNum; i++)
	{
#ifdef PS2
		// go to the section headers...
		if(sceLseek(mHandle,mHeader.shOff + mHeader.shEntSize * i, SCE_SEEK_SET) < 0)
			return false;

		// read in the entry
		if(sceRead(mHandle, &mSectionHeader, Math::Min(mHeader.shEntSize, (u16)(sizeof(SectionHeader)))) < 0)
			return false;
#else
		// need platform specific version (please dont use media:: since it might assert on us)
		LOCKUP();
#endif

		// get section name... (was it unnamed?)
		if(!ReadCString(mStringSectionHeader.offset, mSectionHeader.name, stringBuffer, sizeof(stringBuffer)))
			continue;

		// see if its our entry...
		if(strcmp(stringBuffer, section) == 0)
			return true;
	}

	return false;
}

// return true if current section has load flag.
bool ElfParser::IsSectionLoadable()
{
	if(mSectionHeader.flags & SEC_LOAD)
		return true;
	return false;
}

bool ElfParser::IsInSection(int offsetInSection)
{
	offsetInSection -= mSectionHeader.addr;
	if(offsetInSection >= 0 && offsetInSection < mSectionHeader.size)
		return true;
	return false;
}

int ElfParser::ReadCString(int sectionStart, int offsetInSection, char* buffer, int bufferSize)
{
	t_Error error;

#ifdef PS2
	// get the string...
	if(sceLseek(mHandle, sectionStart + offsetInSection, SCE_SEEK_SET) < 0)
		return false;
	
	if(sceRead(mHandle, buffer, bufferSize) < 0)
		return false;
#else
	// need platform specific version (please dont use media:: since it might assert on us)
	LOCKUP();
#endif

	buffer[bufferSize-1] = '\0';

	// I havent figured out why we need this.  sceRead should handle this for us!
	FlushCache(0);

	return strlen(buffer);
}

int ElfParser::ReadCString(const char* offsetInSection, char* buffer, int bufferSize)
{
	return ReadCString(mSectionHeader.offset, (int)offsetInSection - mSectionHeader.addr, buffer, bufferSize);
}
