/*
**
**  File:   SimpleType.cpp
**  Date:   February 26, 2001
**  By:     Bryant Collard
**  Desc:   A CTableLabel class for a simple list of types.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/GameHelper/SimpleType.cpp $
**  $Revision: 8 $
**      $Date: 11/21/02 12:25p $
**    $Author: Dross $
**
*/

/******************* includes ****************************/

#include "GameHelper/GameHelperPCH.h"





#define TOKEN_LENGTH  256

CSimpleType::CSimpleType()
{
    d_entry = NULL;
}

CSimpleType::~CSimpleType()
{
    Clear();
}

void CSimpleType::Clear(void)
{
    if (d_entry != NULL)
    {
        int i;
        for (i = 0; i < d_numEntries; i++)
            delete[] d_entry[i].d_name;
        delete[] d_entry;
        d_entry = NULL;
        d_numEntries = 0;
    }
}

void CSimpleType::Load(char* i_file)
{
    ASSERT(d_entry == NULL);
    if (d_entry != NULL)
        return;

    int numEntries = 0;
    char token[TOKEN_LENGTH];
    CTableToken tableToken(i_file);
    if (tableToken.IsFileOpen())
    {
        while (tableToken.GetToken(token, TOKEN_LENGTH) > 0)
            numEntries++;
    }

    if (numEntries <= 0)
        return;

    d_entry = new SSimpleEntry[numEntries];
    ASSERT(d_entry != NULL);
    if (d_entry == NULL)
        return;
    d_numEntries = 0;

    if (tableToken.IsFileOpen())
    {
        tableToken.Rewind();
        int length;
        while ((length = tableToken.GetToken(token, TOKEN_LENGTH)) > 0)
        {
            if (GetIndex(token) < 0)
            {
                if ((d_numEntries >= numEntries) ||
                        !AppendEntry(token, length))
                {
                    Clear();
                    ASSERT(false);
                    return;
                }
            }
        }
    }
}

bool CSimpleType::AppendEntry(const char* i_name, int i_length)
{
    d_entry[d_numEntries].d_name = new char[i_length + 1];
    ASSERT(d_entry[d_numEntries].d_name != NULL);
    if (d_entry[d_numEntries].d_name == NULL)
        return(false);
    strncpy(d_entry[d_numEntries].d_name, i_name, i_length);
    d_entry[d_numEntries].d_name[i_length] = '\0';
    d_entry[d_numEntries].d_delete = false;
    d_numEntries++;
    return(true);
}

void CSimpleType::Save(char* i_file) const
{
    if (d_entry == NULL)
        return;

    FileStream* fs = FileStream::NewAndOpen(i_file, FileStream::WRITE, true);
    if (fs != NULL)
    {
/*
 *      Write out the version control keywords. Break the strings up
 *      so the keywords are not replaced here.
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
            if (!d_entry[i].d_delete)
            {
                fs->Write(d_entry[i].d_name, strlen(d_entry[i].d_name));
                fs->Write("\n", 1);
            }
        }

        fs->Close();
        delete fs;
    }
}

int CSimpleType::AddEntry(const char* i_name)
{
    int index = GetIndex(i_name);
    if (index >= 0)
        return(index);

    if (i_name && i_name[0])
    {
        if (d_entry == NULL)
            d_numEntries = 0;

        SSimpleEntry* newEntry = new SSimpleEntry[d_numEntries + 1];
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
            newEntry[i].d_delete = d_entry[i].d_delete;
        }

        Clear();
        d_entry = newEntry;
        if (AppendEntry(i_name, strlen(i_name)))
            return(d_numEntries - 1);
    }
    return(-1);
}

bool CSimpleType::DeleteEntry(const char* i_name, int i_testLength)
{
    return(DeleteEntry(GetIndex(i_name, i_testLength)));
}

bool CSimpleType::DeleteEntry(int i_index)
{
    if ((d_entry != NULL) && (i_index >= 0) && (i_index < d_numEntries))
    {
        d_entry[i_index].d_delete = true;
        return(true);
    }
    return(false);
}

char* CSimpleType::GetName(int i_index) const
{
    if ((d_entry != NULL) && (i_index >= 0) && (i_index < d_numEntries))
        return(d_entry[i_index].d_name);
    return(NULL);
}
