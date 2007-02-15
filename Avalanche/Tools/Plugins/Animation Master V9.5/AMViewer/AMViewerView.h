// Dan  4/11/00  \Dan85\AMViewer\AMViewerView.h

#include "AM-GL.h"
#ifndef __VECTOR2_H
#include "Vector2.h"
#endif
#ifndef __MATRIX34_H
#include "Matrix34.h"
#endif
#ifndef __SPLITPAT_H
#include "SplitPat.h"
#endif
#ifndef __HASHTIME_H
#include "HashTime.h"
#endif

class CAMViewerView;
class CAMViewerDoc;
class Patch;
class PatchModel;
class MyPatchSplitter : public PatchSplitter 
{
public:
   CAMViewerView *m_view;
   Patch *m_origpatch;
   PatchModel *m_model;
   int m_numpatches;
   int m_drawnlast;
   BOOL m_hadtexture;
   GLuint m_texture;

   virtual void SubdivideCallback(SplitPatch &patch);
};

#define MAXSUB 8

class Bone;
class CAMViewerView : public CView
{
protected: // create from serialization only
	DECLARE_DYNCREATE(CAMViewerView)

   enum Mode {MOVE, TURN, ZOOM};

#ifdef _MAC
   AGLContext    m_context;
#else
   HGLRC m_hrc;
#endif

   HPALETTE m_hpalette;
   CPoint m_mouse, m_center;
   Vector m_rotate;
   Vector m_offset;
   float m_zoom, m_focallength, m_focaldistance;
   float m_span, m_backspan, m_frontspan;
   Time m_time, m_lastnonfloortime;
   int m_cursorid;
   int m_width, m_height;
   Matrix34 m_matrix;
   Mode m_mode;
   MyPatchSplitter m_patchsplitter;
   DWORD m_prevtime;

   unsigned long m_bshaded      : 1,
                 m_bvertices    : 1,
                 m_btangents    : 1,
                 m_bnormals     : 1,
                 m_bdecals      : 1,
                 m_bmaterials   : 1,
                 m_bbilinear    : 1,
                 m_bmousedown   : 1,
                 m_bskipframes  : 1,
                 m_bplay        : 1,
                 m_bperspective : 1,
                 m_bsubdivided  : 1,
				 m_bshowbones	: 1;

	CAMViewerView();

public:
	CAMViewerDoc* GetDocument();

   void Render();
   void RenderNormals();
   void RenderVertices();
   void RenderPatches();
   void RenderTangents();

   void SetOpenGLPalette();
   void ZoomFit();

   void StartTurn();
   void DoTurn();

   void StartZoom();
   void DoZoom();
   void StartBoundZoom();
   void DoBoundZoom();
   void EndBoundZoom();

   void StartMove();
   void DoMove();

   BOOL SetPlaying(BOOL play);
   BOOL IsPlaying() { return m_bplay; }
 
   void ComputeSpan();
   void TransformVertices();
   void TransformBoneVertices(Bone *bone, const Matrix34 &parentscreenmatrix);
   void Project(Vector &p);
   void Project(Vector2 &p);
   void Unproject(Vector &p);
   void UnprojectNoNormalize(Vector &p);
   void Unproject(Vector2 &p);
   void InitView();
   Time GetCurrentPlayTime(BOOL store = TRUE);
   BOOL TimerProc();
   BOOL MyCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
   void SetupForBlit();

   void UpdateSubLevelStats();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAMViewerView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

