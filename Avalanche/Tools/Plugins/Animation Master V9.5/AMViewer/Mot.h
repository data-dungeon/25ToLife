// Dan  4/11/00  \Dan85\AMViewer\Mot.h

#define __MOT_H

#ifndef __ACTION_H
#include "Action.h"
#endif
#ifndef __MATRIX34_H
#include "Matrix34.h"
#endif
#include "Afxtempl.h"

class MotActionBone
{
public:
   CString m_name;
   CArray<Matrix34, Matrix34 &>m_matrixlist;
};

class MotAction : public Action
{
public:
   CArray<MotActionBone, MotActionBone &>m_bonelist;

   virtual void Serialize(CArchive &ar);
   virtual void Apply(Bone *bone, const Time &time, BOOL newaction);
};
