/*
**
**  File:   TableToken.h
**  Date:   February 22, 2001
**  By:     Bryant Collard
**  Desc:   Uses the FileStream class to extract tokens from a file
**          for to build table label lists and table data.
**
**   $Archive: /Avalanche/GameLib/GameHelper/TableToken.h $
**  $Revision: 2 $
**      $Date: 3/02/01 1:26a $
**    $Author: Food $
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#ifndef TABLE_FILE_H
#define TABLE_FILE_H

class FileStream;

class CTableToken
{
  public:
	CTableToken();
	CTableToken(const char* i_file);
	~CTableToken();
	bool Init (const char* i_file);
	void Close(void);
	void Rewind(void);
	int GetToken(char* o_token, int i_maxLength) const;
	bool IsFileOpen(void) const {return(d_fs != 0);};
  private:
	FileStream* d_fs;
};

#endif // TABLE_FILE_H
