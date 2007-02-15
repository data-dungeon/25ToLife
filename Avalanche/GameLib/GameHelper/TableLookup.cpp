/*
**
**  File:   TableLookup.cpp
**  Date:   February 23, 2001
**  By:     Bryant Collard
**  Desc:   A base class for classes that loop up entries in tables
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/gamelibnew/GameHelper/TableLookup.cpp $
**  $Revision: 8 $
**      $Date: 1/13/03 4:32p $
**    $Author: Food $
**
*/

/******************* includes ****************************/

#include "GameHelper/GameHelperPCH.h"





#define TOKEN_LENGTH  256

CTableLookup::CTableLookup()
{
    d_head = 0;
    d_size = 0;
}

CTableLookup::~CTableLookup()
{
    Clear();
}

void CTableLookup::Clear()
{
    Unload();
    d_head = 0;
    d_size = 0;
}

void CTableLookup::InitTables(void)
{
    CTableData* curr = d_head;
    while (curr)
    {
        curr->Init(d_size);
        curr = curr->GetNext();
    }
}

bool CTableLookup::Load(char* i_filename)
{
    d_tableToken.Init(i_filename);
    if (!d_tableToken.IsFileOpen())
        return (false);

    int entry = -1;
    char token[TOKEN_LENGTH];
    while (d_tableToken.GetToken(token, TOKEN_LENGTH) > 0)
    {
        char *rowLabel, *colLabel;
        if (ParsePair(token, rowLabel, colLabel))
		  {
			   if (entry == -2)	// just finish parsing the common entry?
				{
					for (CTableData* curr = d_head; curr; curr = curr->GetNext())
						for (int i = 1; i < d_size; i++)
							curr->Copy(i, 0);
				}

			   if (strcmp(rowLabel, "COMMON") == 0 && strcmp(colLabel, "COMMON") == 0)
					entry = -2;
            else
					entry = GetEntry(GetRow(rowLabel), GetCol(colLabel));
		  }
        else if (entry >= 0)
        {
            CTableData* curr = d_head;
            while (curr)
            {
                if (curr->LoadVariable(token, entry))
                    break;
                curr = curr->GetNext();
            }
        }
		  else if (entry == -2)
		  {
			  // load the common into entry 0, will duplicate to all other entries
			  // when it's filled out.
			  for (CTableData* curr = d_head; curr; curr = curr->GetNext())
				  if (curr->LoadVariable(token, 0))
					  break;
		  }
    }

    d_tableToken.Close();
    return(true);
}

bool CTableLookup::LoadTables(char* i_filename)
{
    InitTables();
    return (Load(i_filename));
}

void CTableLookup::Save(char* i_filename)
{
/*
 *  Get the mangled platform independent file name.
 */
	char* filename = Media::GetCommonFileName(i_filename);
/*
 *  Open the file with name mangling disabled.
 */
    bool currMangleState = Media::GetNameManglingState();
    Media::EnableNameMangling(false);	
    d_fs = FileStream::NewAndOpen(filename, FileStream::WRITE, true);
    Media::EnableNameMangling(currMangleState);
    if (d_fs == NULL)
        return;
/*
 *  Write out the version control keywords. Break the strings up
 *  so the keywords are not replaced here.
 */
    d_fs->Write("#\n", 2);
    d_fs->Write("#   $", 5);
    d_fs->Write("Archive: $\n", 11);
    d_fs->Write("#  $", 4);
    d_fs->Write("Revision: $\n", 12);
    d_fs->Write("#      $", 8);
    d_fs->Write("Date: $\n", 8);
    d_fs->Write("#    $", 6);
    d_fs->Write("Author: $\n", 10);
    d_fs->Write("#\n", 2);
    d_fs->Write("\n", 1);

    for (int entry = 0; entry < d_size; entry++)
    {
        char *row, *col;
        if (!GetLabels(entry, row, col))
            continue;

        d_fs->Write("~", 1);
        d_fs->Write(row, strlen(row));
        d_fs->Write(":", 1);
        d_fs->Write(col, strlen(col));
        d_fs->Write("\n", 1);

        CTableData* curr = d_head;
        while (curr)
        {
            curr->SaveEntry(entry);
            curr = curr->GetNext();
        }
    }

    d_fs->Close();
    delete d_fs;
    d_fs = NULL;
}

void CTableLookup::Unload(void)
{
    CTableData* curr = d_head;
    while (curr)
    {
        curr->Unload();
        curr = curr->GetNext();
    }
}

void CTableLookup::AddDataTable(CTableData* i_table)
{
    i_table->SetLookup(this);
    i_table->SetNext(d_head);
    d_head = i_table;
}

int CTableLookup::LoadNextToken(char* o_token, int i_length)
{
    return(d_tableToken.GetToken(o_token, i_length));
}

void CTableLookup::SaveToken(const char* i_token, int i_length)
{
    ASSERT(d_fs != NULL);
    d_fs->Write("\t", 1);
    d_fs->Write(i_token, i_length);
    d_fs->Write("\n", 1);
}

// some handy utilites (use instead of the above)
bool CTableLookup::ParseNextToken(int &o_value)
{
	char variable[256];
	if (LoadNextToken(variable, sizeof(variable)) > 0)
	{
		if (sscanf(variable, "%d", &o_value) == 1)
			return true;
	}

	return false;
}

bool CTableLookup::ParseNextToken(float &o_value)
{
	char variable[256];
	if (LoadNextToken(variable, sizeof(variable)) > 0)
	{
		if (sscanf(variable, "%f", &o_value) == 1)
			return true;
	}

	return false;
}

bool CTableLookup::ParseNextToken(char *o_value, int i_length)
{
	if (LoadNextToken(o_value, i_length) <= 0)
		return false;
	if (strcmp(o_value, "<nil>") == 0)
		o_value[0] = '\0';
	return true;
}

void CTableLookup::SaveNamedToken(const char *i_name, int i_value)
{
	char token[256];
	sprintf(token, "%s %d", i_name, i_value);
	SaveToken(token, strlen(token));
}

void CTableLookup::SaveNamedToken(const char *i_name, float i_value)
{
	char token[256];
	sprintf(token, "%s %f", i_name, i_value);
	SaveToken(token, strlen(token));
}

void CTableLookup::SaveNamedToken(const char *i_name, const char *i_value)
{
	char token[256];
	if (!i_value || !i_value[0])	
		sprintf(token, "%s <nil>", i_name);
	else
		sprintf(token, "%s %s", i_name, i_value);
	SaveToken(token, strlen(token));
}

bool CTableLookup::ParsePair(char* io_token, char* &o_rowLabel,
        char* &o_colLabel)
{
    if (io_token[0] != '~')
        return(false);

    o_rowLabel = &io_token[1];
    o_colLabel = NULL;

    int i = 1;
    while (io_token[i] != '\0')
    {
        if (io_token[i] == ':')
        {
            io_token[i] = '\0';
            o_colLabel = &io_token[i + 1];
            break;
        }
        i++;
    }

    return(o_colLabel != NULL);
}

CTableData::~CTableData()
{
}
