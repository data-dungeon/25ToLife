// Dan  7/6/2004  \DanLP110\Plugin\HXT\Avalanche\AnimationMaster\KeyPose\HashTab.h

#define __HASHTAB_H

#ifndef __STRCLASS_H
#include "StrClass.h"
#endif
#include <afxtempl.h>

class HashTab 
{
public:
   String m_label;
   CRect  m_rect;
   int    m_id, m_lastvscroll;
   void  *m_data;

   HashTab()
   {
      m_data = NULL;
   }
   HashTab( const String &label, void *data )
   {
      m_label = label;
      m_data = data;
   }
};

typedef struct {
	NMHDR nmhdr;
	HashTab *tab;
} HASHTAB_NMHDR;

class CHashTabCtrl : public CWnd
{
public:
   CFont *m_font;
   CArray<HashTab, HashTab &> m_tabs;
   HashTab *m_curtab;
   
          CHashTabCtrl();
         ~CHashTabCtrl();

   BOOL   Create( const RECT& rect, CWnd* pParentWnd, UINT nID );
   int    AddTab( const String &label, void *data );
   void   RemoveTab( const String &label );
   void   SetActiveTab( int index );
   void   GetActiveTab( int index );
   int CHashTabCtrl::GetActiveTab();
   void   DrawTab( CDC *dc, CRect &rcTab, HashTab *tab );

protected:
   afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnPaint();
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};