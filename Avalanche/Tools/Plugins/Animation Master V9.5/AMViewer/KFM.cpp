//
#include "stdafx.h"
#include "KFM.h"
#include "Archive.h"
#include "Model.h"
#include "Matrix34.h"

#define KFMVERSION  0x12345678+1

void KFMAction::Serialize(CArchive& ar)
{
   int version;
   WORD channelcount;

// Header Section
   ar >> version;

   if (version != KFMVERSION) {
      CString text;
      text.Format("Invalid motion file %s", ar.GetFile()->GetFileTitle());
      AfxMessageBox(text, MB_ICONEXCLAMATION);
      AfxThrowUserException();
   }

   float seconds;
   ar >> seconds;
   m_length.SetSeconds(seconds);

   ar >> channelcount;

// Channel Section
   m_channellist.RemoveAll();
   m_channellist.SetSize(channelcount);
   for (int i=0; i<channelcount; i++) {
      Channel &channel = m_channellist[i];
      channel.m_name = ReadString(ar);
      Vector bonedirection, rolldirection,
       parentbonedirection, parentrolldirection;
      ar >> bonedirection >> rolldirection;
      ar >> parentbonedirection >> parentrolldirection;
      channel.ComputeParentToBone(bonedirection, rolldirection, 
       parentbonedirection, parentrolldirection);
      for (int chanitemindex=0; chanitemindex < Channel::NUMCHANNELITEMS; chanitemindex++) {
         ChannelItem *chanitem = &channel.m_channelitem[chanitemindex];
         WORD numkf;
         ar >> numkf;
         chanitem->m_keyframes.SetSize(numkf);
         float seconds;
         for (int kfindex=0; kfindex < numkf; kfindex++) {
            Keyframe &kf = chanitem->m_keyframes[kfindex];
            ar >> seconds;
            kf.m_time.SetSeconds(seconds);
            ar >> kf.m_value;
            ar >> kf.m_flags;
            if (!kf.m_defaulttangents) {
               ar >> kf.m_it;
               ar >> kf.m_ot;
            }
         }
         // build default tangents now
         for (kfindex=0; kfindex < numkf; kfindex++) {
            Keyframe &kf = chanitem->m_keyframes[kfindex];
            if (kf.m_defaulttangents)
               chanitem->CalcTangents(kfindex);
         }
      }
   }
}

void KFMAction::Apply(Bone *bone, const Time &time, BOOL newaction)
{
   for (;bone;bone=bone->m_sibling) {
      for (int i=0; i < m_channellist.GetSize(); i++) {
         if (m_channellist[i].m_name.CompareNoCase(bone->m_name)==0) {
            if (newaction)
               bone->ComputeStandardToAction(m_channellist[i].m_bonetoparent);
            TSQ tsq =  m_channellist[i].GetTSQ(time);
            bone->m_matrix = bone->CreateMatrix(tsq);
            break;
         }
      }
      if (bone->m_child)
         Apply(bone->m_child, time, newaction);
   }
}


