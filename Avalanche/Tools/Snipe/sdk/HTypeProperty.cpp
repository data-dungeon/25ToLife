//

#include "stdafx.h"
#include "HTypeProperty.h"
#include "Properties/Property.h"

// Info
HTypeProperty::HInfo *HTypeProperty::HInfo::New( const String &strName, const String &strMatchName, HNode nodearray[], int nNodeCount, int defaultvalue)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HTypeProperty::HInfo*)SNEW TypeProperty::Info(strName, strMatchName, (TypeProperty::Info::Node *)nodearray, nNodeCount, defaultvalue);
}

void HTypeProperty::HInfo::SetDisplayAsRadios( BOOL bState )
{
   ((TypeProperty::Info *)this)->m_bDisplayAsRadios = bState;
}

BOOL HTypeProperty::HInfo::GetDisplayAsRadios()
{
   return ((TypeProperty::Info *)this)->m_bDisplayAsRadios;
}

HTypeProperty::HInfo::HNode *HTypeProperty::HInfo::AllocNodes( int nSize )
{
   return (HTypeProperty::HInfo::HNode *) SNEW TypeProperty::Info::Node[nSize];
}

HTypeProperty::HInfo::HNode *HTypeProperty::HInfo::GetNodeAt( HNode hnodearray[], int nIndex )
{
   TypeProperty::Info::Node *pNodeArray = (TypeProperty::Info::Node *)hnodearray;

   return (HTypeProperty::HInfo::HNode *) &pNodeArray[nIndex];

         void DeleteHNodeArray( HNode pArray[] );

}

void HTypeProperty::HInfo::HNode::operator delete[] (void *pVoid)
{
   TypeProperty::Info::Node *pArray = (TypeProperty::Info::Node *)pVoid;
   delete [] pArray;
}

// Node
const String &HTypeProperty::HInfo::HNode::GetName() const
{
   return ((TypeProperty::Info::Node *)this)->GetName();
}

void HTypeProperty::HInfo::HNode::SetName( const char *szName )
{
   return ((TypeProperty::Info::Node *)this)->SetName( szName );
}

const String &HTypeProperty::HInfo::HNode::GetMatchName() const
{
   return ((TypeProperty::Info::Node *)this)->GetMatchName();
}

void HTypeProperty::HInfo::HNode::SetMatchName( const char *szName )
{
   return ((TypeProperty::Info::Node *)this)->SetMatchName( szName );
}

// TypeProperty
HTypeProperty *HTypeProperty::New( HTypeProperty::HInfo *pInfo, HProperty *pParent )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   TypeProperty *pProperty = TypeProperty::Create();
   pProperty->Init( (TypeProperty::Info *)pInfo, (Property *)pParent );
   return (HTypeProperty *) pProperty;
}