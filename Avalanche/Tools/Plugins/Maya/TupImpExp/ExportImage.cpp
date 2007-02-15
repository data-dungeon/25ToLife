////////////////////////////////////////////////////////////////////////////
//
// ExportImage
//
// Tupperware importer/ exporter for Maya
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportImage.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/12/03    Time: 5:28p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * move around files
 * animation curve save out less data on child joints
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/13/02   Time: 4:21p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first pass of materials, maps, images, and attributes
 */

#include "TupImpExpPch.h"
#include "TupExport.h"

int TupExport::AddImage(const char *mapName)
{
	TupString mapPath(mapName);
	TupString relativePath,fullPath,name,ext;
	// get relative path from output filename to mapname
	FilePath::GetPathsToFile(relativePath,fullPath,name,ext,mapPath,m_outputFilename);
	fullPath.MakeLower(); // not case sensitive

	int index = m_pImagePoolStrings->GetIndex(fullPath);
	if (index==-1) // if it is not in the list
	{
		index = m_pImagePoolStrings->Add(fullPath);
		TupperwareAggregate *pImageAgg = m_pImagePool->AddImage(index);
		TupImage tupImage(pImageAgg);
		tupImage.AddFilePathName(fullPath);
	}
	return index;
}
