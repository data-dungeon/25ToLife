#pragma once
#include "GLBaseFrame.h"
#include "GLComboControl.h"

class CGLHeaderView;

class CGLHeaderFrame : public CGLBaseFrame
{
   DECLARE_DYNCREATE( CGLHeaderFrame )
public:
   enum { HEADERHEIGHT=15, BUTTONWIDTH=18 };

// Attributes
   CGLHeaderControl  m_glHeaderControl;
   CGLButtonControl  m_glMaximizeButton;
   CGLButtonControl  m_glRestoreButton;
   int               m_nNextControlPos;

            CGLHeaderFrame();
   virtual ~CGLHeaderFrame();

   void PostInitGuts();
   void AddHeaderControl( CGLControl *pControl, const CPoint &ptControlOffset, const CSize &szControl, UINT nID );

// CGLBaseFrame overrides
   virtual void UpdateHeader();

// GLControl overrides
   virtual void PostInit();
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
   virtual BOOL OnLButtonDown( UINT nFlags, CPoint point );
};

