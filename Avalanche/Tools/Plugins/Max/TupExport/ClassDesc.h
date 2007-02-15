////////////////////////////////////////////////////////////////////////////
//
// ClassDesc
//
// classes to handle the class desc for the plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ClassDesc.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/05/01   Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Level A  complete
 */

#ifndef __CLASS_DESC__H
#define __CLASS_DESC__H

class TupExportClassDesc : public ClassDesc2 
{
public:
	int 			IsPublic();
	void *			Create(BOOL loading = FALSE);
	const TCHAR *	ClassName();
	SClass_ID		SuperClassID();
	Class_ID		ClassID();
	const TCHAR* 	Category();

	const TCHAR*	InternalName();
	HINSTANCE		HInstance();

};

extern TupExportClassDesc TupExportDesc;


#endif

