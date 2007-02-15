//
//	ICustomSpinner
//	

/*
 * $History: ICustomSpinner.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 9:58a
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * added new controls to numeric type.
 * Fixed color issues with Max
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

#ifndef ATTR_CONTROLS_ICUSTOM_SPINNER_H
#define ATTR_CONTROLS_ICUSTOM_SPINNER_H

#ifndef ATTR_CONTROLS_ICUSTOM_CONTROL_H
#include "ICustomControl.h"
#endif

// LOWORD(wParam) = ctrlID, 
// HIWORD(wParam) = TRUE if user is dragging the spinner interactively.
// lParam = pointer to ISpinnerControl
#define CC_ATSPINNER_CHANGE  		WM_USER + 600	

// LOWORD(wParam) = ctrlID, 
// lParam = pointer to ISpinnerControl
#define CC_ATSPINNER_BUTTONDOWN	WM_USER + 601

// LOWORD(wParam) = ctrlID, 
// HIWORD(wParam) = FALSE if user cancelled - TRUE otherwise
// lParam = pointer to ISpinnerControl
#define CC_ATSPINNER_BUTTONUP		WM_USER + 602

namespace AttrControls 
{

class ICustomSpinner : public ICustomControl 
{
public:
	virtual float GetFVal(void)=0;
	virtual int GetIVal(void)=0;
	virtual void SetAutoScale(BOOL on=TRUE)=0;
	virtual void SetScale( float s )=0;
	virtual void SetValue( float v, BOOL bNotify )=0;
	virtual void SetValue( int v, BOOL bNotify )=0;
	virtual void SetLimits( int min, int max, BOOL bLimitCurValue = TRUE )=0;
	virtual void SetLimits( float min, float max, BOOL bLimitCurValue = TRUE )=0;
	virtual void LinkToEdit( HWND hEdit, ICustomControl::EditSpinnerType type )=0;
	virtual void SetIndeterminate(BOOL i=TRUE)=0;
	virtual BOOL IsIndeterminate(void)=0;
	virtual void SetResetValue(float v)=0;
	virtual void SetResetValue(int v)=0;
	virtual void SetKeyBrackets(BOOL onOff)=0;
	virtual void SetSnapSpinner(BOOL b)=0;
	virtual BOOL GetSnapSpinner(void)=0;
	virtual void SetSnapSpinValue(float f)=0;
	virtual float GetSnapSpinValue(void)=0;
	virtual void SetSpinnerFormat(const char *pFormat)=0;
	virtual const char *GetSpinnerFormat(void)=0;
	virtual void SetSpinnerWrap(BOOL b)=0;
	virtual BOOL GetSpinnerWrap(void)=0;
	virtual void SetDragNotify(BOOL b)=0;
	virtual BOOL GetDragNotify(void)=0;

	static ICustomSpinner *GetISpinner( HWND hCtrl );
	static void ReleaseISpinner( ICustomSpinner *isc );
	static ICustomSpinner *SetupIntSpinner(HWND hwnd, int idSpin, int idEdit,  int min, int max, int val,int scale);
	static ICustomSpinner *SetupFloatSpinner(HWND hwnd, int idSpin, int idEdit,  float min, float max, float val, float scale, const char *pFormat);
};



} // AttrControls

#endif
