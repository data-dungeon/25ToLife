/*
**
**  File:   TableLookup.h
**  Date:   February 23, 2001
**  By:     Bryant Collard
**  Desc:   A base class for classes that loop up entries in tables
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLib/GameHelper/TableLookup.h $
**  $Revision: 3 $
**      $Date: 5/03/01 1:28p $
**    $Author: Todd Blackburn $
**
*/

#ifndef TABLE_LOOKUP_H
#define TABLE_LOOKUP_H

#include "GameHelper/TableToken.h"

class CTableData;
class FileStream;

class CTableLookup
{
  public:
    CTableLookup();
    virtual ~CTableLookup();
    virtual void Clear();
    void InitTables(void);
    bool Load(char* i_filename);
    bool LoadTables(char* i_filename);
    void Save(char* i_filename);
    void Unload(void);
    void AddDataTable(CTableData* i_table);
    int GetTableSize(void) {return(d_size);};
    virtual int GetRow(const char* i_label) = 0;
    virtual int GetCol(const char* i_label) = 0;
    virtual int GetEntry(int i_row, int i_col) = 0;
    virtual int GetValidEntry(int i_row, int i_col) = 0;
    virtual bool GetLabels(int i_entry, char* &o_row, char* &o_col) = 0;

    int LoadNextToken(char* o_token, int i_length);
    void SaveToken(const char* i_token, int i_length);

    // some handy utilites (use instead of the above)
    bool ParseNextToken(int &o_value);
    bool ParseNextToken(float &o_value);
    bool ParseNextToken(char *o_value, int i_length);

    void SaveNamedToken(const char *i_name, int i_value);
    void SaveNamedToken(const char *i_name, float i_value);
    void SaveNamedToken(const char *i_name, const char *i_value);

  protected:
    int d_size;
  private:
    bool ParsePair(char* io_token, char* &o_rowLabel, char* &o_colLabel);

    CTableData* d_head;
    CTableToken d_tableToken;
    FileStream* d_fs;
};

class CTableData
{
  public:
    CTableData() {d_lookup = 0; d_next = 0;};
    virtual ~CTableData();
  protected:
    CTableLookup* d_lookup;
  private:
    virtual void Init(int i_size) = 0;
    virtual bool LoadVariable(const char* i_variable, int i_entry) = 0;
    virtual void SaveEntry(int i_entry) = 0;
    virtual void Unload(void) = 0;
	 virtual void Copy(int i_destEntry, int i_srcEntry) = 0;
    void SetLookup(CTableLookup* i_lookup) {d_lookup = i_lookup;};
    void SetNext(CTableData* i_next) {d_next = i_next;};
    CTableData* GetNext(void) const {return(d_next);};

    CTableData* d_next;

    friend class CTableLookup;
};

#endif //  TABLE_LOOKUP_H
