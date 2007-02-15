#pragma once

#include "GLControls.h"

class CGLProgressControl : public CGLControl
{
private:
   static GLImageList m_glImageListProgress;

   int  m_nPixelPos;
   int  m_nPos;
   int  m_nStep;
   int  m_nMax;
   UINT m_bAutoHide : 1;

public:
            CGLProgressControl( BOOL bAutoHide=FALSE );
   virtual ~CGLProgressControl();

   int GetPos() const { return m_nPos; }
   int GetStep() const { return m_nStep; }
   int GetMax() const { return m_nMax; }

   void SetPixelPos( int nPixelPos );
   void SetPos( int nPos );
   void SetStep( int nStep );
   void SetMax( int nMax );

   void Start( const String &strMessage, int nTotalItems, int nStep=1 );
   void Step();
   void Stop();

// CGLControl overrides
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};
