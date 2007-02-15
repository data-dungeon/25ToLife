//

#ifndef __POSETREE_H
#include "PoseTree.h"
#endif

class CPropertyWnd : public CWnd
{
public:
   static UINT           m_hitflags, m_hitid, m_timerid;
   static BOOL           m_isnoupdate;
   static CPoint         m_ptHotSpot;
   static CPoint         m_ptDown;

   HProperty *m_property;
   PropertyExt *m_propertyext;
   CPoseTree *m_tree;

                  CPropertyWnd( HProperty *property, CPoseTree *tree, CWnd *parent, const CRect &rect );
   virtual       ~CPropertyWnd();

   virtual int GetDefaultHeight() { return 18; };

//   virtual HashPtrObject **GetPtrAt( int index );
//   virtual BOOL   OnPtrMessage( PtrMessage ptrmessage, HashPtrObject **ptruplink, LONG lparam, Time time=0 );

virtual HTreeObject *HitTestTreeItem(const CPoint &pt, const CRect &rcItem, UINT &nFlags, UINT &nID);
virtual UINT   HitTest( CPoint pt, UINT &nFlags );
//   virtual BOOL   OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo );

   virtual int  GetTreeItemControlWidth(CDC *dc) { return 0; }
   virtual void DrawProperty(CDC *pDC, const CRect &rect);
   virtual void DrawTextLabel( CDC *dc, CRect &rcText, BOOL setcolor=TRUE, BOOL fullname=FALSE );
   virtual void DrawValue( CDC *dc, const CRect &rcItem, const String &value );

private:
   virtual int  OnToolHitTest( CPoint point, TOOLINFO* pTI ) const;

   afx_msg void   OnPaint();
   afx_msg void   OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void   OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void   OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void   OnMButtonDown(UINT nFlags, CPoint point);
   afx_msg void   OnRButtonDown(UINT nFlags, CPoint point);
   afx_msg void   OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL   OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
   afx_msg void   OnTimer( UINT timerid );
   DECLARE_MESSAGE_MAP()
};

class CPercentWnd : public CPropertyWnd
{
public:
   CPercentWnd( HProperty *property, CPoseTree *tree, CWnd *parent, const CRect &rect ) :
      CPropertyWnd(property, tree, parent, rect) {}

   virtual int GetDefaultHeight() { return 28; };

   virtual void DrawProperty(CDC *pDC, const CRect &rect);
   virtual int GetTreeItemControlWidth(CDC *dc);
private:

   DECLARE_MESSAGE_MAP()
};

class CBoolWnd : public CPropertyWnd
{
public:
   CBoolWnd( HProperty *property, CPoseTree *tree, CWnd *parent, const CRect &rect ) :
      CPropertyWnd(property, tree, parent, rect) {}

   virtual void DrawProperty(CDC *pDC, const CRect &rect);
   virtual int GetTreeItemControlWidth(CDC *dc);
private:
 
   DECLARE_MESSAGE_MAP()
};

class CUserCategoryWnd : public CPropertyWnd
{
public:
   CUserCategoryWnd( HProperty *property, CPoseTree *tree, CWnd *parent, const CRect &rect ) :
      CPropertyWnd(property, tree, parent, rect) {}

private:

   DECLARE_MESSAGE_MAP()
};
