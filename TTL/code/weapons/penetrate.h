#ifndef SSTABLE_H
#define SSTABLE_H

#include "GameHelper/TableLookup.h"
#include "platform/BaseType.h"
#include "WatchClient/DataTable.h"    // To spoof watch.

class SPenetrationData
{
public:
	SPenetrationData();

	float		d_energy;
	float		d_deflection;
	int		d_percentBlocked;
	int		d_percentRicochet;
};

class CPenetrationTable : public CTableData, public DataTable<SPenetrationData>
{
public:
	CPenetrationTable();
	~CPenetrationTable();
	SPenetrationData* GetEntry(int i_surface1, int i_surface2);

	// Watch stuff.
	void LoadWatch();
	void UnloadWatch();
	void RequestData(int, int);
	void SetData(int, int, int, VAR_VAL&);
	bool NewRowColumn(char*, bool);
	char* TableName() { return "penetrat"; }
	int Version() { return 1; }
	bool Save(char* filename);

private:
	void Init(int i_size);
	bool LoadVariable(const char* i_variable, int i_entry);
	void Copy(int i_destEntry, int i_srcEntry);
	void SaveEntry(int i_entry);
	void Unload();

	SPenetrationData* d_table;

	//	Watch stuff.
	void RegisterVariables();
};

extern CPenetrationTable g_penetrationTable;

#endif // COLLISION_PARTICLE_H
