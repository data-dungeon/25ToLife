/*
**
**  File:   TableLabel.h
**  Date:   February 22, 2001
**  By:     Bryant Collard
**  Desc:   A base class for classes that contian lists of table labels.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLib/GameHelper/TableLabel.h $
**  $Revision: 1 $
**      $Date: 2/28/01 11:48a $
**    $Author: Bcollard $
**
*/

#ifndef TABLE_LABEL_H
#define TABLE_LABEL_H

class CTableLabel
{
  public:
    CTableLabel() {d_numEntries = 0;};
    virtual ~CTableLabel() {};
    int GetIndex(const char* i_name, int i_testLength = -1) const;
    int GetValidIndex(char* io_name, int i_defaultIndex,
            int i_nameSize) const;
    int GetNumEntries(void) const {return(d_numEntries);};
    virtual char* GetName(int i_index) const = 0;
    virtual bool GetTitle(char* o_title, int i_length) const {return(false);};
    virtual char* GetInfo(int i_index, char* o_info, int i_length) const
        {return(GetName(i_index));};
  protected:
    int d_numEntries;
};

#endif //  TABLE_LABEL_H
