/***************************************************************************/
// Simple file parser
//
// Files must end with at least a single non-relevant character (new line,
// blank, or comment)
/***************************************************************************/
#ifndef SFPARSER_H
#define SFPARSER_H

class SimpleFileParser
{
public:
	SimpleFileParser(const char *fileName);
	SimpleFileParser(const char *data, int dataSize);
	~SimpleFileParser();

	// Count the sections (doesn't not disturb parsing)
	int CountSections();

	// Get a token
	// Lines starting with '#' are ignored, as are blank lines.
	enum TokenType
	{
		TOKEN_SECTION, // "[token]" on a line (token will have [] stripped)
		TOKEN_EQUATE, 	// "token=string" on a line (Rest of line returned)
		TOKEN_SINGLE,	// "token" on a line
		TOKEN_EOF, 	// EOF done reading
	};
	// Note:: NULL return means there is no extra data...look for TOKEN_EOF
	// for end of file
	const char *GetNextToken(TokenType &type, char *token, int tokenLen);

	// If you get an equate token, this will help you pick apart the data
	// This returns the pointer for the rest of line beyond the parsed token
	static const char *ParseRestOfLine(const char *restOfLine, char *value, int valueSize, char separator = ',');
	static int RestOfLineTokens(const char *restOfLine, char separator = ',');

private:
	// The file
	char							*m_file;
	int							m_fileSize;
	bool							m_callFree;
	const char 					*m_parse;

	// Utilities
	int LineLength(const char *start);
	static const char *SkipSpaces(const char *parse);
	static const char *SkipEOL(const char *parse);

	static const char *ParseToken(const char *parse, char *token, int tokenLen, char separator = 0);
	static bool FindEquate(const char *parse);
};

#endif
