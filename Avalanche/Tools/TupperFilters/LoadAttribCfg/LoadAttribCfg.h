////////////////////////////////////////////////////////////////////////////
//
// MeshPath.h
//
// Class to handle the Mesh Path Plugin
//
////////////////////////////////////////////////////////////////////////////

#ifndef LOAD_ATTRIB_CFG_H
#define LOAD_ATTRIB_CFG_H

class TupperwareAggregate;

class LoadAttribCfgFilter : public FilterPlugin
{
	TupString m_nodeConfigFile;
	TupString m_faceConfigFile;
	TupString m_mapConfigFile;
public:
	LoadAttribCfgFilter(TupperChainLogBase &log);

	int GetTotalPasses(void) const;

	void Process(int pass,const FilterOptions &options,FilterDataManager &dataManager);
	void CheckOptions(const FilterOptions &options);
};

#endif
