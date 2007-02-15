////////////////////////////////////////////////////////////////////////////
//
// AssociatedIcon
//
// Class to keep track of associated icons
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AssociatedIcon.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/11/02   Time: 12:21p
 * Updated in $/Avalanche/tools/AssetManager
 * associated icons working (not psd files though)
 */

#ifndef ASSET_MANAGER_ASSOCIATED_ICON_H
#define ASSET_MANAGER_ASSOCIATED_ICON_H

class AssociatedIconManager
{
	Keeper<TupString> m_extensions;
	TupArray<HICON> m_icons;
public:
	AssociatedIconManager(void);
	~AssociatedIconManager();

	int GetIconIndex(const char *pExtension);
	HICON GetIcon(int index);
	void ClearAll(void);
};

#endif

