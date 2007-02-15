/*
**
**  File:   TableToken.cpp
**  Date:   February 22, 2001
**  By:     Bryant Collard
**  Desc:   Uses the FileStream class to extract tokens from a file
**          for to build table label lists and table data.
**
**   $Archive: /Avalanche/GameLibNew/GameHelper/TableToken.cpp $
**  $Revision: 6 $
**      $Date: 11/21/02 12:25p $
**    $Author: Dross $
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

// Precompiled header
#include "GameHelper/GameHelperPCH.h"

CTableToken::CTableToken()
{
	d_fs = NULL;
}

CTableToken::CTableToken(const char* i_file)
{
	d_fs = NULL;
	Init(i_file);
}

CTableToken::~CTableToken()
{
	Close();
}

bool CTableToken::Init (const char* i_file)
{
	Close();
	d_fs = FileStream::NewAndOpen(i_file, FileStream::READ, true);
	return (d_fs != NULL);
}

void CTableToken::Close(void)
{
	if (d_fs != NULL)
	{
		d_fs->Close();
		delete d_fs;
		d_fs = NULL;
	}
}

void CTableToken::Rewind(void)
{
	if (d_fs != NULL)
		d_fs->Seek(0, FileStream::SET);
}

int CTableToken::GetToken(char* o_token, int i_maxLength) const
{
	if (d_fs == NULL)
		return(-1);

	char c;
	while(true)
	{
		if (d_fs->Read(&c, 1) != 1)
			return(-1);

		if (c == '#')
		{
			while(true)
			{
				if (d_fs->Read(&c, 1) != 1)
					return(-1);
				if (c == '\n')
					break;
			}
			continue;
		}

		if ((c >= '!') && (c <= '~'))
			break;
	}

	i_maxLength -= 1;
	int length = 0;

	if (c == '\"')
	{
		while(true)
		{
			if ((d_fs->Read(&c, 1) != 1) || (c == '\"'))
				break;
			if (length < i_maxLength)
				o_token[length++] = c;
		}
	}
	else
	{
		do
		{
			if ((c < '!') || (c > '~'))
				break;
			if (c == '#')
			{
				d_fs->Seek(-1, FileStream::CUR);
				break;
			}
			if (length < i_maxLength)
				o_token[length++] = c;
		} while(d_fs->Read(&c, 1) == 1);
	}

	o_token[length] = '\0';
	return(length);
}
