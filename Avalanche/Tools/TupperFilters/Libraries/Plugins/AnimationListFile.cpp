/**********************************************/
/*                                            */
/* InfoFile.cpp                               */
/* big juju prototype  12/14/01               */
/* ned martin  avalanche software             */
/* visual c++ 6.0                             */
/* DBL database exporter-- multigen plug-in   */
/*                                            */
/**********************************************/

/* for ImportExport-- handle an info .lst file associated with something */

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include "PluginsPch.h"
#include "AnimationListFile.h"
#include "PluginError.h"

static const char NEWLINE = 10;
static const char CARRIAGE_RETURN = 13;

static const int MAX_TOKEN_LENGTH = 200;

AnimationListFile::AnimationListFile(void)
{
	memset(this, 0, sizeof(AnimationListFile));
}

AnimationListFile::~AnimationListFile()
{
	if (m_pcFileBuffer)
	{
		delete m_pcFileBuffer; 
		m_pcFileBuffer = NULL; 
	}
}

void AnimationListFile::Lowercase(void)
{
	if (m_pcFileBuffer)
		strlwr(m_pcFileBuffer); 
}	// lowercases to ending null termintator

bool AnimationListFile::Exists(const char *pcFileName)
{
	FILE *pFile = fopen(pcFileName, "rt");
	if (pFile == NULL)
		return(false);
	fclose(pFile);
	return(true);
}

/* load contents of passed info file */

void AnimationListFile::Load(const char *pcFileName)
{
	/* record name */

	strcpy(m_acFileName, pcFileName);

	/* note what file we are processing for possible errors, and try to open it */

	FILE *pFile = fopen(m_acFileName, "rb");
	if (pFile == NULL)
	{
		throw(PluginException(ERROR_FATAL,"Cannot open animation list file %s.",m_acFileName));
	}
	/* read it in */

	fseek(pFile, 0, SEEK_END);	// to end of file
	m_nLength = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);	// back to beginning
	m_pcFileBuffer = new char[m_nLength + 1];	// + 1 to append null terminator
	if (m_pcFileBuffer == NULL)
	{
		fclose(pFile);
		throw(PluginException(ERROR_FATAL,"Cannot allocate space for animation list file %s.",m_acFileName));
	}
	fread(m_pcFileBuffer, m_nLength, 1, pFile);
	fclose(pFile);
	m_pcFileBuffer[m_nLength] = 0;	// make sure it's null-terminated
}


/* look through file for a token */
bool AnimationListFile::FindToken(const char *pcStart,const char *pcToken)
{
	assert((pcStart >= m_pcFileBuffer) && (pcStart <= m_pcFileBuffer + m_nLength));	// must be within buffer!
	const char *pcSearch = pcStart;
	while (*pcSearch)
	{
		SkipWhiteSpace(&pcSearch);
		if (IsToken(&pcSearch, pcToken))
		{
			/* record current token */
			strcpy(m_acCurrentToken, pcToken);

			/* record position of token value-- IsToken skips white space after token */

			m_pcCurrentTokenValue = pcSearch;

			/* record place to start next search, if requested */

			SkipLine(&pcSearch);
			m_pcCurrentSearch = pcSearch;

			/* success */

			return(true);
		}

		/* not on this line, try the next */

		SkipLine(&pcSearch);
	}

	/* not found */

	strcpy(m_acCurrentToken, "");
	m_pcCurrentTokenValue = NULL;
	m_pcCurrentSearch = NULL;
	return(false);
}

/* find the first occurrence of the token in the file */

bool AnimationListFile::FindFirstToken(const char *pcToken)
{
	assert(strlen(pcToken));
	return(FindToken(m_pcFileBuffer, pcToken));
}

/* find the next occurrence of the token */

bool AnimationListFile::FindNextToken(void)
{
	/* only if FindFirstToken was called successfully */

	if (m_pcCurrentSearch == NULL)
		return(false);
	assert(strlen(m_acCurrentToken));
	return(FindToken(m_pcCurrentSearch, m_acCurrentToken));
}

/* return number of times token appears */

int AnimationListFile::GetTokenCount(const char *pcToken)
{
	if (FindFirstToken(pcToken) == false)
		return(0);
	int nCount = 1;
	while (FindNextToken())
		nCount++;
	return(nCount);
}

/* return integer value of last token found */

int AnimationListFile::GetTokenIntValue(void)
{
	if (m_pcCurrentTokenValue == NULL)
		return(-1);
	return(atoi(m_pcCurrentTokenValue));
}

/* return string value of last token found */

const char *AnimationListFile::GetTokenStringValue(void)
{
	if (m_pcCurrentTokenValue == NULL)
		return(NULL);

	/* return null-terminated string */
	static char acTokenValue[MAX_TOKEN_LENGTH];	// static because pointer to it gets returned
	int i = 0;
	while ((i < MAX_TOKEN_LENGTH) && 
			 (m_pcCurrentTokenValue[i] != 0) &&
			 (m_pcCurrentTokenValue[i] != ';') &&
			 (m_pcCurrentTokenValue[i] != NEWLINE) &&
			 (m_pcCurrentTokenValue[i] != CARRIAGE_RETURN))
	{
		acTokenValue[i] = m_pcCurrentTokenValue[i];
		i++;
	}
	acTokenValue[i] = 0;

	/* now chew white space off end of string */

	i = strlen(acTokenValue);
	while ((i > 0) && (acTokenValue[i - 1] <= ' '))
		i--;
	acTokenValue[i] = 0;

	/* return the string */

	return(acTokenValue);
}

void AnimationListFile::SkipWhiteSpace(const char **ppText)
{
	while ((**ppText <= ' ') && (**ppText != 0))
		(*ppText)++;
}

/* skip to beginning of next line */
void AnimationListFile::SkipLine(const char **ppText)
{
	while ((**ppText != NEWLINE) && (**ppText != CARRIAGE_RETURN) && (**ppText != 0))
		(*ppText)++;
	SkipWhiteSpace(ppText);
}

bool AnimationListFile::IsToken(const char **ppText,const char *pToken)
{
	if (strncmp(*ppText, pToken, strlen(pToken)) == 0)
	{
		*ppText += strlen(pToken);
		SkipWhiteSpace(ppText);
		return(true);
	}
	return(false);
}
