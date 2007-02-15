////////////////////////////////////////////////////////////////////////////
//
// Cineman.h
//
// Class to handle the Cineman Plugin
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef MOTION_SET_H
#define MOTION_SET_H

class MotionSetFilter : public FilterPlugin
{
public:
	MotionSetFilter(TupperChainLogBase &log);
	~MotionSetFilter();

	int GetTotalPasses(void) const;

	void Process(int pass,const FilterOptions &options,FilterDataManager &dataManager);

private:

	void CheckOptions(const FilterOptions &options);
};

#endif
