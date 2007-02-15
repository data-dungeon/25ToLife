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
**   $Archive: /Avalanche/GameLibNew/GameHelper/TableLabel.cpp $
**  $Revision: 6 $
**      $Date: 11/21/02 12:25p $
**    $Author: Dross $
**
*/

/******************* includes ****************************/

#include "GameHelper/GameHelperPCH.h"





#if defined(GCN) && defined(CW)
#include "platform/GCN/GCNMWPort.h"
#endif

int CTableLabel::GetIndex(const char* i_name, int i_testLength) const
{
    if ((i_name == NULL) || (i_name[0] == '\0'))
        return(-1);

    int index;

    if (i_testLength >= 0)
    {
        for (index = 0; index < d_numEntries; index++)
        {
            char* name = GetName(index);
            if ((name != NULL) && (strnicmp(i_name, name, i_testLength) == 0))
                break;
        }
    }
    else
    {
        for (index = 0; index < d_numEntries; index++)
        {
            char* name = GetName(index);
            if ((name != NULL) && (strcmpi(i_name, name) == 0))
                break;
        }
    }

    if (index >= d_numEntries)
        return(-1);
    return(index);
}

int CTableLabel::GetValidIndex(char* io_name, int i_defaultIndex,
        int i_nameSize) const
{
    int type = GetIndex(io_name, i_nameSize - 1);
    if (type >= 0)
        return(type);

    char* name = NULL;
    if ((i_defaultIndex >= 0) && (i_defaultIndex < d_numEntries))
    {
        name = GetName(i_defaultIndex);
        if (name == NULL)
            i_defaultIndex = -1;
    }
	 else
    {
        i_defaultIndex = 0;
        name = GetName(i_defaultIndex);
    }

    if (name == NULL)
        io_name[0] = '\0';
    else
    {
        strncpy(io_name, name, i_nameSize - 1);
        io_name[i_nameSize - 1] = '\0';
    }

    return(i_defaultIndex);
}
