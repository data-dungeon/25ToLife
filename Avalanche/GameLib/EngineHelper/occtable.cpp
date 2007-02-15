/***************************************************************************/
// A simple jukebox interface for playing streaming sounds.  This API should
// improve as time goes on.
/***************************************************************************/
#include "EngineHelper/EngineHelperPCH.h"

#include "EngineHelper/filestream.h"
#include "Stage/SurfaceType.h"
#include "EngineHelper/occtable.h"

/***************************************************************************/
/***************************************************************************/
SoundOcclusionTable::SoundOcclusionTable()
{
	Clear();
}

/***************************************************************************/
// Clear it
/***************************************************************************/
void SoundOcclusionTable::Clear()
{
	for (int i = 0; i < 256; i++)
	{
		m_prop[i].volumeFactor = 1.0f;
		m_prop[i].panFactor = 1.0f;
	}
}

/***************************************************************************/
/***************************************************************************/
bool SoundOcclusionTable::Load(
const char *pathFile)
{
	// Get rid of the old one
	Clear();

	// Load the new one
	int size;
	char *file = (char *)FileStream::OpenReadClose(pathFile, &size);
	if (!file || size == 0)
		return false;

	// Parse!
	char *parse = file;
	int len = size;
	while (*parse && len > 0)
	{
		// Comment line?
		parse = SkipSpaces(parse, len);
		if (*parse == '#' || *parse == '\r' || *parse == '\n')
		{
			parse = SkipTo(parse, len, '\n');
			continue;
		}

		// Read the data
		char *materialName = parse; parse = SkipTo(parse, len, '=');
		parse = SkipSpaces(parse, len);
		char *volumeString = parse; parse = SkipTo(parse, len, ';');
		parse = SkipSpaces(parse, len);
		char *panString = parse; parse = SkipTo(parse, len, '\n');

		// Find the material
		int material = g_surfaceType.GetIndex(materialName);
		ASSERT(material >= 0);
		if (material < 0)
			continue;

		if (*volumeString)
			m_prop[material].volumeFactor = (float)atof(volumeString) * 0.01f;

		if (*panString)
			m_prop[material].panFactor = (float)atof(panString) * 0.01f;

		TRACE("Setting occlusion for \"%s\" (%d) to v = %d, p = %d\n", materialName, material, (int)(m_prop[material].volumeFactor * 100.0f), (int)(m_prop[material].panFactor * 100.0f));
	}
	memFree(file);

	return true;
}

/***************************************************************************/
// Utilities
/***************************************************************************/
char *SoundOcclusionTable::SkipSpaces(
char *parse,
int &len)
{
	while (len > 0 && (*parse == ' ' || *parse == '\t'))
	{
		++parse;
		--len;
	}

	return parse;
}

/***************************************************************************/
/***************************************************************************/
char *SoundOcclusionTable::SkipTo(
char *parse,
int &len,
char separator)
{
	// Go past spaces
	parse = SkipSpaces(parse, len);

	while (len > 0 && (*parse != separator && *parse != '\r' && *parse != '\n'))
	{
		++parse;
		--len;
	}

	if (len > 0 && *parse == separator)
	{
		*parse++ = '\0';
		--len;
	}

	if (len > 0 && *parse == '\r')
	{
		*parse++ = '\0';
		--len;
	}

	return parse;
}

