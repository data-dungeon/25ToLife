// NAP  2/7/2002  \Noel90\AMViewer\AMViewerDoc.cpp

#include "stdafx.h"
#include "StrClass.h"
#include "AMViewerDoc.h"
#include "Vector.h"
#include "F_Protos.h"
#include <Direct.h>
#include "AMViewer.h"
#include "AMViewerView.h"
#include "Model.h"
#include "MainFrm.h"
#include "ScriptView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CAMViewerApp theApp;
extern CMainFrame *g_mainframe;

int CAMViewerDoc::m_fps;

/////////////////////////////////////////////////////////////////////////////
// CAMViewerDoc

IMPLEMENT_DYNCREATE(CAMViewerDoc, CDocument)

BEGIN_MESSAGE_MAP(CAMViewerDoc, CDocument)
	//{{AFX_MSG_MAP(CAMViewerDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAMViewerDoc construction/destruction

CAMViewerDoc::CAMViewerDoc()
{
   theApp.m_doc = this;
   m_fps = AfxGetApp()->GetProfileInt( "General", "FPS", 30 );
   m_scriptlength=0;
   m_model = NULL;
   m_lasttime.SetTagged(TRUE);
   m_currentaction = NULL;
   m_currentactiontime = 0;
#ifdef _MAC
   m_elapse = 0;
#else
   m_elapse.QuadPart = 0;
#endif
}

CAMViewerDoc::~CAMViewerDoc()
{
   theApp.m_doc = NULL;

   while (!m_actionlist.IsEmpty()) {
      Action *action = m_actionlist.RemoveHead();
      delete action;
   }

   delete m_model;

   AfxGetApp()->WriteProfileInt( "General", "FPS", m_fps );
}

void CAMViewerDoc::DeleteContents()
{
   if (g_mainframe) {
      if (g_mainframe->m_scriptview) 
         g_mainframe->m_scriptview->DeleteContents();
      g_mainframe->SetRenderedFPS("");
      g_mainframe->SetActionFrame("");
      g_mainframe->SetScriptFrame("");
      g_mainframe->SetStatusMessage("");
      g_mainframe->SetSubLevel("");
      g_mainframe->SetPatchPolyCount("");
   }

    delete m_model;
    m_model = NULL;

   while (!m_actionlist.IsEmpty()) {
      Action *action = m_actionlist.RemoveHead();
      delete action;
   }

   m_scriptlength = 0;
   m_lasttime.SetTagged(TRUE);
   m_currentaction = NULL;
   m_currentactiontime = 0;
#ifdef _MAC
   m_elapse = 0;
#else
   m_elapse.QuadPart = 0;
#endif
 }

/////////////////////////////////////////////////////////////////////////////
// CAMViewerDoc serialization

void CAMViewerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
      SetBasePath((LPCTSTR)ar.GetFile()->GetFilePath());
      ar << "HSF"; // HEADER
      ar << (float)1.0; // VERSION
      if (m_model) {
         String modelname = m_model->m_name; // use our own string class that does relative file saving.
         ar << modelname.GetRelativePath().Get();
      }
      else
         ar << "No Model";
      ar << m_fps;
      ar << m_actionlist.GetCount();
      for (POSITION pos = m_actionlist.GetHeadPosition(); pos;) {
         Action *action = m_actionlist.GetNext(pos);
         String actionname = action->m_filename;
         ar << actionname.GetRelativePath().Get();
      }
	}
	else
	{
      SetBasePath((LPCTSTR)ar.GetFile()->GetFilePath());

      CString header;
      ar >> header;

      if (header != "HSF") {
         CString text;
         text.Format("Invalid Script file : %s", ar.GetFile()->GetFileTitle());
         AfxMessageBox(text, MB_ICONEXCLAMATION);
         AfxThrowUserException();
      }

      float version;
      ar >> version;

      CString name;
      String filename; // use our own string class to get absolute path
      ar >> name;
      filename = (LPCTSTR)name;
      name=filename.GetAbsolutePath().Get();
      if (name != "No Model")
         LoadModel(name);

      ar >> m_fps;

      int count;
      ar >> count; // action count
      for (int i=0; i < count; i++) {
         ar >> name;
         filename = (LPCTSTR)name;
         Action *action = LoadAction(filename.GetAbsolutePath().Get());
         if (action) {
            AddAction(action); // adds to CList
            g_mainframe->m_scriptview->AddActionToListCtrl(action);
         }
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// CAMViewerDoc diagnostics


#ifdef _DEBUG
void CAMViewerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAMViewerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void CAMViewerDoc::SetModel(Model *model)
{
   delete m_model;
   m_model = model;

   CString strcount;
   if (dynamic_cast<PatchModel *>(m_model))
      strcount.Format("%d Patches", m_model->GetPatchPolyCount());
   else
      strcount.Format("%d Polys", m_model->GetPatchPolyCount());
   g_mainframe->SetPatchPolyCount(strcount);
}

void CAMViewerDoc::AddAction(Action *action)
{
   m_actionlist.AddTail(action);
}

void CAMViewerDoc::RemoveAction(Action *action)
{
   POSITION pos = m_actionlist.Find(action);
   if (pos)
	   m_actionlist.RemoveAt(pos);
}

void CAMViewerDoc::SetStatusBarStats()
{
   CString text;
   if (m_model &&
#ifdef _MAC
      m_elapse) {
#else
      m_elapse.QuadPart) {
#endif

#ifdef _MAC
      DWORD frequency = TICKSPERSEC;
      float seconds = (float)m_elapse / frequency;
#else
      LARGE_INTEGER frequency;
      QueryPerformanceFrequency(&frequency);
      float seconds = (float)m_elapse.QuadPart / frequency.QuadPart;
#endif
      if ( seconds > 1.0 )
         text.Format("%.2f Seconds Per Frame", seconds);
      else
         text.Format("%.1f FPS", 1.0f/seconds);
text.Format("%.2f = %.1f fps", seconds*1000, 1.0f/seconds);
//text.Format("%d", frequency.LowPart);
      g_mainframe->SetRenderedFPS(text); 
   }

   if (m_currentaction)
      text.Format("%s: %s/%s",m_currentaction->m_name, m_currentactiontime.ToString(m_fps, SMPTE, FALSE).Get(), 
       m_currentaction->m_length.ToString(m_fps, SMPTE, FALSE).Get());
   else
      text = " ";
   g_mainframe->SetActionFrame(text); 

   if (m_scriptlength.GetSeconds())
      text.Format("%s/%s", g_mainframe->m_modelview->m_time.ToString(m_fps, SMPTE, FALSE).Get(), 
       m_scriptlength.ToString(m_fps, SMPTE, FALSE).Get());
   else
      text = " ";

   g_mainframe->SetScriptFrame(text); 

   g_mainframe->UpdateStatusBar();
}

