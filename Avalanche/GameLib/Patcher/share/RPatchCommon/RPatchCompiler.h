#pragma once

#include <list>
#include <string>

class RPatchBuilder;

// PatchHeader Class  -- This is the header that goes at the top
// of the compiled patch file.
class RPatchHeader 
{
public:
	RPatchHeader()
		: compressedLength(0)
		, uncompressedLength(0)
		, patchOffset(0)
		, patchedSize(0)
		, originalSize(0)
		, fileNameLength(0)
	{
		memset(originalSig, 0, 16);
		memset(modifiedSig, 0, 16);
		memset(filename, 0, 256);
	}

	int				compressedLength;	// Size in file of compressed patch
	int				uncompressedLength;	// Size after decompressing data in patch
	int				patchOffset;		// Offset of this patch from beginning of big file
	int				patchedSize;		// Size of patched modified file
	unsigned char	fileNameLength;		// sic
	char			filename[ 256 ];	// sic
	int				originalSize;		// Size of original file
	unsigned char	originalSig[16];	// Signature of original file
	unsigned char	modifiedSig[16];	// Signature of patched, modified file
};

// The RPatchDescriptor class just contains a header (RPatchHeader) and a 
// builder. It is a convenience to avoid all the extra file writing the patcher
// held originally, without having to rewrite everything.
class RPatchDescriptor {
public:
	RPatchDescriptor(RPatchBuilder* builder);
	~RPatchDescriptor();

	RPatchBuilder*	GetBuilder(void) const {return m_builder;}
	int				NameLength(void) const {return (int)m_header.fileNameLength;}

	void			SetBaseName(const std::string& baseName);
	void			SetFinalSize(int size);
	void			SetOffset(int offset);
	void			SetOriginalSize(int size);
	void			SetOriginalSig(unsigned char* sig);
	void			SetModifiedSig(unsigned char* sig);
	bool			WritePatch(FILE* f);
	bool			WriteHeader(FILE* f);
	int				HeaderSize(void) const;
private:
	RPatchBuilder*		m_builder;
	RPatchHeader		m_header;
};

// RPatchCompiler Class -- This is class responsible for compiling
// many patch files into the final patch.
class RPatchCompiler
{
private:
	std::list<std::string> _path1Files;
	std::list<std::string> _path2Files;
	std::list<RPatchDescriptor*> _descriptors;

	std::string _basePath1;
	std::string _basePath2;

	void _ClearDescriptors(void);
	std::string FindMatchingFile(const std::string& path1File, std::list<std::string>& modFileList);
	bool CreatePatchFile(const std::string& file, const std::string fileMatch);
	int CalculateHeaderSize(void);
	bool PatchDataToBigPatch(FILE *bigPatch);
	bool WriteBigPatchHeader(FILE *bigPatch, unsigned int version);
	bool CompileToBigPatch(const std::string& patchName, unsigned int version);
	void CalculateMD5(std::string filename, unsigned char *signature);
	void CalculateBigMD5(FILE* bigPatch, int startOffset, unsigned char *signature);

public:
	RPatchCompiler() {}
	RPatchCompiler(const std::string& basePath1, const std::string& basePath2);
	~RPatchCompiler();

	// Main function to compile the patch -- Takes two file lists, compares them, makes individual patch
	// files for each file, and the compiles them into a single patch.
	bool CompilePatch(std::list<std::string>& filesToPatch, const std::string& patchName, unsigned int version);
	void GetDifferenceList(std::list<std::string>& sourceFileList, std::list<std::string>& modFileList, std::list<std::string>& differenceList);
	void SetPaths(const std::string& basePath1, const std::string& basePath2);

	// Used internally, but also useful as a helper functions.
	bool GenerateFileList(std::list<std::string>& finalFileList, std::string pathname, bool recurseDirs = true);
	int GetFileSize(std::string filename);
};
