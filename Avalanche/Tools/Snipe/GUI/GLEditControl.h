#pragma once
#include "GLScrollBarControl.h"

class CGLEditControlBase : public CGLScrollableControl
{
public:
   UINT m_bCenterText       : 1,
        m_bReadOnly         : 1,
        m_bKillFocusOnEnter : 1,
        m_bMultiLine        : 1;

            CGLEditControlBase();
   virtual ~CGLEditControlBase();

   BOOL IsReadOnly() const { return m_bReadOnly; };
   void SetReadOnly( BOOL bReadOnly ) { m_bReadOnly = bReadOnly; }

   void SetText( const String &strText, BOOL bNotify );

// Overridables
   virt_base virtual void StoreText( const String &strText ) = 0;
   virt_base virtual void GetText( String &strText ) const = 0;
   virt_base virtual void GetAbbreviatedText( String &strText ) const { GetText( strText ); }
   virt_base virtual GLImageList *GetImageList() const { return NULL; }
   virt_base virtual void AdjustEditRect( CRect &rcEdit ) const { if (m_glVScroll.IsVisible()) rcEdit.right += m_glVScroll.m_szControl.cx; }
   virt_base virtual void OnScrub( int nPixels, BOOL bStoreUndo = FALSE ) {}
   virt_base virtual void OnEndScrub() {}
   virt_base virtual COLORREF GetBkColor() const { return RGB(255,255,255); }
   virt_base virtual RGBAFloat GetTextColor();
   virt_base virtual void CreatePopupEdit();
   virt_base virtual CWnd *GetPopupEdit();

// CGLControl overrides
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual BOOL OnLButtonDown( UINT nFlags, CPoint point );
   virtual BOOL OnLButtonUp( UINT nFlags, CPoint point );
   virtual BOOL OnMouseMove( UINT nFlags, CPoint point );
   virtual UINT GetToolTipText( CPoint point, String &strText ) const;
};

class CGLEditControl : public CGLEditControlBase
{
private:
   static GLImageList m_glImageListBackground;

   String m_strText;

public:
// CGLEditControlBase overrides
   virtual void StoreText( const String &strText );
   virtual void GetText( String &strValue ) const;
   virtual GLImageList *GetImageList() const { return &m_glImageListBackground; }
   virtual void AdjustEditRect( CRect &rcEdit ) const { rcEdit.DeflateRect( 2, 2 ); CGLEditControlBase::AdjustEditRect( rcEdit ); }

// CGLControl overrides
   virtual void PostInit();
};

class CGLFileNameEditControl : public CGLEditControl
{
private:
   String m_strAbbreviated;

public:
// CGLEditControlBase overrides
   virtual void StoreText( const String &strText );
   virtual void GetAbbreviatedText( String &strText ) const;

//CGLControl
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
};