////////////////////////////////////////////////////////////////////////////
//
// RealTimeLighting
//
// Class to handle the RealTime Lighting Plugin
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef REAL_TIME_LIGHTING_H
#define REAL_TIME_LIGHTING_H

class TupperwareAggregate;

class RealTimeLightingSettings
{
public:
	bool m_bAssignLightsToInstance;
	int m_maxAssignedPointLights;
	bool m_bBreakUpMesh;
	float m_pointLightThreshold;
};

class RealTimeLightingFilter : public FilterPlugin
{
	RealTimeLightingSettings m_settings;

public:
	RealTimeLightingFilter(TupperChainLogBase &log);

	int GetTotalPasses(void) const;

	void Process(int pass,const FilterOptions &options,FilterDataManager &dataManager);

private:
	void CheckOptions(const FilterOptions &options);
};

#endif
