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
 * User: Adam Clayton Date: 12/05/01   Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Level A  complete
 */

#include "TupExportPch.h"

#include "ClassDesc.h"
#include "TupExport.h"

TupExportClassDesc TupExportDesc;

int TupExportClassDesc::IsPublic()
{
	return TRUE; 
}

void *TupExportClassDesc::Create(BOOL loading)
{
	return new TupExport(); 
}

const TCHAR *TupExportClassDesc::ClassName()
{ 
	return GetString(IDS_CLASS_NAME); 
}

SClass_ID TupExportClassDesc::SuperClassID() 
{
	return SCENE_EXPORT_CLASS_ID; 
}

Class_ID TupExportClassDesc::ClassID()
{
	return TUPEXPORT_CLASS_ID; 
}

const TCHAR *TupExportClassDesc::Category()
{
	return GetString(IDS_CATEGORY); 
}

const TCHAR *TupExportClassDesc::InternalName()
{
	return _T("TupExport"); 
}

HINSTANCE TupExportClassDesc::HInstance()
{
	return hInstance;
}
