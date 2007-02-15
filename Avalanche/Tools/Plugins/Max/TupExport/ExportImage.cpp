////////////////////////////////////////////////////////////////////////////
//
// ExportScene
//
// Class to handle the scene
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportImage.cpp $
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 9/09/02    Time: 4:29p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * oops on the paths again
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 9/06/02    Time: 3:36p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * paths should not be case sensitive
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 9/04/02    Time: 11:14a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * absolute paths for image and xref files
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 5/21/02    Time: 1:55p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Fixed due to changes in keeper
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 5/09/02    Time: 10:22a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Moved to TupArray and TupString
 * Fixed bug with keepers and TupStrings (GetHashValue)
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 2:04p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Relative Path Support for Images
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/27/01   Time: 4:30p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * xrefs & derived objects (skeleton)
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/17/01   Time: 11:07a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Materials & maps exporting (others still to do)
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/14/01   Time: 4:39p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Start of standard material exporting
 */

#include "TupExportPch.h"
#include "TupExport.h"
#include "filepath.h"

int TupExport::AddImage(const TupString &mapName)
{
	TupString relativePath,fullPath1,fullPath2,name,ext;
	// get full path of mapname)
	FilePath::GetPathsToFile(relativePath,fullPath1,name,ext,mapName,m_sourceFilename);
	// get relative path from output filename to mapname
	FilePath::GetPathsToFile(relativePath,fullPath2,name,ext,fullPath1,m_outputFilename);
	fullPath2.MakeLower(); // not case sensitive

	int index = m_pImagePoolStrings->GetIndex(fullPath2);
	if (index==-1) // if it is not in the list
	{
		index = m_pImagePoolStrings->Add(fullPath2);
		TupperwareAggregate *pImageAgg = m_pImagePool->AddImage(index);
		TupImage tupImage(pImageAgg);
//		tupImage.AddFilePathName(relativePath);
		tupImage.AddFilePathName(fullPath2);
	}
	return index;
}
