////////////////////////////////////////////////////////////////////////////
//
// Stamp
//
// Class to handle the Stamp Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: Stamp.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:29p
 * Updated in $/Avalanche/tools/TupperFilters/Stamp
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/24/03    Time: 9:41a
 * Updated in $/Avalanche/tools/TupperFilters/Stamp
 * Added Stamp Filter
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 3/24/03    Time: 9:29a
 * Created in $/Avalanche/tools/TupperFilters/Stamp
 */

////////////////////////////////////////////////////////////////////////////

#include "StampPch.h"
#include "Stamp.h"


StampFilter::StampFilter(TupperChainLogBase &log) : FilterPlugin(log)
{
}

int StampFilter::GetTotalPasses(void) const
{
	return 1;
}

void StampFilter::Process(int pass,const FilterOptions &options,FilterDataManager &dataManager)
{
	PluginLog::SetLog(&m_log);
}


/*

List of items to do for the filter

Build Stamp Database
	Scan through and find stamps
	Setup information about each stamp
Go through each node
	if attrbutes allow stamps and is instanced only once
	go through each stamp
	if stamp is able to hit mesh
		if mesh database is not created then create it
		go through each triangle in the mesh
			if triangle is able to be affected by stamp
				project points from triangle to stamp plane
				compute uv coordinates for face using stamp triangles
				test if uv coordinates are intersecting 0-1 range of uv space
				if so add texture layer to face in database
	if any stamps were applied to the mesh
		create new materials, maps, etc
		add new uv maps to mesh

Components:
	Stamp Database
		Individual Stamps
	Mesh Database
	Managers:
		Main
			Main Flow
		Stamp Manager
			Individual Stamps
		Stamp Mesh Manager
			Build data using stamp information and face
			Applies layers to mesh and creates new materials, maps, etc

			
*/