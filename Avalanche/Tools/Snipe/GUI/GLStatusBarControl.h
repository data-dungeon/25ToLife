#include "GLProgressControl.h"
#include "GLRenderWnd.h"
#include "KeyModifiers.h"
#include "MouseStates.h"

class CGLStatusBarControl : public CGLTransparentControl
{
public:
   //CGLProgressControl m_glProgressControl;

   CGLStatusBarControl();

// CGLControl overrides
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
//   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};

class CGLStatusPaneControl : public CGLControl
{
public:
   const char  *m_pText;
   GLImageList  m_glImageListIcon;
   int          m_nIconIndex;
   GLImageList  m_glImageListBackground;

   CGLStatusPaneControl();

   BOOL SetIcon( UINT nBitmapID, int nIconWidth=0, int nIndex=0 );
   BOOL SetText( const char *pText );

// CGLControl overrides
   virtual void PostInit();
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};

class CGLKeyModifierPaneControl : public CGLControl
{
public:
   KeyModifiers m_keymodifiers;
   GLImageList  m_glImageListIcon;
   int          m_nIconIndex;
   GLImageList  m_glImageListBackground;

   CGLKeyModifierPaneControl();

   BOOL SetIcon( UINT nBitmapID, int nIconWidth=0, int nIndex=0 );
   BOOL SetKeyModifiers( KeyModifiers keymodifiers );

   const char *GetText( KeyModifiers nKeyModifier ) const;
   BOOL IsActive( KeyModifiers nKeyModifier ) const;

   // CGLControl overrides
   virtual void PostInit();
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};

class CGLStatusBarWnd : public CGLRenderWnd
{
public:
   CGLStatusBarControl m_glStatusBarControl;
   
   DECLARE_MESSAGE_MAP()
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

