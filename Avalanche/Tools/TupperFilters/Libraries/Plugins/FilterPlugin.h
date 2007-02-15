////////////////////////////////////////////////////////////////////////////
//
// FilterPlugin
//
// Base class for all plugin filters
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_FILTER_PLUGIN_H
#define TUPPERFILTERS_LIB_PLUGINS_FILTER_PLUGIN_H

class FilterOptions;
class FilterDataManager;
class TupperChainLogBase;
class FilePathArray;

#ifndef TUPPERFILTERS_LIB_PLUGINS_REVISION_H
#include "revision.h"
#endif

class FilterPlugin
{
	FilterPlugin(void); // not defined
protected:
	TupperChainLogBase &m_log;

public:
	FilterPlugin(TupperChainLogBase &log) : m_log(log) {}
	virtual ~FilterPlugin() {};
	virtual int GetTotalPasses(void) const = 0;

	virtual int GetTotalWriteDependencies(int pass,const FilterOptions &options) const { return 0; }
	virtual const char *GetWriteDependencyKey(int pass,const FilterOptions &options,int index) const { return 0; }
	virtual Revision GetWriteDependencyRevision(int pass,const FilterOptions &options,int index) const { return Revision(0,0); }
	virtual int GetTotalReadDependencies(int pass,const FilterOptions &options) const { return 0; }
	virtual const char *GetReadDependencyKey(int pass,const FilterOptions &options,int index) const { return 0; }
	virtual Revision GetReadDependencyRevision(int pass,const FilterOptions &options,int index) const { return Revision(0,0); }

	virtual void GetFileDependencies(int pass,const FilterOptions &options,FilterDataManager &dataManager,FilePathArray &filePathArray) {}

	virtual void Process(int pass,const FilterOptions &options,FilterDataManager &dataManager) = 0;
};

#endif
