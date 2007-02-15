////////////////////////////////////////////////////////////////////////////
//
// AnimationListFile
//
// Class to handle parsing an animation list text file
// Taken from Ned's InfoFile class in ImportExport
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimationListFile.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/25/02    Time: 4:43p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * First phase for texture animation
 * Analysis tools, utilities and palette generator
 */

/* for ImportExport-- handle an info .lst file associated with something. info files generally consist of tokens
followed by values (can be strings), and semi-colons mark comments. here's an example:

; animated texture list file

VERSION 1	; must have a version number
FPS 5		; animation speed

; images

FILE c:\dev\juju\data\font\font00.pcx
FILE c:\dev\juju\data\font\font01.pcx

*/

#ifndef TUPPERFILTERS_LIB_PLUGINS_ANIMATION_LIST_FILE_H
#define TUPPERFILTERS_LIB_PLUGINS_ANIMATION_LIST_FILE_H

/* system includes */

class AnimationListFile
{
// Constructor/destructor

public:

	AnimationListFile(void);
	~AnimationListFile();

protected:

	char m_acFileName[_MAX_PATH];
	char *m_pcFileBuffer;				// malloc'd buffer to hold file contents
	char *m_pcSearch;					// for searching through file
	int	m_nLength;						// size of malloc'd buffer

	/* for searching file for tokens and values */

	char m_acCurrentToken[200];		// current token being searched for (set by call to FindFirstToken)
	const char *m_pcCurrentTokenValue;		// pointer to value of last token found
	const char *m_pcCurrentSearch;			// place to start next token search, if requested-- start of line following line that contained last token found

public:
	void Lowercase(void);
	bool Exists(const char *pcFileName);
	void Load(const char *pcFileName);
	bool FindFirstToken(const char *pcToken);
	bool FindNextToken();
	int GetTokenCount(const char *pcToken);
	int GetTokenIntValue();
	const char *GetTokenStringValue();
protected:
	bool FindToken(const char *pcStart,const char *pcToken);
	static void SkipWhiteSpace(const char **ppText);
	static void SkipLine(const char **ppText);
	static bool IsToken(const char **ppText, const char *pToken);
};

#endif

