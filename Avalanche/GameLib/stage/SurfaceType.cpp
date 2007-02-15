/*
**
**  File:   SurfaceType.cpp
**  Date:   February 22, 2001
**  By:     Bryant Collard
**  Desc:   A list of surface types with IDs.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/stage/SurfaceType.cpp $
**  $Revision: 14 $
**      $Date: 11/21/02 12:25p $
**    $Author: Dross $
**
*/

// Precompiled header
#include "stage/StagePCH.h"

#define TOKEN_LENGTH      256
#define NAME_TITLE        "SURFACE"
#define NAME_TITLE_LENGTH 7

DeclareSingleton(CSurfaceType);

CSurfaceType::CSurfaceType()
{
	d_entry = NULL;
	d_numDynamicEntries = 0;
	d_maxNameLength = NAME_TITLE_LENGTH;
}

CSurfaceType::~CSurfaceType()
{
	Clear();
}

void CSurfaceType::Clear(void)
{
	if (d_entry != NULL)
	{
		int i;
		for (i = 0; i < d_numEntries; i++)
			delete[] d_entry[i].d_name;
		delete[] d_entry;
		d_entry = NULL;
		d_numEntries = 0;
		d_numDynamicEntries = 0;
		d_maxNameLength = NAME_TITLE_LENGTH;
	}
}

void CSurfaceType::Load(t_Handle i_worldHandle, const char* i_file)
{
	if (d_entry != NULL)
	{
		conPuts("Attempting to load multiple surface type tables.\n");
		ASSERT(false);
		return;
	}
	if ((i_file == NULL) || (i_file[0] == 0))
	 {
		char message[128];
		sprintf(message, "Loading surface type table without file.\n");
		conPuts(message);
	 }

	int i;
	char token[TOKEN_LENGTH];
	int terrainCount;
	if (i_worldHandle != INVALID_HANDLE)
		terrainCount = GetTerrainCount(i_worldHandle);
	else
		terrainCount = 0;
	int numEntries = terrainCount;

	CTableToken tableToken(i_file);
	if (tableToken.IsFileOpen())
	{
		while (tableToken.GetToken(token, TOKEN_LENGTH) > 0)
		{
			ParseToken(token);
			for (i = 0; i < terrainCount; i++)
			{
				if (stricmp(token, GetTerrainName(i_worldHandle, i)) == 0)
					break;
			}
			if (i >= terrainCount)
				numEntries++;
		}
	}

	if (numEntries <= 0)
		return;

	d_entry = new SSurfaceEntry[numEntries];
	ASSERT(d_entry != NULL);
	if (d_entry == NULL)
		return;
	d_numEntries = 0;
	d_numDynamicEntries = 0;

	while (d_numEntries < terrainCount)
	{
		char* name = GetTerrainName(i_worldHandle, d_numEntries);
		if ((d_numEntries >= numEntries) ||
				!AppendEntry(name, strlen(name), true, KINETIC_UNSPECIFIED))
		{
			Clear();
			ASSERT(false);
			return;
		}
	}

	if (tableToken.IsFileOpen())
	{
		tableToken.Rewind();
		int length;
		while ((length = tableToken.GetToken(token, TOKEN_LENGTH)) > 0)
		{
			EKineticType kineticType = EvaluateKinetic(ParseToken(token));
			int index = GetIndex(token);
			if (index < 0)
			{
				if ((d_numEntries >= numEntries) ||
						!AppendEntry(token, length, false, kineticType))
				{
					Clear();
					ASSERT(false);
					return;
				}
			}
			else if (kineticType == KINETIC_STATIC)
			{
				if (d_entry[index].d_dynamic)
					d_numDynamicEntries--;
				d_entry[index].d_dynamic = false;
				d_entry[index].d_kineticTypeSpecified = true;
			}
			else if (kineticType == KINETIC_DYNAMIC)
			{
				if (!d_entry[index].d_dynamic)
					d_numDynamicEntries++;
				d_entry[index].d_dynamic = true;
				d_entry[index].d_kineticTypeSpecified = true;
			}
		}
	}
}

char* CSurfaceType::ParseToken(char* io_token)
{
	int i = 0;
	while (io_token[i] != '\0')
	{
		if (io_token[i] == ':')
		{
			io_token[i] = '\0';
			return(&io_token[i + 1]);
		}
		i++;
	}
	return(NULL);
}

