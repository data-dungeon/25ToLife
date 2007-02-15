/*
**
**  File:   RigidBodyTable.h
**  Date:   July 5, 2004
**  By:     Bryant Collard
**  Desc:   A table for rigid body collision data.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLib/physics/RigidBodyTable.h $
**  $Revision: #6 $
**      $Date: 2005/03/29 $
**    $Author: Rob $
**
*/

#ifndef RIGID_BODY_TABLE_H
#define RIGID_BODY_TABLE_H

#include "GameHelper/TableLookup.h"
#include "platform/BaseType.h"
#include "WatchClient/DataTable.h"  // To spoof watch.

struct SRigidBodyData
{
	float d_cor;
	float d_cog;
};

class CRigidBodyTable : public CTableData, public DataTable<SRigidBodyData>
{
  public:
	CRigidBodyTable();
	virtual ~CRigidBodyTable();
	SRigidBodyData* GetEntry(int i_surface1, int i_surface2);
	SRigidBodyData* GetValidEntry(int i_surface1, int i_surface2);

	// Watch stuff.
	void LoadWatch(void);
	void UnloadWatch(void);
	void RequestData(int, int);
	void SetData(int, int, int, VAR_VAL&);
	bool NewRowColumn(char*, bool);
	char* TableName() {return "SurfSurf";};
	int Version() {return 1;};
	bool Save(char* filename);

  private:
	virtual void Init(int i_size);
	virtual bool LoadVariable(const char* i_variable, int i_entry);
	virtual void Copy(int i_destEntry, int i_srcEntry);
	virtual void SaveEntry(int i_entry);
	virtual void Unload(void);

	SRigidBodyData* d_table;

	// Watch stuff.
	void RegisterVariables();
};

extern CRigidBodyTable g_rigidBodyTable;

#endif // RIGID_BODY_TABLE_H
