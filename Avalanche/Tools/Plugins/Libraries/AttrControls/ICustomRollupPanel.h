//
//	ICustomRollupPanel
//	

/*
 * $History: ICustomRollupPanel.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 4:04p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * Rollups in
 */

#ifndef ATTR_CONTROLS_ICUSTOM_ROLLUP_PANEL_H
#define ATTR_CONTROLS_ICUSTOM_ROLLUP_PANEL_H

#ifndef ATTR_CONTROLS_ICUSTOM_CONTROL_H
#include "ICustomControl.h"
#endif

namespace AttrControls 
{

class ICustomRollupPanel
{
public:
	virtual HINSTANCE GetHInst(void)=0;
	virtual DWORD_PTR GetResID(void)=0;
	virtual BOOL operator==(const ICustomRollupPanel& id)=0;
	virtual HWND GetHWnd(void)=0;
	virtual HWND GetRollupWindowHWND(void)=0;
	virtual HWND GetTitleWnd(void)=0;
	virtual HWND GetDlgWnd(void)=0;

	static BOOL IsRollupPanelOpen(HWND hDlg);
	
};

} // AttrControls

#endif