	virtual ~CAMViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CAMViewerView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg BOOL OnQueryNewPalette();
   afx_msg void OnPaletteChanged(CWnd *);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMove();
   afx_msg void OnUpdateMove(CCmdUI* pCmdUI);
   afx_msg void OnTurn();
   afx_msg void OnUpdateTurn(CCmdUI* pCmdUI);
   afx_msg void OnZoom();
   afx_msg void OnUpdateZoom(CCmdUI* pCmdUI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
   afx_msg void OnZoomFit();
   afx_msg void OnFrontView();
   afx_msg void OnBackView();
   afx_msg void OnLeftView();
   afx_msg void OnRightView();
   afx_msg void OnTopView();
   afx_msg void OnBottomView();
   afx_msg void OnUpdatePolyMode(CCmdUI* pCmdUI);
   afx_msg void OnPolyMode();
   afx_msg void OnUpdateSubdividedMode(CCmdUI* pCmdUI);
   afx_msg void OnSubdividedMode();
   afx_msg void OnUpdateShaded(CCmdUI* pCmdUI);
   afx_msg void OnShaded();
   afx_msg void OnUpdateWireframe(CCmdUI* pCmdUI);
   afx_msg void OnWireframe();
   afx_msg void OnUpdateVertices(CCmdUI* pCmdUI);
   afx_msg void OnVertices();
   afx_msg void OnUpdateTangents(CCmdUI* pCmdUI);
   afx_msg void OnTangents();
   afx_msg void OnUpdateNormals(CCmdUI* pCmdUI);
   afx_msg void OnNormals();
   afx_msg void OnUpdateMaterials(CCmdUI* pCmdUI);
   afx_msg void OnMaterials();
   afx_msg void OnUpdateDecals(CCmdUI* pCmdUI);
   afx_msg void OnDecals();
   afx_msg void OnUpdatePerspective(CCmdUI* pCmdUI);
   afx_msg void OnPerspective();
   afx_msg void OnUpdateOrthogonal(CCmdUI* pCmdUI);
   afx_msg void OnOrthogonal();
   afx_msg void OnOptions();
   afx_msg void OnUpdatePlay(CCmdUI* pCmdUI);
   afx_msg void OnPlay();
	afx_msg void OnNextframe();
	afx_msg void OnUpdateNextframe(CCmdUI* pCmdUI);
	afx_msg void OnPrevframe();
	afx_msg void OnUpdatePrevframe(CCmdUI* pCmdUI);
	afx_msg void OnLastframe();
	afx_msg void OnFirstframe();
	afx_msg void OnIncreaseSub();
	afx_msg void OnUpdateIncreaseSub(CCmdUI* pCmdUI);
	afx_msg void OnDecreaseSub();
	afx_msg void OnUpdateDecreaseSub(CCmdUI* pCmdUI);
	afx_msg void OnMaxSub();
	afx_msg void OnUpdateMaxSub(CCmdUI* pCmdUI);
	afx_msg void OnMinSub();
	afx_msg void OnUpdateMinSub(CCmdUI* pCmdUI);
	afx_msg void OnFixSub();
	afx_msg void OnUpdateFixSub(CCmdUI* pCmdUI);
	afx_msg void OnShowBones();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AMViewerView.cpp
inline CAMViewerDoc* CAMViewerView::GetDocument()
   { return (CAMViewerDoc*)m_pDocument; }
#endif

inline void CAMViewerView::Project(Vector &p)
{
   if (m_bperspective) {
      if (p.z > -1)
         return;

      float perspective = p.z/-m_focallength;
      p.x = p.x/perspective/m_center.x*m_zoom;
      p.y = p.y/perspective/m_center.y*m_zoom;
   }
   else {
      p.x = p.x * m_zoom / m_center.x;
      p.y = p.y * m_zoom / m_center.y;
   }
   p.z = (p.z-m_frontspan)/m_span;
}

inline void CAMViewerView::Unproject(Vector &p)
{
   p.z = p.z * m_span + m_frontspan;

   if (m_bperspective) {
      float perspective = p.z/-m_focallength;
      p.x = p.x/m_zoom*m_center.x*perspective;
      p.y = p.y/m_zoom*m_center.y*perspective;
   }
   else {
      p.x = p.x * m_center.x / m_zoom;
      p.y = p.y * m_center.y / m_zoom;
   }
}

inline void CAMViewerView::UnprojectNoNormalize(Vector &p)
{
   if (m_bperspective) {
      float perspective = p.z/-m_focallength;
      p.x = p.x/m_zoom*perspective;
      p.y = p.y/m_zoom*perspective;
   }
   else {
      p.x /= m_zoom;
      p.y /= m_zoom;
   }
}

inline void CAMViewerView::Project( Vector2 &p )
{
   p.x = ((p.x + m_offset.x) * m_zoom)/m_center.x;
   p.y = ((p.y + m_offset.y) * m_zoom)/m_center.y;
}

inline void CAMViewerView::Unproject( Vector2 &p )
{
   p.x = p.x*m_center.x/m_zoom - m_offset.x;
   p.y = p.y*m_center.y/m_zoom - m_offset.y;
}


/////////////////////////////////////////////////////////////////////////////
