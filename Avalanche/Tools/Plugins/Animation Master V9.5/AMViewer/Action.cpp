 // Dan  4/11/00  \Dan85\AMViewer\Action.cpp

#include "stdafx.h"
#include "MainFrm.h"
#include "Archive.h"
#include "AMViewerDoc.h"
#include "Model.h"
#include "Mot.h"
#include "KFM.h"

Action *CAMViewerDoc::LoadAction(const CString &filename)
{
   CString ext;

   ext = filename.Mid(filename.ReverseFind('.')+1);
   ext.MakeLower();

   Action *action;
   
   if (ext == "mot")
      action = new MotAction;
   else if (ext == "kfm")
      action = new KFMAction;
   else {
      CString text;
      text.Format("Unknown file format %s.", filename);
      AfxMessageBox( text, MB_OK );
      return NULL;
   }

   CFile file;
   CFileException fileexception;
 
   if (!file.Open( filename, CFile::modeRead, &fileexception)) {
      CString text;
      text.Format("Can't open file %s, error = %u\n", filename, fileexception.m_cause);
      AfxMessageBox( text, MB_OK );
      return NULL;
   }
  
   CArchive ar(&file, CArchive::load);
   TRY {
      CWaitCursor wait;
      if (file.GetLength() != 0)
         action->Serialize(ar);
      ar.Close();
      file.Close();
   }
   CATCH(CUserException, e) {
      ar.Close();
      file.Close();
      delete action;
      return FALSE;
   }
   AND_CATCH_ALL(e) {
      ar.Close();
      file.Close();
      delete action;
      e->ReportError();
      return FALSE;
   }
   END_CATCH_ALL

   action->m_filename = filename;
   action->m_name = filename.Mid(filename.ReverseFind(PATHSEP)+1);//Remove Path
   return action;     
}

BOOL CAMViewerDoc::ApplyAction(const Time &time)
{
   if (time == m_lasttime)
      return FALSE;

   m_lasttime = time;

   RemoveActions(m_model->m_bonehead);
   static Model *lastmodel = NULL;
   Time nexttime;
   for (POSITION pos = m_actionlist.GetHeadPosition(); pos; ) {
      Action *action = m_actionlist.GetNext(pos);
      if (action->m_isactive) {
         nexttime += action->m_length;
         if (time <= nexttime) {
            m_currentactiontime = action->m_length-(nexttime-time);
            action->Apply(m_model->m_bonehead, m_currentactiontime, m_currentaction!=action || m_model!=lastmodel);
            lastmodel = m_model;
            m_currentaction = action;
            return TRUE;
         }
      }
   }
   m_currentaction = NULL;
   m_currentactiontime = 0;
   return TRUE;
}

void CAMViewerDoc::RemoveActions(Bone *bone)
{
   for (;bone;bone=bone->m_sibling) {
      bone->m_matrix = Identity34();
      if (bone->m_child)
         RemoveActions(bone->m_child);
   }
}

