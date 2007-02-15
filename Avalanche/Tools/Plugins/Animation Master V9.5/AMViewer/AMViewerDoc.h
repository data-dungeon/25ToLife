// Dan  4/11/00  \Dan85\AMViewer\AMViewerDoc.h

#include "Afxtempl.h"
#include "Action.h"

class Model;
class Vector2;
class Vector;
class Vector4;
class Matrix34;

#ifdef _MAC
   #define PATHSEP ':'
#else
   #define PATHSEP  '\\'
#endif

class CAMViewerDoc : public CDocument
{
public:
   static int m_fps;
   CList<Action *, Action *>m_actionlist;
   Model *m_model;
   Time m_scriptlength;
   Time m_lasttime;
   Time m_currentactiontime;
   Action *m_currentaction;
#ifdef _MAC
   DWORD m_elapse;
#else
   LARGE_INTEGER m_elapse;
#endif

   virtual ~CAMViewerDoc();

   virtual void DeleteContents();

   void SetModel(Model *model);

   Action *LoadAction(const CString &filename);
   BOOL ApplyAction(const Time &time);
   void AddAction(Action *action);
   void RemoveAction(Action *action);
   void RemoveActions(Bone *bone);

   void SetStatusBarStats();

   BOOL LoadModel(const CString &filename);

   //{{AFX_VIRTUAL(CAMViewerDoc)
	public:
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CAMViewerDoc();
	DECLARE_DYNCREATE(CAMViewerDoc)

	//{{AFX_MSG(CAMViewerDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

