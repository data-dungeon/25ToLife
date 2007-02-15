//
//	ICustomEditBoxImp
//	

/*
 * $History: ICustomEditBoxImp.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/19/02   Time: 3:47p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/22/02   Time: 1:18p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * all compiling
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/20/02   Time: 4:20p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * working on custom control port
 */

#ifndef ATTR_CONTROLS_ICUSTOM_EDIT_BOX_IMP_H
#define ATTR_CONTROLS_ICUSTOM_EDIT_BOX_IMP_H

#ifndef ATTR_CONTROLS_ICUSTOM_EDIT_BOX_H
#include "ICustomEditBox.h"
#endif

namespace AttrControls 
{

class CustomEditBox;

class ICustomEditBoxImp : public ICustomEditBox
{
	CustomEditBox *m_pControl;
public:
	ICustomEditBoxImp(CustomEditBox *pControl);

// ICustomControl
	HWND GetHWnd(void);
	void Enable(BOOL onOff=TRUE);
	void Disable(void);
	BOOL IsEnabled(void);

// ICustomEditBox
	void GetText( char *text, int ct );
	void SetText( const char *text );	
	void SetText( int i );
	void SetText( float f, const char *pFormat );
	int GetInt(BOOL *valid=NULL);
	float GetFloat(BOOL *valid=NULL);
	void SetLeading(int lead);
	void WantReturn(BOOL yesNo);
	BOOL GotReturn(void);		// call this on receipt of EN_CHANGE
	void GiveFocus(void);
	BOOL HasFocus(void);
	void WantDlgNextCtl(BOOL yesNo);
	void SetNotifyOnKillFocus(BOOL onOff);
	void SetBold(BOOL onOff);
	HWND GetHEdit(void);
};

} // AttrControls

#endif