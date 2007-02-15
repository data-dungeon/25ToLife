//
// This is a (hopefully) generic ELF file parser used for grabbing one
// string out of a section of strings
//
// This works with ELF version 
//

#ifndef __PARSEELF_H__
#define __PARSEELF_H__

class ElfParser
{
public:
	ElfParser();
	~ElfParser();

	bool Open(const char* filename);

	// returns true if it found the header...
	bool LoadSectionHeader(const char* section);

	// return true if current section has load flag.
	bool IsSectionLoadable();

	// does the passed offset represent a place in this section.
	bool IsInSection(int offsetInSection);

	// reads a CString into the outputBuffer from the currently loaded section
	// if the CString is longer than maxLength, only maxLength-1 data is copied to the outputBuffer
	// and the buffer is null terminated.
	// function returns the length of the string up to (maxLength-1)
	int ReadCString(const char* stringOffset, char* outputBuffer, int maxLength);

protected:
	u32 mHandle;

	struct Header
	{
		u8 fileID[16];		// file ID
		u16 type;			// object file type
		u16 machine;		// target architecture
		u32 version;		// ELF version
		u32 entry;			// starting virtual address
		u32 phOff;			// file offset to program header
		u32 shOff;			// file offset to section header
		u32 flags;			// flags
		u16 ehSize;			// ELF header size
		u16 phEntSize;		// program header entry size
		u16 phNum;			// program header entry number
		u16 shEntSize;		// section header entry size
		u16 shNum;			// section header entry number
		u16 shStrNdx;		// section header index for strings
	} mHeader;

	struct SectionHeader
	{
		u32 name;			// name of section
		u32 type;			// type of section
		u32 flags;			// section specific attributes
		u32 addr;			// memory location of section
		u32 offset;			// file offset to section
		u32 size;			// size of section
		u32 link;			// section type dependant
		u32 info;			// extra information
		u32 addralign;		// address alignment constraint
		u32 entSize;		// size of an entry in a section
	} mSectionHeader, mStringSectionHeader;

	int ReadCString(int sectionstart, int offset, char* outputBuffer, int maxLength);
};

	
	
#endif // __PARSEELF_H__
