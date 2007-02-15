#pragma once
#include "GLEditControl.h"

class CGLHotKeyControl : public CGLEditControl
{
public:
            CGLHotKeyControl();
   virtual ~CGLHotKeyControl();

// CGLEditControl overrides

// CGLEditControlBase overrides
   virtual void CreatePopupEdit();
   virtual CWnd *GetPopupEdit();
};
