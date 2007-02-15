////////////////////////////////////////////////////////////////////////////
//
// WriteTextureNames
//
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "ExportTexturesPch.h"
#include "ProcessTup.h"

u32 ProcessTup::WriteTextureNames(void)
{
	FILE *pDBLFile = DBLUtil::GetDBLFile();

	// warn if name outputted more than once
	Keeper<TupString> nameKeeper;
	int numTextures = m_textureEntries.GetSize();
	for (int textureIndex = 0; textureIndex < numTextures; textureIndex++)
	{
		TextureEntry &textureEntry = m_textureEntries[textureIndex];
		int oldSize = nameKeeper.GetSize();
		int nameIndex = nameKeeper.Add(textureEntry.m_name);
		// if name was already added
		if (oldSize!=nameIndex)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Warning Name %s used for more than one texture",(const char *)textureEntry.m_name);
		}
	}

	DBLUtil::StartChunk(DB_TEXTURE_NAMES, DB_TEXTURE_NAMES_VERSION, DBL_CHUNK_NO_IGNORE, "1000");
	DBLUtil::StartRecordList(numTextures, RECORD_LIST_HAS_OFFSET_TABLE, 0);

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Texture Names");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Index  Name                              FilePath");

	for (int textureIndex = 0; textureIndex < numTextures; textureIndex++)
	{
		TextureEntry &textureEntry = m_textureEntries[textureIndex];

		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"%4d  %-30s  %s",textureIndex,(const char *)textureEntry.m_name,(const char *)textureEntry.m_filePathName);

		DBLUtil::RecordListEntry();

		DBLWrite::WriteRecord(&textureIndex,pDBLFile);
		DBLWrite::WriteData((const char *)textureEntry.m_name,textureEntry.m_name.GetLength()+1,pDBLFile);
		DBLUtil::AlignFile();
	}
	DBLUtil::FinishRecordList();
	return DBLUtil::FinishChunk(FALSE);
}