CSurfaceType::EKineticType CSurfaceType::EvaluateKinetic(char* i_kinetic)
{
	if (i_kinetic == NULL)
		return(KINETIC_UNSPECIFIED);

	if (stricmp(i_kinetic, "static") == 0)
		return(KINETIC_STATIC);
	if (stricmp(i_kinetic, "dynamic") == 0)
		return(KINETIC_DYNAMIC);
	return(KINETIC_UNSPECIFIED);
}

bool CSurfaceType::AppendEntry(const char* i_name, int i_length,
		bool i_readOnly, EKineticType i_kineticType)
{
	d_entry[d_numEntries].d_name = new char[i_length + 1];
	ASSERT(d_entry[d_numEntries].d_name != NULL);
	if (d_entry[d_numEntries].d_name == NULL)
		return(false);
	strncpy(d_entry[d_numEntries].d_name, i_name, i_length);
	d_entry[d_numEntries].d_name[i_length] = '\0';

	if (i_length > d_maxNameLength)
		d_maxNameLength = i_length;

	if (i_readOnly)
		d_entry[d_numEntries].d_storage =
				SSurfaceEntry::SURFACE_TYPE_READ_ONLY;
	else
		d_entry[d_numEntries].d_storage = SSurfaceEntry::SURFACE_TYPE_SAVE;

	switch (i_kineticType)
	{
	case KINETIC_STATIC:
		d_entry[d_numEntries].d_dynamic = false;
		d_entry[d_numEntries].d_kineticTypeSpecified = true;
		break;
	case KINETIC_DYNAMIC:
		d_entry[d_numEntries].d_dynamic = true;
		d_entry[d_numEntries].d_kineticTypeSpecified = true;
		break;
	case KINETIC_UNSPECIFIED:
		d_entry[d_numEntries].d_dynamic = !i_readOnly;
		d_entry[d_numEntries].d_kineticTypeSpecified = false;
		break;
	}

	if (d_entry[d_numEntries].d_dynamic)
		d_numDynamicEntries++;

	d_numEntries++;
	return(true);
}

void CSurfaceType::Save(char* i_file) const
{
	if (d_entry == NULL)
	{
		conPuts("Attempting to save non-existant surface type table.\n");
		return;
	}
	char message[128];
	if (i_file && i_file[0])
		sprintf(message, "Saving surface type table %s.\n", i_file);
	else
		sprintf(message,
				"Trying to save surface type table to unspecified file.\n");
	conPuts(message);

	FileStream* fs = FileStream::NewAndOpen(i_file, FileStream::WRITE, true);
	if (fs != NULL)
	{
/*
 *		Write out the version control keywords. Break the strings up
 *		so the keywords are not replaced here.
 */
		fs->Write("#\n", 2);
		fs->Write("#   $", 5);
		fs->Write("Archive: $\n", 11);
		fs->Write("#  $", 4);
		fs->Write("Revision: $\n", 12);
		fs->Write("#      $", 8);
		fs->Write("Date: $\n", 8);
		fs->Write("#    $", 6);
		fs->Write("Author: $\n", 10);
		fs->Write("#\n", 2);
		fs->Write("\n", 1);

		for (int i = 0; i < d_numEntries; i++)
		{
			if (d_entry[i].d_storage != SSurfaceEntry::SURFACE_TYPE_DELETE)
			{
				fs->Write(d_entry[i].d_name, strlen(d_entry[i].d_name));
				if (d_entry[i].d_kineticTypeSpecified)
				{
					if (d_entry[i].d_dynamic)
						fs->Write(":dynamic", 8);
					else
						fs->Write(":static", 7);
				}
				fs->Write("\n", 1);
			}
		}

		fs->Close();
		delete fs;
	}
}

int CSurfaceType::AddEntry(const char* i_name, EKineticType i_kineticType)
{
	int index = GetIndex(i_name);
	if (index >= 0)
		return(index);

	if (i_name && i_name[0])
	{
		if (d_entry == NULL)
			d_numEntries = 0;

		SSurfaceEntry* newEntry = new SSurfaceEntry[d_numEntries + 1];
		ASSERT(newEntry != NULL);
		if (newEntry == NULL)
			return(-1);

		for (int i = 0; i < d_numEntries; i++)
		{
			newEntry[i].d_name = new char[strlen(d_entry[i].d_name) + 1];
			ASSERT(newEntry[i].d_name != NULL);
			if (newEntry[i].d_name == NULL)
			{
				for (int j = 0; j < i; j++)
					delete[] newEntry[j].d_name;
				delete[] newEntry;
				return(-1);
			}
			strcpy(newEntry[i].d_name, d_entry[i].d_name);
			newEntry[i].d_storage = d_entry[i].d_storage;
		}

		Clear();
		d_entry = newEntry;
		if (AppendEntry(i_name, strlen(i_name), false, i_kineticType))
			return(d_numEntries - 1);
	}
	return(-1);
}

