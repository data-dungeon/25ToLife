// Dan  4/11/00  \Dan85\AMViewer\Action.h

#ifndef __ACTION_H
#define __ACTION_H

#ifndef __HASHTIME_H
#include "HashTime.h"
#endif

class Bone;
class Action
{
public:
   CString m_name;
   CString m_filename;
   Time    m_length;
   union { 
      WORD  m_flags;
      struct {
         WORD m_isactive : 1;
      };
   };
   Action() { m_flags = 0; }
   virtual ~Action() {}
   virtual void Serialize(CArchive &ar)=0;
   virtual void Apply(Bone *bone, const Time &time, BOOL newaction)=0;
};

#endif