#pragma once
#include "GLEditControl.h"
#include "GLButtonControl.h"

class CGLBrowseEditControl : public CGLTransparentControl
{
private:
   CGLFileNameEditControl m_glFileNameEditControl;
   CGLButtonControl m_glBrowseButton;
   String m_strFilters;

public:
   union {
      ULONG m_nBrowseEditControlFlags;
      struct {
         ULONG m_bOpenDialog : 1;
      };
   };

   CGLBrowseEditControl()
   {
      m_nBrowseEditControlFlags = 0;
   }
   virtual ~CGLBrowseEditControl()
   {
   }

   void SetReadOnly( BOOL bReadOnly ) { m_glFileNameEditControl.SetReadOnly( bReadOnly ); }
   BOOL IsReadOnly() const { return m_glFileNameEditControl.IsReadOnly(); }

   void SetFilters( const String &strFilters ) { m_strFilters = strFilters; }
   String GetFilters() const { return m_strFilters; }

   void SetText( const String &strText, BOOL bNotify );

// Overridables
   virt_base virtual void GetText( String &strValue ) const;
   virt_base virtual void OnBrowse();

// CGLControl Overrides
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};


