//===================================================================
//  File:  WatchTable.h
//  Description:
//     Declaration of a WatchTable class that is merely an interface
//     for classes that need to use the table capabilities of Watch.
//===================================================================
#ifndef _hWatchTable_
#define _hWatchTable_

#include "WatchClient/WatchInfo.h"

class WatchTable
{
public:
	virtual void RequestData(int, int) = 0;
	virtual void SetData(int, int, int, VAR_VAL&) = 0;
	virtual bool Save(char*) = 0;
	virtual bool Open(char*) = 0;
	virtual bool NewRowColumn(char*, bool) = 0;
	virtual bool FillRowColumn(TABLE_DATA&) = 0;
};

#endif // _hWatchTable_
