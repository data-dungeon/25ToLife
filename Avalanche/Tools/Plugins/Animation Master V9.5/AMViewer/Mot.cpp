// Dan  4/11/00  \Dan85\AMViewer\Mot.cpp

#include "stdafx.h"
#include "Mot.h"
#include "Archive.h"
#include "Model.h"

#define MOTVERSION  0x12345678+1

void MotAction::Serialize(CArchive& ar)
{
   int version;
   WORD bonecount;

// Header Section
   ar >> version;

   if (version != MOTVERSION) {
      CString text;
      text.Format("Invalid motion file %s", ar.GetFile()->GetFileTitle());
      AfxMessageBox(text, MB_ICONEXCLAMATION);
      AfxThrowUserException();
   }

   CString modelname = ReadString(ar);
   WORD frames;
   ar >> frames;
   m_length.SetCell(frames, 30);// MOT's hardcoded to 30 FPS for this demo
   ar >> bonecount;

// Bone Section
   m_bonelist.RemoveAll();
   m_bonelist.SetSize(bonecount);
   for (int i=0; i<bonecount; i++) {
      MotActionBone &bone = m_bonelist[i];
      bone.m_name = ReadString(ar);
      bone.m_matrixlist.SetSize(frames);
      for (int l=0; l < frames; l++)
         ar >> bone.m_matrixlist[l];
   }  
}

void MotAction::Apply(Bone *bone, const Time &time, BOOL newaction)
{
   int frame = (int)time.GetFrame(30); //MOT hard coded to 30 FPS
   for (;bone;bone=bone->m_sibling) {
      for (int i=0; i < m_bonelist.GetSize(); i++) {
         if (m_bonelist[i].m_name.CompareNoCase(bone->m_name)==0) {
            bone->m_matrix = m_bonelist[i].m_matrixlist[frame];
            break;
         }
      }
      if (bone->m_child)
         Apply(bone->m_child, time, newaction);
   }
}