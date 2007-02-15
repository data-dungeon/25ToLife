// Dan  4/11/00  \Dan85\AMViewer\KFM.h

#define __KFM_H

#ifndef __CHANNEL_H
#include "Channel.h"
#endif
#ifndef __ACTION_H
#include "Action.h"
#endif

class KFMAction : public Action
{
public:
   CArray<Channel, Channel &>m_channellist;

   virtual void Serialize(CArchive &ar);
   virtual void Apply(Bone *bone, const Time &time, BOOL newaction);
};
