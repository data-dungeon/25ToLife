// Dan  7/7/2004  \DanLP110\Plugin\HXT\Avalanche\AnimationMaster\KeyPose\HashTree.h

#define __POSETREE_H

#ifndef __COLORBUF_H
#include "ColorBuf.h"
#endif
#ifndef __HASHTAB_H
#include "HashTab.h"
#endif
#ifndef __SCROLLBAR_H
#include "ScrollBar.h"
#endif

#define HTVHT_HASHOBJECT           0x00001000
#define HTVHT_PROPERTY             0x00002000
#define HTVHT_ONITEMPROPERTYBUTTON 0x00004000
#define HTVHT_ONPROPERTYCONTROL    0x00008000
#define HTVHT_HASCURSOR            0x00010000
#define HTVHT_NOTOOLTIPID          0x00020000
#define HTVHT_DRAGSPOT             0x00040000

#define SIZE_INDENT    10
#define SIZE_GAP       2
#define SIZE_EXPAND    10
#define SIZE_BK        3
#define SIZE_VARBK     5
#define SIZE_ICON      18
#define SIZE_VAREXPAND 11

class HTreeObject;
class String;
class PropertyExt;
class HProperty;
class CPropertyWnd;

class CPoseTree : public CWnd
{
   DECLARE_DYNCREATE(CPoseTree)
public:
   CFont          *m_font;
   CPen            m_textpen;
   int             m_xoffset, m_yoffset;
   int             m_indent;
   CSize           m_extents;
   HashScrollBar  *m_hscroll, *m_vscroll;
   HProperty      *m_startobj;
   
   union {
      ULONG          m_flags;
      struct {
         BITFIELDS32  m_needsrecalcscroll  : 1,
                      m_needsadjustprop    : 1,
                      m_ismmoving          : 1,
                      m_ishideproperties   : 1;
      };
   };

    CPoseTree();
   ~CPoseTree();

   BOOL Create( const RECT& rect, CWnd* pParentWnd, UINT nID );

   CPropertyWnd *CreatePropertyWnd(HProperty *property, CWnd *parent, const CRect &rc);
   void TotalRefresh();
   void ExpandProperties(HProperty *obj, BOOL expand=TRUE);
	void EnsureVisible(HProperty *obj);

   void Select(HProperty *property, BOOL select=TRUE, BOOL setfocus=TRUE);

   HProperty *GetArrowDownObject( HProperty *obj );
   HProperty *GetArrowUpObject( HProperty *obj );

   BOOL GetItemRect( HProperty *obj, CRect &rcItem );
   BOOL GetItemRectBranch( HProperty *find, CRect &rcItem, HProperty *obj );

   HProperty *HitTest( CPoint point, UINT &nFlags );
   HProperty *HitTestBranch( HProperty *obj, CPoint point, UINT &nFlags );

   CRect GetScrollRect();
   void RecalcScrollBars();
   void RecalcScrollBarsGuts();
   void ScrollVert( int amount );
   void RecalcExtents( HProperty *obj=NULL );

   void AdjustAllPropertyWnd( HProperty *prop=NULL );
   void RemoveAllPropertyWnd( HProperty *prop=NULL );

   void AdjustPropertyWnd( HProperty *property, CWnd *parent, const CRect &rcItem );
   void RemovePropertyWnd( HProperty *prop=NULL );

   BOOL IsShowPropertyTriangle();

   HProperty *GetStartObject() { return m_startobj; }
   HProperty *GetNextObject( HProperty *prev );
   BOOL OnKeyDown( UINT nChar );










   BOOL GetTreeItemRect( HProperty *property, CRect &rcItem, HProperty *find=NULL, HProperty *caller=NULL );

   afx_msg void OnMButtonDown(UINT nFlags, CPoint point);

protected:
// CWnd
	virtual BOOL PreTranslateMessage(MSG* pMsg);
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   virtual BOOL OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo );

   afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSize( UINT nType, int cx, int cy );
   afx_msg void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar *pScrollBar );
   afx_msg void OnVScroll( UINT nSBCode, UINT nPos, CScrollBar *pScrollBar );
   afx_msg void OnTimer( UINT timerid );
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
   afx_msg BOOL OnMouseWheel(UINT fFlags, short zDelta, CPoint point);
   afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
   afx_msg void OnNcPaint();
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   afx_msg void OnPaint();

   DECLARE_MESSAGE_MAP()
};

