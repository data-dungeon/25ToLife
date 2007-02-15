//
//	ICustomSpinnerImp
//	

/*
 * $History: ICustomSpinnerImp.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/22/02   Time: 1:18p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * all compiling
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/21/02   Time: 4:07p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * spinner, sliders, interfaces, and implemtations
 */

#ifndef ATTR_CONTROLS_ICUSTOM_SPINNER_IMP_H
#define ATTR_CONTROLS_ICUSTOM_SPINNER_IMP_H

#ifndef ATTR_CONTROLS_ICUSTOM_SPINNER_H
#include "ICustomSpinner.h"
#endif

namespace AttrControls 
{

class CustomSpinner;

class ICustomSpinnerImp : public ICustomSpinner 
{
	CustomSpinner *m_pControl;
public:
	ICustomSpinnerImp(CustomSpinner *pControl);

	// ICustomControl
	HWND GetHWnd(void);
	void Enable(BOOL onOff=TRUE);
	void Disable(void);
	BOOL IsEnabled(void);

// ICustomSpinnerImp
	float GetFVal(void);
	int GetIVal(void);
	void SetAutoScale(BOOL on=TRUE);
	void SetScale( float s );
	void SetValue( float v, BOOL bNotify );
	void SetValue( int v, BOOL bNotify );
	void SetLimits( int min, int max, BOOL bLimitCurValue = TRUE );
	void SetLimits( float min, float max, BOOL bLimitCurValue = TRUE );
	void LinkToEdit( HWND hEdit, ICustomSpinner::EditSpinnerType type );
	void SetIndeterminate(BOOL i=TRUE);
	BOOL IsIndeterminate(void);
	void SetResetValue(float v);
	void SetResetValue(int v);
	void SetKeyBrackets(BOOL onOff);
	void SetSnapSpinner(BOOL b);
	BOOL GetSnapSpinner(void);
	void SetSnapSpinValue(float f);
	float GetSnapSpinValue(void);
	void SetSpinnerFormat(const char *pFormat);
	const char *GetSpinnerFormat(void);
	void SetSpinnerWrap(BOOL b);
	BOOL GetSpinnerWrap(void);
	void SetDragNotify(BOOL b);
	BOOL GetDragNotify(void);


};



} // AttrControls

#endif
