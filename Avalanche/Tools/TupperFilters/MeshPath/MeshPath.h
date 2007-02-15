////////////////////////////////////////////////////////////////////////////
//
// MeshPath.h
//
// Class to handle the Mesh Path Plugin
//
////////////////////////////////////////////////////////////////////////////

#ifndef MESH_PATH_H
#define MESH_PATH_H

class TupperwareAggregate;

class MeshPathFilter : public FilterPlugin
{
public:
	MeshPathFilter(TupperChainLogBase &log);

	int GetTotalPasses(void) const;

	void Process(int pass,const FilterOptions &options,FilterDataManager &dataManager);

};

#endif