bool CSurfaceType::DeleteEntry(const char* i_name, int i_testLength)
{
	return(DeleteEntry(GetIndex(i_name, i_testLength)));
}

bool CSurfaceType::DeleteEntry(int i_index)
{
	if ((d_entry != NULL) && (i_index >= 0) && (i_index < d_numEntries) &&
			(d_entry[i_index].d_storage !=
			SSurfaceEntry::SURFACE_TYPE_READ_ONLY))
	{
		d_entry[i_index].d_storage = SSurfaceEntry::SURFACE_TYPE_DELETE;
		return(true);
	}
	return(false);
}

char* CSurfaceType::GetName(int i_index) const
{
	if ((d_entry != NULL) && (i_index >= 0) && (i_index < d_numEntries))
		return(d_entry[i_index].d_name);
	return(NULL);
}

int CSurfaceType::GetValidDynamicIndex(char* io_name, int i_defaultIndex,
		int i_nameSize) const
{
	int type = GetIndex(io_name, i_nameSize - 1);
	if ((type >= 0) && d_entry[type].d_dynamic)
		return(type);

	char* name = NULL;
	if ((i_defaultIndex >= 0) && (i_defaultIndex < d_numEntries))
	{
		name = GetName(i_defaultIndex);
		if ((name == NULL) || !d_entry[i_defaultIndex].d_dynamic)
			i_defaultIndex = -1;
	}

	if ((i_defaultIndex < 0) || (i_defaultIndex >= d_numEntries))
	{
		for (i_defaultIndex = 0; i_defaultIndex < d_numEntries;
			 i_defaultIndex++)
		{
			if (d_entry[i_defaultIndex].d_dynamic)
				break;
		}
		if (i_defaultIndex >= d_numEntries)
			i_defaultIndex = 0;
		name = GetName(i_defaultIndex);
	}

	if (name == NULL)
		io_name[0] = '\0';
	else if (i_nameSize > 0)
	{
		strncpy(io_name, name, i_nameSize - 1);
		io_name[i_nameSize - 1] = '\0';
	}

	return(i_defaultIndex);
}

bool CSurfaceType::GetTitle(char* o_title, int i_length) const
{
	o_title[0] = '\0';
	int remain = i_length - 1;

	remain -= d_maxNameLength;
	if (remain < 0)
		return(true);
	sprintf(o_title, "%-*s", d_maxNameLength, NAME_TITLE);

	remain -= 10;
	if (remain < 0)
		return(true);
	strcat(o_title, "  SOURCE  ");

	remain -= 10;
	if (remain < 0)
		return(true);
	strcat(o_title, "  KINETIC ");

	remain -= 10;
	if (remain < 0)
		return(true);
	strcat(o_title, "  SPECIFY ");

	return(true);
}

char* CSurfaceType::GetInfo(int i_index, char* o_info, int i_length) const
{
	o_info[0] = '\0';

	if ((d_entry == NULL) || (i_index < 0) || (i_index >= d_numEntries))
		return(o_info);

	int remain = i_length - d_maxNameLength - 1;
	if (remain < 0)
		return(o_info);
	sprintf(o_info, "%-*s", d_maxNameLength, d_entry[i_index].d_name);

	remain -= 10;
	if (remain < 0)
		return(o_info);
	if (d_entry[i_index].d_storage == SSurfaceEntry::SURFACE_TYPE_READ_ONLY)
		strcat(o_info, "  database");
	else
		strcat(o_info, "  program ");

	remain -= 10;
	if (remain < 0)
		return(o_info);
	if (d_entry[i_index].d_dynamic)
		strcat(o_info, "  dynamic ");
	else
		strcat(o_info, "  static  ");

	remain -= 10;
	if (remain < 0)
		return(o_info);
	if (d_entry[i_index].d_kineticTypeSpecified)
		strcat(o_info, "  explicit");
	else
		strcat(o_info, "  implicit");

	return(o_info);
}
