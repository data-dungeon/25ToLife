/*
**
**  File:   SurfaceLookup.h
**  Date:   February 23, 2001
**  By:     Bryant Collard
**  Desc:   Lookup class for surface to surface pairings.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLib/stage/SurfaceLookup.h $
**  $Revision: 3 $
**      $Date: 11/27/01 12:19p $
**    $Author: Nmartin $
**
*/

#ifndef SURFACE_LOOKUP_H
#define SURFACE_LOOKUP_H

#include "Layers/Assert/Assert.h"
#include "GameHelper/TableLookup.h"
#include "EngineHelper/Singleton.h"

class CSurfaceType;

class CSurfaceLookup : public CTableLookup
{
  public:
	CSurfaceLookup();
	~CSurfaceLookup();
	bool Init(CSurfaceType* i_surfaceType);
	virtual void Clear(void);
	void InitTables(CSurfaceType* i_surfaceType);
/*
 *	To enable CRigidBodyTable to spoof watch. Should go away.
 */
	int GetNumRow(void) {return(d_numRow);};
	int GetNumCol(void) {return(d_numCol);};
	char* GetMappedRow(int i_row);
	char* GetMappedCol(int i_row);
	int GetMappedEntry(int i_row, int i_col);
  private:
	virtual int GetRow(const char* i_label);
	virtual int GetCol(const char* i_label);
	virtual int GetEntry(int i_row, int i_col);
	virtual int GetValidEntry(int i_row, int i_col);
	virtual bool GetLabels(int i_entry, char* &o_row, char* &o_col);

	CSurfaceType* d_surfaceType;
	int d_numRow;
	int d_numCol;
	int* d_indexMap;
	int* d_reverseIndexMap;
};

DefineSingleton(CSurfaceLookup)
#define g_surfaceLookup GetSingleton(CSurfaceLookup)

#endif //  SURFACE_LOOKUP_H
