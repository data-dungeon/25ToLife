/*
**
**  File:   SimpleType.h
**  Date:   February 26, 2001
**  By:     Bryant Collard
**  Desc:   A CTableLabel class for a simple list of types.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLib/GameHelper/SimpleType.h $
**  $Revision: 2 $
**      $Date: 3/02/01 1:26a $
**    $Author: Food $
**
*/

#ifndef SIMPLE_TYPE_H
#define SIMPLE_TYPE_H

#include "GameHelper/TableLabel.h"

class FileStream;

struct SSimpleEntry
{
    char* d_name;
    bool d_delete;
};

class CSimpleType : public CTableLabel
{
  public:
    CSimpleType();
    ~CSimpleType();
    void Clear(void);
    void Load(char* i_file);
    void Save(char* i_file) const;
    int AddEntry(const char* i_name);
    bool DeleteEntry(const char* i_name, int i_testLength = -1);
    bool DeleteEntry(int i_index);
    virtual char* GetName(int i_index) const;
  private:
    bool AppendEntry(const char* i_name, int i_length);
    SSimpleEntry* d_entry;
};

#endif //  SIMPLE_TYPE_H
