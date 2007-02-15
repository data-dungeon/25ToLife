////////////////////////////////////////////////////////////////////////////
//
// ClassDesc
//
// classes to handle the class desc for the plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ClassDesc.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/24/02   Time: 4:29p
 * Updated in $/Avalanche/tools/Plugins/Max/TupImport
 * materials & maps
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 10/23/02   Time: 9:54a
 * Created in $/Avalanche/tools/Plugins/Max/TupImport
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/05/01   Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Max/TupImport
 * Level A  complete
 */

#include "TupImportPch.h"

#include "ClassDesc.h"
#include "TupImport.h"

TupImportClassDesc TupImportDesc;

int TupImportClassDesc::IsPublic()
{
	return TRUE; 
}

void *TupImportClassDesc::Create(BOOL loading)
{
	return new TupImport(); 
}

const TCHAR *TupImportClassDesc::ClassName()
{ 
	return GetString(IDS_CLASS_NAME); 
}

SClass_ID TupImportClassDesc::SuperClassID() 
{
	return SCENE_IMPORT_CLASS_ID; 
}

Class_ID TupImportClassDesc::ClassID()
{
	return TUPIMPORT_CLASS_ID; 
}

const TCHAR *TupImportClassDesc::Category()
{
	return GetString(IDS_CATEGORY); 
}

const TCHAR *TupImportClassDesc::InternalName()
{
	return _T("TupImport"); 
}

HINSTANCE TupImportClassDesc::HInstance()
{
	return hInstance;
}
