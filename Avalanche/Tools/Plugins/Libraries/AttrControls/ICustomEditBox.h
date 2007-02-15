//
//	ICustomEditBox
//	

/*
 * $History: ICustomEditBox.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/19/02   Time: 3:47p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/21/02   Time: 4:07p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * spinner, sliders, interfaces, and implemtations
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/20/02   Time: 4:20p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * working on custom control port
 */

#ifndef ATTR_CONTROLS_ICUSTOM_EDIT_BOX_H
#define ATTR_CONTROLS_ICUSTOM_EDIT_BOX_H

#ifndef ATTR_CONTROLS_ICUSTOM_CONTROL_H
#include "ICustomControl.h"
#endif


// Sent when the user hits the enter key in an edit control.
// wParam = cust edit ID
// lParam = HWND of cust edit control.
#define WM_ATCUSTOMEDIT_ENTER	(WM_USER+685)

namespace AttrControls 
{

class ICustomEditBox : public ICustomControl 
{
public:
	virtual void GetText( char *text, int ct )=0;
	virtual void SetText( const char *text )=0;
	virtual void SetText( int i )=0;
	virtual void SetText( float f, const char *pFormat )=0;
	virtual int GetInt(BOOL *valid=NULL)=0;
	virtual float GetFloat(BOOL *valid=NULL)=0;
	virtual void SetLeading(int lead)=0;
	virtual void WantReturn(BOOL yesNo)=0;
	virtual BOOL GotReturn(void)=0;		// call this on receipt of EN_CHANGE
	virtual void GiveFocus(void)=0;
	virtual BOOL HasFocus(void)=0;
	virtual void WantDlgNextCtl(BOOL yesNo)=0;
	virtual void SetNotifyOnKillFocus(BOOL onOff)=0;
	virtual void SetBold(BOOL onOff)=0;
	virtual HWND GetHEdit(void)=0;

	static ICustomEditBox *GetICustomEditBox(HWND hCtrl);
	static void ReleaseICustomEditBox(ICustomEditBox *pControl);
};

} // AttrControls

#endif