//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//
// XPRess:
//
// This tool takes a .DBL file and parses the textures
// to create an .XPR file for the xbox. 
//
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

#ifndef XPRESS_H
#define XPRESS_H

// prototypes
int InitBundler(int argc, char* argv[]);

#include <graphics.h>

class CVertexBuffer;

struct XPRess_MapAttrib
{
	int	MipLevels;
	int	CompressionType;
	char*	FilterType;
	char*	TextureFormat;
	bool	TwoSidedFaces;
	char	Fname[256];
};

// class
class XPRess
{
public:
	XPRess();
	~XPRess();

	// dbl handling
	void	LoadDBL();
	void	SaveDBL();

	// grafx conversion
	BOOL  WriteBMPToMemory( BITMAP2 *pbm );
	BOOL	WriteTGAToMemory( BITMAP2 *pbm );

	void	ProcessTextures();
	int	InitBundler(int argc, char* argv[]);
	int	RunBundler(int NumTextures);

	// key handling
	bool	LoadKeys();

public:

	bool	m_NoTextures;
	int	m_NumTextures;
	char	m_DibbleName[256];
	char	m_DibbleOutputName[256];
	char	m_FormatType[256];
	bool	m_OverrideOutputName;
	BYTE	*m_pBuffer;
	int   m_BufferSize;
	bool	m_WriteBMPs;
	bool	m_TextureFormat;
	bool	m_Compress;
	bool	m_UseMapAttrib;
	bool	m_RequestUseMapAttrib;
	bool	m_RunClean;
	bool	m_PS2Saturation;
	bool	m_TextureList;
};

extern	XPRess	XPRessObj;
extern	XPRess_MapAttrib	MapAttrib[4096];		// max 4096 textures per dbl

#endif	// XPRESS_H
