////////////////////////////////////////////////////////////////////////////
//
// ImportScene
//
// Class to handle the scene
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImportImage.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/24/02   Time: 4:29p
 * Updated in $/Avalanche/tools/Plugins/Max/TupImport
 * materials & maps
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 10/23/02   Time: 9:54a
 * Created in $/Avalanche/tools/Plugins/Max/TupImport
 */

#include "TupImportPch.h"
#include "TupImport.h"
#include "filepath.h"

char *TupImport::GetImagePathName(int imageRef)
{
	TupperwareAggregate *pImageAgg = m_pImagePool->GetImage(imageRef);
	assert(pImageAgg);
	char *pName;
	TupImage tupImage(pImageAgg);
	tupImage.GetFilePathName(&pName);
	return pName;
}
