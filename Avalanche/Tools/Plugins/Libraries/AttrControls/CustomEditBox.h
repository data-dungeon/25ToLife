//
//	CustomEditBox
//	

/*
 * $History: CustomEditBox.h $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/19/02   Time: 3:47p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 11/22/02   Time: 1:18p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * all compiling
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

#ifndef ATTR_CONTROLS_CUSTOM_EDIT_BOX_H
#define ATTR_CONTROLS_CUSTOM_EDIT_BOX_H

#ifndef ATTR_CONTROLS_CUSTOM_CONTROL_H
#include "CustomControl.h"
#endif

//----------------------------------------------------------------------------//
// ATCustEdit control

namespace AttrControls 
{

class CustomSpinner;
class CustomSlider;

class CustomEditBox : public CustomControl 
{
public:
	CustomEditBox::CustomEditBox( HWND hWnd );

// ICustomEditBox methods
	void GetText( char *text, int ct );
	void SetText( const char *text );	
	void SetText( int i );
	void SetText( float f, const char *pFormat);
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

// other methods
	void UpdateEditWindow(void);
	BOOL IsValidCharacter(int ch);
	void SetSpinner(CustomSpinner *pSpinner);
	void SetSlider(CustomSlider *pSlider);

// windows messages
	LRESULT OnCommand( WORD id, WORD notify, HWND hCtrl );
	LRESULT OnPaint(void);
	LRESULT OnRButDown( int x, int y, WORD keys );
	LRESULT OnSetFocus( HWND hLost );
	LRESULT OnWMEnable( BOOL bEnabled );
	LRESULT OnSize( int w, int h );

// member variables
	int m_xOff,m_yOff;
	CustomSpinner *m_pSpinner;
	CustomSlider *m_pSlider;
	BOOL m_bWantReturn;
	BOOL m_bGotReturn;
	BOOL m_bWantDlgNextCtrl;
	BOOL m_bNotifyOnKillFocus;
	HWND m_hStatic;
	HWND m_hEdit;

};

} // AttrControls

#endif