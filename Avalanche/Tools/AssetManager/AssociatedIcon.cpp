////////////////////////////////////////////////////////////////////////////
//
// AssociatedIcon
//
// Class to keep track of associated icons
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AssociatedIcon.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/11/02   Time: 12:21p
 * Updated in $/Avalanche/tools/AssetManager
 * associated icons working (not psd files though)
 */

#include "stdafx.h"
#include "AssociatedIcon.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

AssociatedIconManager::AssociatedIconManager(void)
{
}

AssociatedIconManager::~AssociatedIconManager()
{
	ClearAll();
}

int AssociatedIconManager::GetIconIndex(const char *pExtension)
{
	TupString extString(pExtension);
	extString.MakeLower(); // all lower case
	extString.Remove('.'); // no periods
	int size = m_extensions.GetSize();
	int index = m_extensions.Add(extString);
	if (index==size) // was it just added?
	{
		// create dummy filename "foo.ext" 
		TupString sDummyFileName = _T("foo.") + extString;
		
		// Call SHGetFileInfo to get the icon. Call with dummy filename and
		// SHGFI_USEFILEATTRIBUTES flag, so file doesn't actually have to
		// exist.
		SHFILEINFO shfi;
		memset(&shfi,0,sizeof(shfi));
		SHGetFileInfo(sDummyFileName,
			FILE_ATTRIBUTE_NORMAL,
			&shfi,
			sizeof(shfi),
			SHGFI_ICON|SHGFI_SMALLICON|SHGFI_USEFILEATTRIBUTES);

		m_icons.Add(shfi.hIcon);
	}
	return index;
}

HICON AssociatedIconManager::GetIcon(int index)
{
	assert(index>=0&&index<m_icons.GetSize());
	return m_icons[index];
}

void AssociatedIconManager::ClearAll(void)
{
	m_extensions.RemoveAll();
	int numIcons = m_icons.GetSize();
	for (int iconIndex=0;iconIndex<numIcons;iconIndex++)
	{
		DestroyIcon(m_icons[iconIndex]);
	}
	m_icons.RemoveAll();
}


