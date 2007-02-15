/***************************************************************************/
// Simple file parser
/***************************************************************************/
#include "EngineHelper/EngineHelperPCH.h"

#include "EngineHelper/filestream.h"
#include "EngineHelper/sfparse.h"

/***************************************************************************/
/***************************************************************************/
SimpleFileParser::SimpleFileParser(const char *fileName)
{
	// Read in the file
	m_fileSize = 0;
	m_file = (char *)FileStream::OpenReadClose(fileName, &m_fileSize);
	m_callFree = true;

	// Make sure we NULL terminate this baby
	if (m_file && m_fileSize >= 0)
		m_file[m_fileSize - 1] = '\0';

	// Start the parse
	m_parse = m_file;
}

/***************************************************************************/
/***************************************************************************/
SimpleFileParser::SimpleFileParser(const char *data, int dataSize)
{
	// Read in the file
	m_fileSize = dataSize;
	m_file = (char *)data;
	m_callFree = false;

	// Make sure we NULL terminate this baby
	if (m_file && m_fileSize >= 0)
		m_file[m_fileSize - 1] = '\0';

	// Start the parse
	m_parse = m_file;
}

/***************************************************************************/
/***************************************************************************/
SimpleFileParser::~SimpleFileParser()
{
	if (m_callFree)
		memFree(m_file);
}

/***************************************************************************/
// Count the sections (doesn't not disturb parsing)
/***************************************************************************/
int SimpleFileParser::CountSections()
{
	int sections = 0;
	const char *parse = m_file;
	while (parse && *parse)
	{
		parse = SkipSpaces(parse);
		if (*parse == '[')
			++sections;
		parse = SkipEOL(parse);
	}

	return sections;
}

/***************************************************************************/
/***************************************************************************/
const char *SimpleFileParser::GetNextToken(
TokenType &type,
char *token,
int tokenLen)
{
	// Assume the woest
	ASSERT(tokenLen > 1);

	while (m_parse)
	{
		// Skip spaces
		m_parse = SkipSpaces(m_parse);
		if (*m_parse == '\0')
			break;
		else if (*m_parse == '#')
			m_parse = SkipEOL(m_parse);
		else if (*m_parse == '[')
		{
			type = TOKEN_SECTION;
			m_parse = ParseToken(m_parse + 1, token, tokenLen, ']');
			m_parse = SkipEOL(m_parse);
			return NULL;
		}
		else if (FindEquate(m_parse))
		{
			type = TOKEN_EQUATE;
			const char *restOfLine = ParseToken(m_parse, token, tokenLen, '=');
			m_parse = SkipEOL(restOfLine);
			return restOfLine;
		}
		else
		{
			type = TOKEN_SINGLE;
			m_parse = ParseToken(m_parse, token, tokenLen);
			m_parse = SkipEOL(m_parse);
			return NULL;
		}
	}

	// Nothing to see here
	type = TOKEN_EOF;
	*token = '\0';
	return NULL;
}

/***************************************************************************/
// If you get an equate token, this will help you pick apart the data
// This returns the pointer for the rest of line beyond the parsed token
/***************************************************************************/
const char *SimpleFileParser::ParseRestOfLine(
const char *restOfLine,
char *value,
int valueSize,
char separator)
{
	// Pick off the next value
	restOfLine = ParseToken(restOfLine, value, valueSize, separator);

	// Are we done?
	if (*restOfLine == '\0' || *restOfLine == '\n' || *restOfLine == '\r')
		return NULL;

	// not yet
	return restOfLine;
}

/***************************************************************************/
/***************************************************************************/
int SimpleFileParser::RestOfLineTokens(
const char *restOfLine,
char separator)
{
	int tokens = 0;
	const char *parse = restOfLine;
	while (*parse)
	{
		char dummyToken[8];
		parse = ParseToken(parse, dummyToken, sizeof(dummyToken), separator);
		if (dummyToken[0])
			++tokens;
	}

	return tokens;
}

/***************************************************************************/
// Utilities
/***************************************************************************/
const char *SimpleFileParser::SkipSpaces(
const char *parse)
{
	while (*parse == ' ' || *parse == '\t')
		++parse;

	return parse;
}

/***************************************************************************/
/***************************************************************************/
const char *SimpleFileParser::SkipEOL(
const char *parse)
{
	// Are we there yet?
	while (*parse && *parse != '\n' && *parse != '\r')
		++parse;

	// Now skip them
	while (*parse == '\n' || *parse == '\r')
		++parse;

	return parse;
}

/***************************************************************************/
/***************************************************************************/
const char *SimpleFileParser::ParseToken(
const char *parse,
char *token,
int tokenLen,
char separator)
{
	// Sart out skipping spaces
	parse = SkipSpaces(parse);

	int len = 0;
	while (*parse && *parse != '\n' && *parse != '\r' && *parse != separator)
	{
		if (len < tokenLen)
			token[len++] = *parse;
		++parse;
	}

	// Remove trailing blanks
	while (len > 0 && (token[len - 1] == ' ' || token[len - 1] == '\t'))
		--len;

	// terminate the token
	token[len] = '\0';

	// Skip the separator (if we hit it)
	if (*parse && *parse == separator)
		++parse;

	// next
	return parse;
}

/***************************************************************************/
/***************************************************************************/
bool SimpleFileParser::FindEquate(
const char *parse)
{
	while (*parse && *parse != '\n' && *parse != '\r')
	{
		if (*parse == '=')
			return true;
		++parse;
	}

	return false;
}
